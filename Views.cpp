#include "Views.h"
#include "Utility.h"
#include "Navigation.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <iterator>

#include <cassert>

using namespace std;

/***** helper functions ********/

// same code from project4
// Calculate the cell subscripts corresponding to the supplied location parameter, 
// using the current size, scale, and origin of the display. 
// This function assumes that origin is a  member variable of type Point, 
// scale is a double value, and size is an integer for the number of rows/columns 
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool get_subscripts(int &ix, int &iy, Point location, Point origin, double scale, int size)
{
	// adjust with origin and scale
	Cartesian_vector subscripts = (location - origin) / scale;
	// truncate coordinates to integer after taking the floor
	// floor function will produce integer smaller than even for negative values, 
	// so - 0.05 => -1., which will be outside the array.
	ix = int(floor(subscripts.delta_x));
	iy = int(floor(subscripts.delta_y));
	// if out of range, return false
	if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size)) {
		return false;
		}
	else
		return true;
}


/*********************** 
 *******Map view********
 ***********************/

const int map_init_size_c = 25;
const double map_init_scale_c = 2.0;
const Point map_init_origin_c = Point(-10, -10);

//size needs to be in (6, 30]
const int map_min_size_c = 6;
const int map_max_size_c = 30;
const double map_min_scale_c = 0.0;

const int map_ycoordinate_width_c = 4;
const int map_xcoordinate_width_c = 6;
const int map_axis_spacing_c = 3;

Map_view::Map_view() : origin(map_init_origin_c), 
					   scale(map_init_scale_c), 
					   size(map_init_size_c) {}

void Map_view::draw() {
	//save cout settings
	ios::fmtflags old_settings = cout.flags();
	int old_precision = cout.precision();

	//populate what will be in each cell
	// i will avoid using iomanip functions if i don't need it - too much nit pikcy bookkeeping
	// make sure that each string within this vector is of 2 characters and 2 characters ONLY
	/*
		PRINT_MAP
		    i\j	0 		1 		2
			0	(0,0)	(1,1)	(2,0)
			1   (0,1)	(1,2)	(2,1)
			2   (0,2)	(1,3)	(2,2)
	*/
	vector<vector<string>> print_map(size, vector<string>(size, ". "));
	vector<string> outside; //ships that are outside the map
	vector<string> all_names = get_all_object_names(); //names of all objects to print
	for (auto name : all_names) {
		Point object_location = get_object_location(name);

		int x, y;
		//get the x and y coordinate of each ship in the map of object_locations
		if (!get_subscripts(x, y, object_location, origin, scale, size)) {
			outside.push_back(name);
			continue;
		}

		//only/first object at this co-ordinate
		if (print_map.at(y).at(x) == ". ") 
			print_map.at(y).at(x) = name.substr(0, 2);
		//another object is on the same coordinate
		else
			print_map.at(y).at(x) = "* ";
	}


	//printing pre-map information
	//printing size, scale and origin
	cout << fixed << setprecision(2); //scale and origin round off to 2 d.p.
	cout << "Display size: " << size << ", scale: " << scale << ", origin: " << origin << endl;

	//printing outside ships
	if (!outside.empty()) {
		cout << outside.at(0); //english grammar is the bane of prgramming :(
							   //need to output the first one manually to get the commas right
		//already printed one, start on the second thing in outside
		for (auto itr = ++outside.begin(); itr != outside.end(); ++itr)
			cout << ", " << *itr;
		cout << " outside the map" << endl;
	}


	//printing map + axis
	cout << fixed << setprecision(0);
	// rows need to be printed in reverse due to the way print_map was populated
	for (int i = print_map.size() - 1; i >= 0; --i) { 
		//y-axis mark only needs to be shown once every axis_spacing_c times
		// if i is a multiple of axis_spacing_c  - print the y-axis mark
		if (!(i % map_axis_spacing_c)) //this row needs to print a y axis mark
			cout << setw(map_ycoordinate_width_c) << (i * scale) + origin.y;
		else  //this dosn't need a y-axis mark
			cout << setw(map_ycoordinate_width_c) << " ";
		cout << " ";

		//printing rest of row row - this is easy :P
		for (size_t j = 0; j < print_map.size(); ++j) 
			cout << print_map.at(i).at(j);
		cout << endl;
	}


	//last row needs to print the x-axis
	int xvalue = origin.x;
	int max_xvalue = origin.x + (size*scale);
	while (xvalue < max_xvalue) {
		cout << setw(map_xcoordinate_width_c) << xvalue;
		xvalue += (scale * map_axis_spacing_c);
	}
	cout << endl;



	//restore old cout settings
	cout.flags(old_settings);
	cout.precision(old_precision);
}

void Map_view::set_size(int size_) {
	if (size_ <= map_min_size_c) throw Error("New map size is too small!");
	if (size_ > map_max_size_c) throw Error("New map size is too big!");
	size = size_;
}

void Map_view::set_scale(double scale_) {
	if (scale_ <= 0.0) throw Error("New map scale must be positive!");
	scale = scale_;
}

void Map_view::set_origin(Point origin_) {
	origin = origin_;
}

void Map_view::set_defaults() {
	origin = map_init_origin_c;
	scale = map_init_scale_c;
	size = map_init_size_c;
}


/***************************
 *******Sailing view********
 ***************************/

const int column_width_c = 10;

void Sailing_data_view::draw() {
	//headers
	cout << "----- Sailing Data -----" << endl;
	cout << setw(column_width_c) << "Ship" 
		 << setw(column_width_c) << "Fuel" 
		 << setw(column_width_c) << "Course" 
		 << setw(column_width_c) << "Speed" << endl;

	//print each ship name + information
	vector<string> all_ship = get_all_ship_names();
	for (string name : all_ship) {
		Ship_info si = get_ship_info(name);
		cout << setw(column_width_c) << name
		     << setw(column_width_c) << si.fuel
		     << setw(column_width_c) << si.course
		     << setw(column_width_c) << si.speed << endl;
	}
}


/***************************
 *******Bridge view*********
 ***************************/
const int bridge_rows_c = 3;
const int bridge_columns_c = 19;
const Point bridge_origin_c = Point(-90,0);
const int bridge_scale_c = 10;
const int bridge_size_c = 20;
const int bridge_max_range_c = 20;

const int bridge_column_width_c = 5;
const int bridge_x_min_c = -90;
const int bridge_x_max_c = 90;
const int bridge_x_increment_c = 30;
const int bridge_x_spacing_c = 6;

//wraps around an angle if it's too large/too small
void wrap(double & angle) {
	if (angle < -180) angle += 360;
	else if (angle > 180) angle -= 360;
}


Bridge_view::Bridge_view(const string& ship_name_) : ship_name(ship_name_) {}


void Bridge_view::update_remove(const string& name) {
	if (ship_name == name)  {
		sunk = true;
		ship_loc = get_object_location(ship_name);
	}
	View::update_remove(name);
}

void Bridge_view::draw() {
	vector<vector<string>> printMap(bridge_rows_c, vector<string>(bridge_columns_c, ". "));

	if (sunk) {
		//ship sunk, just populate printMap with "w-"'s
		cout << "Bridge view from " << ship_name << " sunk at " << ship_loc << endl;
		printMap = vector<vector<string>> (bridge_rows_c, vector<string>(bridge_columns_c, "w-"));
	}
	
	else {
		auto all_names = get_all_object_names();		
		double ownship_heading = get_ship_info(ship_name).course;
		cout << "Bridge view from " << ship_name 
			 << " position " << get_object_location(ship_name)
			 << " heading " << ownship_heading << endl;

		//populating map
		for (string name : all_names) {
			Compass_position cp(get_object_location(ship_name), get_object_location(name));
			//ignore same location / too far
			if (cp.range < round_off_error_c || cp.range > bridge_max_range_c) 
				continue; 

			double angle = cp.bearing - ownship_heading;
			wrap(angle);

			int x, y; //we don't care about the y though...
			get_subscripts(x, y, Point(angle, 0), bridge_origin_c, bridge_scale_c, bridge_size_c);
			if (x >= 0 && x < bridge_columns_c) {
				if (printMap.at(0).at(x) == ". ")
					printMap.at(0).at(x) = name.substr(0, 2);
				else 
					printMap.at(0).at(x) = "**";
			}
		}
	}

	//printing the map
	for (int i = printMap.size() - 1; i >= 0; --i) { //print rows in reverse
		cout << setw(bridge_column_width_c) << " ";
		for (size_t j = 0; j < printMap.at(i).size(); ++j)
			cout << printMap.at(i).at(j);
		cout << endl;
	}
	
	//printing x-axis
	for (int print_x_mark = bridge_x_min_c; print_x_mark <= bridge_x_max_c; print_x_mark += bridge_x_increment_c) {
		cout << setw(bridge_x_spacing_c) << print_x_mark;
	}
	cout << endl;

}