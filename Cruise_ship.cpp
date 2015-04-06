#include "Cruise_ship.h"
#include "Geometry.h"
#include "Model.h"
#include "Island.h"

#include <iostream>
#include <cassert>
#include <limits>

using namespace std;


const double init_fuel_c = 500.;
const double init_max_speed_c = 15.;
const double init_fuel_consumption_c = 2.;
const double init_resistance_c = 0.;

const double roundoff_error_c = 0.005;


Cruise_ship::Cruise_ship(const string name_, Point position_) :
	Ship(name_, position_, init_fuel_c, init_max_speed_c, init_fuel_consumption_c, init_resistance_c),
	num_visited_islands(0),
	cruise_state(Cruise_state::NOT_ON_CRUISE) {

	//in this project there is no way to insert a island - just initialize this array when ship is created
	auto shared_ptr_islands = Model::get() -> get_all_islands();
	
	//convert shared_ptr to weak_ptr
	for (size_t i = 0; i < shared_ptr_islands.size(); ++i) 
		all_islands.push_back(shared_ptr_islands.at(i));
	
	visited_island = vector<bool>(all_islands.size(), false);
}

void Cruise_ship::set_destination_position_and_speed(Point destination, double speed) {
	if (cruise_state != Cruise_state::NOT_ON_CRUISE) cancel_cruise();

	Ship::set_destination_position_and_speed(destination, speed);

	bool start_cruise = false;
	for (size_t i = 0; i < all_islands.size(); ++i) {
		auto shared_island_ptr = all_islands.at(i).lock();
		assert(shared_island_ptr);
		if (cartesian_distance(shared_island_ptr -> get_location(), destination) < roundoff_error_c)  {
			// this is the island
			start_cruise = true;
			cruise_begin_index = i;
		}
	}

	if (start_cruise) {
		cruise_state = Cruise_state::MOVING_TO_NEXT;
		next_location_index = cruise_begin_index;
		crusing_speed = speed;

		shared_ptr<Island> begin_island = all_islands.at(cruise_begin_index).lock();
		assert(begin_island);
		cout << get_name() << " will visit "  
			 << begin_island -> get_name() << endl;

		cout << get_name() << " cruise will start and end at " 
			 << begin_island -> get_name() << endl;
	}
}

void Cruise_ship::set_course_and_speed(double course, double speed) {
	if (cruise_state != Cruise_state::NOT_ON_CRUISE) cancel_cruise();
	Ship::set_course_and_speed(course, speed);
}

void Cruise_ship::stop() {
	if (cruise_state != Cruise_state::NOT_ON_CRUISE) cancel_cruise();
	Ship::stop();
}

void Cruise_ship::update() {
	Ship::update();
	shared_ptr<Island> island_ptr;
	switch (cruise_state) {
		case Cruise_state::NOT_ON_CRUISE: break; //do nothing
		case Cruise_state::REFUEL:
			//should be docked at the next island - refuel here and do nothing further
			assert(is_docked());
			assert(get_docked_Island());
			refuel();
			cruise_state = Cruise_state::WAIT;
			break;
		case Cruise_state::WAIT:
			//do nothing further
			cruise_state = Cruise_state::PREP_NEXT_LOC;
			break;
		case Cruise_state::PREP_NEXT_LOC:
			//update been_to
			//find the next island loaction and set next_location to it
			if (find_next_stop())
				cruise_state = Cruise_state::MOVING_TO_NEXT;
			else 
				cruise_state = Cruise_state::RETURNING;

			island_ptr = all_islands.at(next_location_index).lock();
			assert(island_ptr);
			Ship::set_destination_position_and_speed(island_ptr -> get_location(), 
													 crusing_speed);

			cout << get_name() << " will visit "  << island_ptr -> get_name() << endl;
			break;
		case Cruise_state::MOVING_TO_NEXT:
			//check if we have arrived
			//if yes dock and update cruise_state
			//else do nothing
			island_ptr = all_islands.at(next_location_index).lock();
			assert(island_ptr);
			if (!is_moving() && can_dock(island_ptr)) { //arrived
				//could start of docked at the starting location
				if (get_docked_Island() != island_ptr) dock(island_ptr);
				cruise_state = Cruise_state::REFUEL;
			}
			break;
		case Cruise_state::RETURNING:
			//check if arrived
			//if so set next state to be not_on_cruise
			//announce end of cruise
			island_ptr = all_islands.at(cruise_begin_index).lock();
			assert(island_ptr);
			if (!is_moving() && can_dock(island_ptr)) {
				dock(island_ptr);
				cout << get_name() << " cruise is over at " << island_ptr -> get_name() << endl;
				end_cruise();
			}
			break;
	}
}

void Cruise_ship::describe() const {
	cout << "\nCruise_ship ";
	Ship::describe();
	auto next_location = all_islands.at(next_location_index).lock();
	switch (cruise_state) {
		case Cruise_state::NOT_ON_CRUISE: break;
		case Cruise_state::REFUEL:
		case Cruise_state::WAIT:
		case Cruise_state::PREP_NEXT_LOC:
			cout << "Waiting during cruise at " << next_location -> get_name() << endl;
			break;
		case Cruise_state::MOVING_TO_NEXT: 
		case Cruise_state::RETURNING: 
			cout << "On cruise to "<< next_location -> get_name() << endl;
			break;
	}
	
}

bool Cruise_ship::find_next_stop() {
	num_visited_islands++;
	assert(!visited_island.at(next_location_index));
	visited_island.at(next_location_index) = true;

	if (num_visited_islands == all_islands.size()) { //been everywhere
		next_location_index = cruise_begin_index;
		return false;
	}

	//start the min dist at max(double) - can't pick one to start at :(....
	double min_dist = numeric_limits<double>::max();
	for (size_t i = 0; i < all_islands.size(); ++i) {
		if (visited_island.at(i)) continue; //been here
		shared_ptr<Island> island_ptr = all_islands.at(i).lock();
		assert(island_ptr);
		double dist = cartesian_distance(island_ptr -> get_location(), get_location());
		if (dist < min_dist) {
			next_location_index = i;
			min_dist = dist;
		}
	}
	return true;
}

void Cruise_ship::end_cruise() {
	visited_island = vector<bool>(all_islands.size(), false);
	num_visited_islands = 0;

	cruise_state = Cruise_state::NOT_ON_CRUISE;
}

void Cruise_ship::cancel_cruise() {
	cout << get_name() << " canceling current cruise" << endl;
	end_cruise();
}