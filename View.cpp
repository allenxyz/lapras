#include "View.h"
#include "Utility.h"

#include <iostream>
#include <iomanip>
// #include <iterator>
#include <cassert>

using namespace std;

const int init_size_c = 25;
const double init_scale_c = 2.0;
const Point init_origin_c = Point(-10, -10);

//size needs to be in (6, 30]
const int min_size_c = 6;
const int max_size_c = 30;
const double min_scale_c = 0.0;

const int ycoordinate_width_c = 4;
const int xcoordinate_width_c = 6;
const int axis_spacing_c = 3;

View::View() {}

void View::update_location(const string& name, Point location) {
	object_locations[name] = location;
}

void View::update_ship_info(const string& name, double fuel, double course, double speed) {
	ship_infos[name] = {fuel, course, speed};
}

void View::update_remove(const string& name) {
	object_locations.erase(name);
	ship_infos.erase(name); 
}


void View::clear() {
	//simply cear all object_locations;
	object_locations.clear();
}

//fat interface functions
void View::draw() {
	throw Error("Cannot draw");
}
void View::set_size(int size_) {
	throw Error("Cannot set size");
}

void View::set_scale(double scale_) {
	throw Error("Cannot scale");
}

void View::set_origin(Point origin_) {
	throw Error("Cannot set origin");
}

void View::set_defaults() {
	throw Error("Cannot set default");
}


//protected stuff
vector<string> View::get_all_object_names() {
	vector<string> retVal;
	for (auto pair : object_locations) 
		retVal.push_back(pair.first);
	return retVal;
}

vector<string> View::get_all_ship_names() {
	vector<string> retVal;
	for (auto pair : ship_infos)
		retVal.push_back(pair.first);
	return retVal;
}

Point View::get_object_location(string object_name) {
	assert(object_locations.find(object_name) != object_locations.end());
	return object_locations[object_name];
}

View::Ship_info View::get_ship_info(string ship_name) {
	assert(ship_infos.find(ship_name) != ship_infos.end());
	return ship_infos[ship_name];
}