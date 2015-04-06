#ifndef CONTROLLER_H
#define CONTROLLER_H
 /* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

#include <map>
#include <string>
#include <list>
#include <memory>

class View;
class Ship;

class Controller {
public:	
	Controller();

	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
	
private: 
	struct View_info {
		std::list<std::shared_ptr<View>> all_views;
		std::shared_ptr<View> map_view;
		std::shared_ptr<View> sailing_view;
		std::map<std::string, std::shared_ptr<View>> bridge_view;
	};

	using model_func_ptr = void(Controller::*)();
	using view_func_ptr = void(Controller::*)(View_info&);
	using ship_func_ptr = void(Controller::*)(std::shared_ptr<Ship>);

	//map containers holding function pointers to map from command input strings
	std::map<std::string, model_func_ptr> model_commands;
	std::map<std::string, view_func_ptr> view_commands;
	std::map<std::string, ship_func_ptr> ship_commands;

	//view functions
	void view_default_command(View_info &);
	void view_size_command(View_info &);
	void view_zoom_command(View_info &);
	void view_pan_command(View_info &);
	void view_show_command(View_info &);
	void view_open_map_command(View_info &);
	void view_close_map_command(View_info &);
	void view_open_sailing_command(View_info &);
	void view_close_sailing_command(View_info &);
	void view_open_bridge_command(View_info &);
	void view_close_bridge_command(View_info &);
	//model functions
	void model_status_command();
	void model_go_command();
	void model_create_command();
	//ship functions
	void ship_course_command(std::shared_ptr<Ship> ship);
	void ship_position_command(std::shared_ptr<Ship> ship);
	void ship_destination_command(std::shared_ptr<Ship> ship);
	void ship_load_at_command(std::shared_ptr<Ship> ship);
	void ship_unload_at_command(std::shared_ptr<Ship> ship);
	void ship_dock_at_command(std::shared_ptr<Ship> ship);
	void ship_attack_command(std::shared_ptr<Ship> ship);
	void ship_refuel_command(std::shared_ptr<Ship> ship);
	void ship_stop_command(std::shared_ptr<Ship> ship);
	void ship_stop_attack_command(std::shared_ptr<Ship> ship);



};

#endif