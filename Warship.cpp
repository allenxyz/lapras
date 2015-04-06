#include "Warship.h"
#include "Utility.h"

#include <iostream>
#include <cassert>
using namespace std;

Warship::Warship(const std::string& name_, Point position_, double fuel_capacity_, 
		double maximum_speed_, double fuel_consumption_, int resistance_,
		int firepower_, double maximum_range_) :
		Ship(name_, position_, fuel_capacity_, maximum_speed_, fuel_consumption_, resistance_), 
		firepower(firepower_),
		max_range(maximum_range_),
		warship_state(Warship_state::NOT_ATTACKING) {
	// cout << "Warship " << get_name() << " constructed" << endl;
}

// Warship::~Warship() {
// 	// cout << "Warship " << get_name() << " destructed"<<endl;
// }

void Warship::update() {
	Ship::update();
	if (warship_state == Warship_state::ATTACKING) {
		// assert(!target.expired());
		if (!is_afloat() || target.expired() || !(target.lock() -> is_afloat()))
			stop_attack();
		else
			cout << get_name() << " is attacking" << endl;
	}
}

void Warship::attack(shared_ptr<Ship> target_ptr) {
	if (!is_afloat()) throw Error("Cannot attack!");
	if (target_ptr == shared_from_this()) throw Error("Warship may not attack itself!");
	if (target_ptr == target.lock()) throw Error("Already attacking this target!");

	if (warship_state != Warship_state::NOT_ATTACKING || target.lock() != target_ptr) {
		target = target_ptr;
		warship_state = Warship_state::ATTACKING;
		cout << get_name() << " will attack " << target_ptr -> get_name() << endl;
	}
}

void Warship::stop_attack() {
	if (warship_state == Warship_state::NOT_ATTACKING) throw Error("Was not attacking!");
	cout << get_name() << " stopping attack" << endl;
	warship_state = Warship_state::NOT_ATTACKING;
	target.reset();
}

void Warship::describe() const {
	Ship::describe();
	if (warship_state == Warship_state::ATTACKING) {
		if (target.expired() || !(target.lock() -> is_afloat())) cout << "Attacking absent ship" << endl;
		else cout << "Attacking " << target.lock() -> get_name() << endl;
	}
}

bool Warship::is_attacking() const {
	return warship_state == Warship_state::ATTACKING;
}

void Warship::fire_at_target() {
	assert(!target.expired());
	cout << get_name() << " fires" << endl;
	target.lock() -> receive_hit(firepower, shared_from_this());
}

bool Warship::target_in_range() const {
	assert(!target.expired());
	double distance = cartesian_distance(get_location(), target.lock() -> get_location());
	return !(distance > max_range);
}

//note to self: taget might be nullptr/empty shared_ptr!!
weak_ptr<Ship> Warship::get_target() const {
	return target;
}