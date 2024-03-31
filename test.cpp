#include "Server.hpp"

int main() {
    time_t now = time(0); //  Get the current calendar time as a time_t object.
tm *local_time = localtime(&now); // Convert to a 'tm' structure representing local time.
std::cout << "Year: " << 1900 + local_time->tm_year << std::endl;
std::cout << "Month: " << 1 + local_time->tm_mon << std::endl;
std::cout << "Day: " << local_time->tm_mday << std::endl;
std::cout << "Hour: " << local_time->tm_hour << std::endl;
std::cout << "Minute: " << local_time->tm_min << std::endl;
std::cout << "Second: " << local_time->tm_sec << std::endl;

}