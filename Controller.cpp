#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Views.h"
#include "Ship.h"
#include "Island.h"
#include "Geometry.h"
#include "Ship_factory.h"

#include "Utility.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

Controller::Controller() {
	//populate command maps - need to do it manually :(
	view_commands["default"] = &Controller::view_default_command;
	view_commands["size"] = &Controller::view_size_command;
	view_commands["zoom"] = &Controller::view_zoom_command;
	view_commands["pan"] = &Controller::view_pan_command;
	view_commands["show"] = &Controller::view_show_command;
	view_commands["open_map_view"] = &Controller::view_open_map_command;
	view_commands["close_map_view"] = &Controller::view_close_map_command;
	view_commands["open_sailing_view"] = &Controller::view_open_sailing_command;
	view_commands["close_sailing_view"] = &Controller::view_close_sailing_command;
	view_commands["open_bridge_view"] = &Controller::view_open_bridge_command;
	view_commands["close_bridge_view"] = &Controller::view_close_bridge_command;

	model_commands["status"] = &Controller::model_status_command;
	model_commands["go"] = &Controller::model_go_command;
	model_commands["create"] = &Controller::model_create_command;

	ship_commands["course"] = &Controller::ship_course_command;
	ship_commands["position"] = &Controller::ship_position_command;
	ship_commands["destination"] = &Controller::ship_destination_command;
	ship_commands["load_at"] = &Controller::ship_load_at_command;
	ship_commands["unload_at"] = &Controller::ship_unload_at_command;
	ship_commands["dock_at"] = &Controller::ship_dock_at_command;
	ship_commands["attack"] = &Controller::ship_attack_command;
	ship_commands["refuel"] = &Controller::ship_refuel_command;
	ship_commands["stop"] = &Controller::ship_stop_command;
	ship_commands["stop_attack"] = &Controller::ship_stop_attack_command;
}



void Controller::run() {
	View_info vi; //start off with empty views

	while (true) {
		try {
			string input;
			cout << "\nTime " << Model::get() -> get_time() << ": Enter command: ";
			cin >> input;


			//quit command
			if (input == "quit") break;

			//ship commands
			else if (Model::get() -> is_ship_present(input)) {
				//input should be ship name
				shared_ptr<Ship> ship_ptr = Model::get() -> get_ship_ptr(input);
				
				//next input should be ship command
				cin >> input; 
				auto command_func_ptr = ship_commands[input];
				if (command_func_ptr) {//function pointer found
					(this ->*command_func_ptr)(ship_ptr);
					continue;
				}

				//not a valid ship command
				ship_commands.erase(input); //indexing inserted it
				throw Error("Unrecognized command!");
			}

			//View or Model command
			else {
				//check if it is a view command
				auto view_func_ptr = view_commands[input];
				if (view_func_ptr) { //function pointer found - view command
					(this ->*view_func_ptr)(vi);
					continue;
				}

				//not a view command
				view_commands.erase(input); //indexing added it

				//check if it's a model command
				auto model_func_ptr = model_commands[input];
				if (model_func_ptr) { //model command
					(this ->*model_func_ptr)();
					continue;
				}

				//not a model command
				model_commands.erase(input);

				//invalud command
				throw Error("Unrecognized command!");
			}
		}
		catch (Error & e) {
			cout << e.what() << endl;
			cin.clear();
			//skip the rest of the line
			while (cin.get() != '\n');
		}
	} //end while(true)

	//quit happened
	cout << "Done" << endl;
}

/*************
 View commands
 *************/
void Controller::view_default_command(View_info& views) {
	if (!views.map_view) throw Error("Map view is not open!");
	views.map_view -> set_defaults();
}

void Controller::view_size_command(View_info& views) {
	int size = read_int();
	if (!views.map_view) throw Error("Map view is not open!");
	views.map_view -> set_size(size);
}

void Controller::view_zoom_command(View_info& views) {
	double scale = read_double();
	if (!views.map_view) throw Error("Map view is not open!");
	views.map_view -> set_scale(scale);
}

void Controller::view_pan_command(View_info& views) {
	Point new_origin = read_point();
	if (!views.map_view) throw Error("Map view is not open!");
	views.map_view -> set_origin(new_origin);
}

void Controller::view_show_command(View_info& views) {
	for_each(views.all_views.begin(), views.all_views.end(), mem_fn(&View::draw));
}

void Controller::view_open_map_command(View_info& views) {
	if (views.map_view) throw Error("Map view is already open!");
	//create a map_view
	shared_ptr<View> new_view = make_shared<Map_view>();
	
	Model::get() -> attach(new_view);
	
	views.all_views.push_back(new_view);
	views.map_view = new_view;
}

void Controller::view_close_map_command(View_info& views) {
	if (!views.map_view) throw Error("Map view is not open!");

	Model::get() -> detach(views.map_view);
	
	views.all_views.remove(views.map_view);
	views.map_view.reset();

}
void Controller::view_open_sailing_command(View_info& views) {
	if (views.sailing_view)	throw Error("Sailing data view is already open!");
	//create sailing_view
	shared_ptr<View> new_view = make_shared<Sailing_data_view>();
	
	Model::get() -> attach(new_view);
	
	views.all_views.push_back(new_view);
	views.sailing_view = new_view;
}

void Controller::view_close_sailing_command(View_info& views) {
	if (!views.sailing_view) throw Error("Sailing data view is not open!");

	Model::get() -> detach(views.sailing_view);
	
	views.all_views.remove(views.sailing_view);
	views.sailing_view.reset();
}

void Controller::view_open_bridge_command(View_info& views) {
	auto ship_ptr = read_ship();
	if (views.bridge_view.find(ship_ptr -> get_name()) != views.bridge_view.end())  
		throw Error("Bridge view is already open for that ship!");

	shared_ptr<View> new_view = make_shared<Bridge_view>(ship_ptr -> get_name());

	Model::get() -> attach(new_view);

	views.all_views.push_back(new_view);
	views.bridge_view[ship_ptr -> get_name()] = new_view;
}
void Controller::view_close_bridge_command(View_info& views) {
	string ship_name;
	cin >> ship_name;
	if (views.bridge_view.find(ship_name) == views.bridge_view.end())
		throw Error("Bridge view for that ship is not open!");

	Model::get() -> detach(views.bridge_view[ship_name]);
	
	views.all_views.remove(views.bridge_view[ship_name]);
	views.bridge_view.erase(ship_name);
}


/*************
 Model commands
 *************/
void Controller::model_status_command() {
	Model::get() -> describe();
}
void Controller::model_go_command() {
	Model::get() -> update();
}
void Controller::model_create_command() {
	string ship_name = read_name();
	if (Model::get() -> is_name_in_use(ship_name)) throw Error("Name is already in use!");
	string ship_type;
	cin >> ship_type;
	Point start_location = read_point();

	shared_ptr<Ship> new_ship = create_ship(ship_name, ship_type, start_location);
	Model::get() -> add_ship(new_ship);
}


/*************
 Ship commands
 *************/
void Controller::ship_course_command(shared_ptr<Ship> ship) {
	double heading = read_heading();
	double speed = read_speed();
	ship -> set_course_and_speed(heading, speed);
}
void Controller::ship_position_command(shared_ptr<Ship> ship) {
	Point position = read_point();
	double speed = read_speed();

	ship -> set_destination_position_and_speed(position, speed);
}

void Controller::ship_destination_command(shared_ptr<Ship> ship) {
	shared_ptr<Island> island_ptr = read_island();
	double speed = read_speed();

	ship -> set_destination_position_and_speed(island_ptr -> get_location(), speed);
}

void Controller::ship_load_at_command(shared_ptr<Ship> ship) {
	shared_ptr<Island> island_ptr = read_island();
	ship -> set_load_destination(island_ptr);
}

void Controller::ship_unload_at_command(shared_ptr<Ship> ship) {
	shared_ptr<Island> island_ptr = read_island();
	ship -> set_unload_destination(island_ptr);
}

void Controller::ship_dock_at_command(shared_ptr<Ship> ship) {
	shared_ptr<Island> island_ptr = read_island();
	ship -> dock(island_ptr);
}

void Controller::ship_attack_command(shared_ptr<Ship> ship) {
	shared_ptr<Ship> ship_ptr = read_ship();
	ship -> attack(ship_ptr);
}

void Controller::ship_refuel_command(shared_ptr<Ship> ship) {
	ship -> refuel();
}

void Controller::ship_stop_command(shared_ptr<Ship> ship) {
	ship -> stop();
}

void Controller::ship_stop_attack_command(shared_ptr<Ship> ship) {
	ship -> stop_attack();
}
