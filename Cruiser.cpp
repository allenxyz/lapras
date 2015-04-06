#include "Cruiser.h"

#include <iostream>

using namespace std;

const double init_fuel_c = 1000;
const double init_max_speed_c = 20.;
const double init_fuel_consumption_c = 10.;
const double init_resisitance_c = 6;
const double init_firepower_c = 3;
const double init_max_range_c = 15;

Cruiser::Cruiser(const string & name_, Point position_) : 
	Warship(name_, position_, init_fuel_c, init_max_speed_c	, 
			init_fuel_consumption_c, init_resisitance_c, 
			init_firepower_c, init_max_range_c) {
	// cout << "Cruiser " << get_name() << " constructed" << endl;
}

// Cruiser::~Cruiser() {
// 	// cout << "Cruiser " << get_name() << " destructed" << endl;
// }

void Cruiser::update() {
	Warship::update();
	if (is_attacking()) {
		if (target_in_range()) fire_at_target();
		else {
			cout << get_name() << " target is out of range" << endl;
			stop_attack();
		}
	}
}

void Cruiser::describe() const {
	cout << "\nCruiser ";
	Warship::describe();
}

void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
	Ship::receive_hit(hit_force, attacker_ptr);
	if (!is_afloat()) return;
	if (!is_attacking()) Warship::attack(attacker_ptr);
}