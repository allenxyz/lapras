#ifndef UTILITIES_H
#define UTILITIES_H

#include <exception>
#include <string>
#include <memory>

struct Point;
class Island;
class Ship;

class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

extern const double round_off_error_c;

/* add any of your own declarations here */
int read_int();
double read_double();
double read_speed();
double read_heading();
Point read_point();
std::string read_name();
std::shared_ptr<Island> read_island();
std::shared_ptr<Ship> read_ship();

#endif
