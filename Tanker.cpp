#include "Tanker.h"
#include "Island.h"
#include "Utility.h"

#include <iostream>
#include <cassert>

using namespace std;

const double init_fuel_c = 100.;
const double init_max_speed_c = 10.;
const double init_fuel_consumption_c = 2.;
const double init_resistance_c = 0.;
const double init_cargo_capacity_c = 1000.;
const double init_cargo_c = 0.;

const double roundoff_error_c = 0.005;

Tanker::Tanker(const string& name_, Point position_) : 
	Ship(name_, position_, init_fuel_c, init_max_speed_c, init_fuel_consumption_c, init_resistance_c),
	tanker_state(Tanker_state::NO_CARGO_DESTINATION),
	max_cargo(init_cargo_capacity_c),
	cargo(init_cargo_c)   {
	// cout << "Tanker " << get_name() << " constructed" << endl;
}

// Tanker::~Tanker() {
// 	// cout << "Tanker " << get_name() << " destructed" << endl;
// }

void Tanker::set_destination_position_and_speed(Point destination, double speed) {
	if (tanker_state != Tanker_state::NO_CARGO_DESTINATION) throw Error("Tanker has cargo destinations!"); 
	Ship::set_destination_position_and_speed(destination, speed);
}

void Tanker::set_course_and_speed(double course, double speed) {
	if (tanker_state != Tanker_state::NO_CARGO_DESTINATION) throw Error("Tanker has cargo destinations!"); 
	Ship::set_course_and_speed(course, speed);
}

void Tanker::set_load_destination(shared_ptr<Island> island_ptr) {
	if (tanker_state != Tanker_state::NO_CARGO_DESTINATION) throw Error("Tanker has cargo destinations!");
	load = island_ptr;
	if (island_ptr == unload) throw Error("Load and unload cargo destinations are the same!");

	cout << get_name() << " will load at " << island_ptr -> get_name() << endl;
	set_destination_helper(island_ptr);
}

void Tanker::set_unload_destination(shared_ptr<Island> island_ptr) {
	if (tanker_state != Tanker_state::NO_CARGO_DESTINATION) throw Error("Tanker has cargo destinations!");
	unload = island_ptr;
	if (island_ptr == load) throw Error("Load and unload cargo destinations are the same!");

	cout << get_name() << " will unload at " << island_ptr -> get_name() << endl;
	set_destination_helper(island_ptr);
}

// due to the extreme simiarlities between set_load_destination and set_unload_destination
// creating a helper function for set_destinations - need a boolean variable to know which
// shared_ptr<Island> to update and the correct word to output (load or unload)
void Tanker::set_destination_helper(shared_ptr<Island> island_ptr) {
	if (unload && load) {
		if (is_docked()) {
			//must be docked somewhere
			auto docked_island = get_docked_Island();
			assert(docked_island);
			if (docked_island == load) tanker_state = Tanker_state::LOADING;
			//must be at unload
			else if (docked_island == unload) tanker_state = Tanker_state::UNLOADING;
			else assert(false); //saftey net - this just helps me debug
		}
		else if (!is_moving() && cargo == 0 && can_dock(load)) {
			dock(load);
			tanker_state = Tanker_state::LOADING;
		}
		else if (!is_moving() && cargo > 0 && can_dock(unload)) {
			dock(unload);
			tanker_state = Tanker_state::UNLOADING;
		}
		//must be moving
		else if (cargo == 0) {
			Ship::set_destination_position_and_speed(load -> get_location(), get_maximum_speed());
			tanker_state = Tanker_state::MOVING_TO_LOAD;
		}
		else if (cargo > 0) {
			Ship::set_destination_position_and_speed(unload -> get_location(), get_maximum_speed());
			tanker_state = Tanker_state::MOVING_TO_UNLOAD;
		}
		else  assert(false); 
	}
}

void Tanker::stop() {
	Ship::stop();
	reset_to_no_cargo_destination();
}

void Tanker::reset_to_no_cargo_destination() {
	load.reset();
	unload.reset();
	tanker_state = Tanker_state::NO_CARGO_DESTINATION;
	cout << get_name() << " now has no cargo destinations" << endl;
}


void Tanker::update() {
	Ship::update();
	if (!can_move()) reset_to_no_cargo_destination();
	else if (tanker_state == Tanker_state::NO_CARGO_DESTINATION) {} //do nothing
	else if (tanker_state == Tanker_state::MOVING_TO_LOAD) {
		assert(load && unload);
		if (!is_moving() && can_dock(load)) {
			dock(load);
			tanker_state = Tanker_state::LOADING;
		}
	}
	else if (tanker_state == Tanker_state::MOVING_TO_UNLOAD) {
		assert(load && unload);
		if (!is_moving() && can_dock(unload)) {
			dock(unload);
			tanker_state = Tanker_state::UNLOADING;
		}
	}
	else if (tanker_state == Tanker_state::LOADING) {
		assert(load && unload);
		refuel();
		double delta_cargo = max_cargo - cargo;
		if (delta_cargo < roundoff_error_c) {
			cargo = max_cargo;
			Ship::set_destination_position_and_speed(unload -> get_location(), get_maximum_speed());
			tanker_state = Tanker_state::MOVING_TO_UNLOAD;
		}
		else {
			double supplied_cargo = load -> provide_fuel(delta_cargo);
			cargo += supplied_cargo;
			cout << get_name() << " now has " << cargo << " of cargo" << endl;
		}
	}
	else if (tanker_state == Tanker_state::UNLOADING) {
		assert(load && unload);
		if (cargo == 0.) {
			Ship::set_destination_position_and_speed(load -> get_location(), get_maximum_speed());
			tanker_state = Tanker_state::MOVING_TO_LOAD;
		}
		else {
			unload -> accept_fuel(cargo);
			cargo = 0.;
		}
	}
	else assert(false); //safety net
}


void Tanker::describe() const {
	cout << "\nTanker ";
	Ship::describe();
	cout << "Cargo: " << cargo << " tons";
	switch (tanker_state) {
		case Tanker_state::NO_CARGO_DESTINATION: 
			cout << ", no cargo destinations";
			break;
		case Tanker_state::LOADING: 
			cout << ", loading";
			break;
		case Tanker_state::UNLOADING:
			cout << ", unloading";
			break;
		case Tanker_state::MOVING_TO_LOAD:
			cout << ", moving to loading destination";
			break;
		case Tanker_state::MOVING_TO_UNLOAD:
			cout << ", moving to unloading destination";
			break;
		default: //safety net
			assert(false); //tells the AG to give me a different message
			break;
	}
	cout << endl;
}