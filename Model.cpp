#include "Model.h"

#include "Sim_object.h"
#include "Island.h"
#include "Ship.h"
#include "View.h"
#include "Geometry.h"
#include "Ship_factory.h"
#include "Navigation.h"

#include "Utility.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <memory>

using namespace std;
using namespace std::placeholders;



Model::Model() : time(0) {
	//adding islands
	shared_ptr<Island> island_ptr(make_shared<Island>("Exxon", Point(10, 10), 1000, 200));
	track_island(island_ptr);
	island_ptr = make_shared<Island>("Shell", Point(0, 30), 1000, 200);
	track_island(island_ptr);
	island_ptr = make_shared<Island>("Bermuda", Point(20, 20));
	track_island(island_ptr);
	island_ptr = make_shared<Island>("Treasure_Island", Point(50, 5), 100, 5);
	track_island(island_ptr);

	//adding ships
	shared_ptr<Ship> ship_ptr;
	ship_ptr = create_ship("Ajax", "Cruiser", Point (15, 15));
	track_ship(ship_ptr);
	ship_ptr = create_ship("Xerxes", "Cruiser", Point (25, 25));
	track_ship(ship_ptr);
	ship_ptr = create_ship("Valdez", "Tanker", Point (30, 30));
	track_ship(ship_ptr);
}

//need to check the first two characters
bool Model::is_name_in_use(const string& name) const {
	//only place that needs to check the first two charracters
	auto found_itr = 
		find_if(all.begin(), all.end(), 
			//lambda that checks the first 2 characters of the name of the obj_ptr
			[name] (shared_ptr<Sim_object> obj_ptr) {
				if (obj_ptr -> get_name().at(0) == name.at(0) &&
					obj_ptr -> get_name().at(1) == name.at(1))
					return true;
				return false;
			});
	return found_itr != all.end();
}

//needs exact name
bool Model::is_island_present(const string& name) const {
	return islands.find(name) != islands.end();
}

//needs to be exact name
shared_ptr<Island> Model::get_island_ptr(const string& name) const {
	auto itr = islands.find(name);
	if (itr == islands.end()) throw Error("Island not found!");
	return itr -> second;
}

//needs exact name
bool Model::is_ship_present(const string& name) const {
	return ships.find(name) != ships.end();
}

void Model::add_ship(shared_ptr<Ship> ship_ptr) {
	track_ship(ship_ptr);
	ship_ptr -> broadcast_current_state();
}

//needs exact name
shared_ptr<Ship> Model::get_ship_ptr(const string& name) const {
	auto itr = ships.find(name);
	if (itr == ships.end()) throw Error("Ship not found!");
	return itr -> second;
}

void Model::describe() const {
	for (auto sim_obj_ptr : all) sim_obj_ptr -> describe();
}

void Model::update() {
	time++;
	for (auto sim_obj_ptr : all) sim_obj_ptr -> update();
}

void Model::attach(shared_ptr<View> view_ptr) {
	views.insert(view_ptr);
	for_each(all.begin(), all.end(), mem_fn(&Sim_object::broadcast_current_state));
}

void Model::detach(shared_ptr<View> view_ptr) {
	assert(!views.empty());
	views.erase(view_ptr);
}

void Model::notify_location(const string& name, Point location) {
	for_each(views.begin(), views.end(), bind(&View::update_location, _1, name, location));
	// View::update_location(name, location);
}

void Model::notify_ship_info(const string & name, Course_speed cs, double fuel) {
	for_each(views.begin(), views.end(), bind(&View::update_ship_info, _1, name, fuel, cs.course, cs.speed));
	// View::update_ship_info(name, fuel, cs.course, cs.speed);
}

void Model::notify_gone(const string& name) {
	for_each(views.begin(), views.end(), bind(&View::update_remove, _1, name));
	// View::update_remove(name);
}

void Model::track_ship(shared_ptr<Ship> ship_ptr) {
	all.insert(ship_ptr);
	ships[ship_ptr -> get_name()] = ship_ptr;
}
void Model::track_island(shared_ptr<Island> island_ptr) {
	all.insert(island_ptr);
	islands[island_ptr -> get_name()] = island_ptr;
}

void Model::remove_ship(std::shared_ptr<Ship> ship_ptr) {
	all.erase(ship_ptr);
	ships.erase(ship_ptr -> get_name());
}

Model* Model::get() {
	static Model singleton;
	return &singleton;
}

vector<shared_ptr<Island>> Model::get_all_islands() {
	vector<shared_ptr<Island>> island_vec;
	for (auto pair : islands) 
		island_vec.push_back(pair.second);

	return island_vec;
}