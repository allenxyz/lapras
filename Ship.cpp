#include "Ship.h"
#include "Island.h"
#include "Model.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <cassert>

using namespace std;

const double docking_distance_c = 0.1;
const double roundoff_error_c = 0.005;

Ship::Ship(const string& name_, Point position_, double fuel_capacity_, 
		double maximum_speed_, double fuel_consumption_, int resistance_)
		: Sim_object(name_), 
		  track_base(position_), 
		  fuel(fuel_capacity_), 
		  max_speed(maximum_speed_), 
		  fuel_consumption(fuel_consumption_), 
		  resistance(resistance_),
		  ship_state(Ship_state::STOPPED), 
		  max_fuel(fuel_capacity_) {
}

/*
Define the destructor function even if it was declared as a pure virtual function.
This seems odd, because pure virtual functions are usually not defined in the class
that declares them. But this is often done as a way to make a class abstract, 
if there is no other virtual function that makes sense to mark as pure. Here we
are defining it just to get the destructor message output.
*/

// Ship::~Ship() {
// 	// cout << "Ship "  << get_name() << " destructed" << endl;
// }

/* public function Defintions */
bool Ship::can_move() const {
	return !(!is_afloat() || ship_state == Ship_state::DEAD_IN_THE_WATER);
}

bool Ship::is_moving() const {
	return (ship_state == Ship_state::MOVING_TO_POSITION || 
			ship_state == Ship_state::MOVING_ON_COURSE);
}

bool Ship::is_docked() const {
	return ship_state == Ship_state::DOCKED;
}

bool Ship::is_afloat() const {
	return ship_state != Ship_state::SUNK;
	// return !(ship_state == Ship_state::SINKING ||
	// 		ship_state == Ship_state::SUNK || 
	// 		ship_state == Ship_state::ON_THE_BOTTOM);
}

// bool Ship::is_on_the_bottom() const {
// 	return ship_state == Ship_state::ON_THE_BOTTOM;
// }

bool Ship::can_dock(shared_ptr<Island> island_ptr) const {
	if (ship_state != Ship_state::STOPPED) return false;
	double dist = cartesian_distance(get_location(), island_ptr -> get_location());
	return dist <= docking_distance_c; 
}

void Ship::update() {
	cout << get_name();

	if (!is_afloat()) { //sinking
		//HAS to be 'sunk'
		cout << " sunk";
		// switch (ship_state) {
			// case Ship_state::SINKING: 	
			// 	ship_state = Ship_state::SUNK;
			// 	cout << " sunk";
			// 	Model::get() -> notify_gone(get_name());
			// 	break;
			// case Ship_state::SUNK: 
			// 	cout << " sunk";
			// 	break;
			// case Ship_state::ON_THE_BOTTOM: 
			// 	cout << " on the bottom";
			// 	break;
		// 	default:  //saftey net + supress those dam warnings!
		// 		assert(false); //use assert to let AG give me a differnet error
		// 		break;
		// }
	}
	else { //afloat
		// if (resistance < 0) {
		// 	ship_state = Ship_state::SINKING;
		// 	track_base.set_speed(0.);
		// 	cout << " sinking";
		// }
		//get the correct aflat update

		switch (ship_state) {
			case Ship_state::MOVING_ON_COURSE: 
			case Ship_state::MOVING_TO_POSITION:
				calculate_movement();
				cout << " now at " << get_location();
				Model::get() -> notify_location(get_name(), get_location());
				break;
			case Ship_state::STOPPED: 
				cout << " stopped at " << get_location();
				break;
			case Ship_state::DOCKED: 
				assert(docked);
				cout << " docked at " << docked -> get_name();
				break;
			case Ship_state::DEAD_IN_THE_WATER: 
				cout << " dead in the water at " << get_location();
				break;
			default: //safety net
				assert(false);
				break;
		}
	}
	cout << endl;

	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::describe() const {
	cout << get_name() << " at " << get_location();
	if (!is_afloat()) {
		switch (ship_state) {
			// case Ship_state::SINKING: 
			// 	cout << " sinking";
			// 	break;
			case Ship_state::SUNK: 
				cout << " sunk";
				break;
			// case Ship_state::ON_THE_BOTTOM: 
			// 	cout << " on the bottom";
			// 	break;
			default: //help debugging - should not happen
				assert(false);
				break;
		}
	}
	else {
		cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
		switch (ship_state) {
			case Ship_state::MOVING_TO_POSITION: 
				cout << "Moving to " << destination << " on " << track_base.get_course_speed();
				break;
			case Ship_state::MOVING_ON_COURSE: 
				cout << "Moving on " << track_base.get_course_speed();
				break;
			case Ship_state::DOCKED: 
				assert(docked);
				cout << "Docked at " << docked -> get_name();
				break;
			case Ship_state::STOPPED:
				cout << "Stopped";
				break;
			case Ship_state::DEAD_IN_THE_WATER:
				cout << "Dead in the water";
				break;
			default: //saftey net
				assert(false);
				break;
		}
	}
	cout << endl;
}

void Ship::broadcast_current_state() {
	Model::get() -> notify_location(get_name(), get_location());
	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::set_destination_position_and_speed(Point destination_position, double speed) {
	if (!can_move()) throw Error("Ship cannot move!");
	if (speed > max_speed) throw Error("Ship cannot go that fast!");

	Compass_vector cv(get_location(), destination_position);
	track_base.set_course_speed(Course_speed(cv.direction, speed));

	//check if docked!!
	if (ship_state == Ship_state::DOCKED) docked.reset(); 
	destination = destination_position;
	ship_state = Ship_state::MOVING_TO_POSITION;
	cout << get_name() << " will sail on " << track_base.get_course_speed() << " to " << destination_position << endl;

	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::set_course_and_speed(double course, double speed) {
	if (!can_move()) throw Error("Ship cannot move!");
	if (speed > max_speed) throw Error("Ship cannot go that fast!");
	
	track_base.set_course_speed(Course_speed(course, speed));
	
	if (ship_state == Ship_state::DOCKED) docked.reset();
	ship_state = Ship_state::MOVING_ON_COURSE;
	cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;

	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::stop() {
	if (!can_move()) throw Error("Ship cannot move!");

	track_base.set_speed(0.);

	if (ship_state == Ship_state::DOCKED) docked.reset();
	ship_state = Ship_state::STOPPED;
	cout << get_name() << " stopping at " << get_location() << endl;

	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::dock(shared_ptr<Island> island_ptr) {
	if (!can_dock(island_ptr)) throw Error("Can't dock!");

	track_base.set_position(island_ptr -> get_location());
	Model::get() -> notify_location(get_name(), get_location());

	docked = island_ptr;
	ship_state = Ship_state::DOCKED;
	cout << get_name() << " docked at " << island_ptr -> get_name() << endl;

	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::refuel() {
	if (ship_state != Ship_state::DOCKED) throw Error("Must be docked!");
	assert(docked);
	double delta_fuel = max_fuel - fuel;
	if (delta_fuel < roundoff_error_c) fuel = max_fuel;
	else {
		fuel += docked -> provide_fuel(delta_fuel);
		cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
	}
	Model::get() -> notify_ship_info(get_name(), track_base.get_course_speed(), fuel);
}

void Ship::set_load_destination(shared_ptr<Island> island_ptr) {
	throw Error("Cannot load at a destination!");
}

void Ship::set_unload_destination(shared_ptr<Island> island_ptr) {
	throw Error("Cannot unload at a destination!");
}

void Ship::attack(shared_ptr<Ship> in_target_ptr) {
	throw Error("Cannot attack!");
}

void Ship::stop_attack() {
	throw Error("Cannot attack!");
}

void Ship::receive_hit(int hit_force, shared_ptr<Ship>) {
	resistance -= hit_force;
	cout << get_name() << " hit with " <<  hit_force << ", resistance now " << resistance << endl;
	if (resistance < 0) {
		cout << get_name() << " sunk" << endl;
		ship_state = Ship_state::SUNK;
		track_base.set_speed(0.);
		Model::get() -> notify_gone(get_name());
		Model::get() -> remove_ship(shared_from_this());
	}
}

/* Private Function Definitions */

/*
Calculate the new position of a ship based on how it is moving, its speed, and
fuel state. This function should be called only if the state is 
MOVING_TO_POSITION or MOVING_ON_COURSE.

Track_base has an update_position(double time) function that computes the new position
of an object after the specified time has elapsed. If the Ship is going to move
for a full time unit (one hour), then it will get go the "full step" distance,
so update_position would be called with time = 1.0. If we can move less than that,
e.g. due to not enough fuel, update position  will be called with the corresponding
time less than 1.0.

For clarity in specifying the computation, this code assumes the specified private variable names, 
but you may change the variable names or state names if you wish (e.g. movement_state).
*/
void Ship::calculate_movement()
{
	// Compute values for how much we need to move, and how much we can, and how long we can,
	// given the fuel state, then decide what to do.
	double time = 1.0;	// "full step" time
	// get the distance to destination
	double destination_distance = cartesian_distance(get_location(), destination);
	// get full step distance we can move on this time step
	double full_distance = track_base.get_speed() * time;
	// get fuel required for full step distance
	double full_fuel_required = full_distance * fuel_consumption;	// tons = nm * tons/nm
	// how far and how long can we sail in this time period based on the fuel state?
	double distance_possible, time_possible;
	if(full_fuel_required <= fuel) {
		distance_possible = full_distance;
		time_possible = time;
		}
	else {
		distance_possible = fuel / fuel_consumption;	// nm = tons / tons/nm
		time_possible = (distance_possible / full_distance) * time;
		}
	
	// are we are moving to a destination, and is the destination within the distance possible?
	if(ship_state == Ship_state::MOVING_TO_POSITION && destination_distance <= distance_possible) {
		// yes, make our new position the destination
		track_base.set_position(destination);
		// we travel the destination distance, using that much fuel
		double fuel_required = destination_distance * fuel_consumption;
		fuel -= fuel_required;
		track_base.set_speed(0.);
		ship_state = Ship_state::STOPPED;
		}
	else {
		// go as far as we can, stay in the same movement state
		// simply move for the amount of time possible
		track_base.update_position(time_possible);
		// have we used up our fuel?
		if(full_fuel_required >= fuel) {
			fuel = 0.0;
            track_base.set_speed(0.);
			ship_state = Ship_state::DEAD_IN_THE_WATER;
			}
		else {
			fuel -= full_fuel_required;
			}
		}
}


