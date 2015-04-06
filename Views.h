#ifndef VIEWS_H
#define VIEWS_H


#include "View.h"
#include "Geometry.h"

class Map_view : public View {
public:
	Map_view();

	//override fat interface functions
	void draw() override;
	void set_size(int size_) override;
	void set_scale(double scale_) override;
	void set_origin(Point origin) override;
	void set_defaults() override;

private:
	Point origin;
	double scale;
	int size;
};


class Sailing_data_view : public View {
public:
	void draw() override;
};


class Bridge_view : public View {
public:
	Bridge_view(const std::string& ship_name_);

	//getter to know which ship this bridge is in charge of
	std::string get_ship_name() const {return ship_name;}
	
	void draw() override;

	//update variables when the ship has sunk/been removed
	void update_remove(const std::string &name) override;


private:
	std::string ship_name; //keep track of which ship this bridge is in charge of
	Point ship_loc; //keep track of location - incase ship sinks
	bool sunk = false; //cache if it sinks
};





#endif