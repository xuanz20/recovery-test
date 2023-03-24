#ifndef UTILS_H
#define UTILS_H
#include <string>

double get_DB_load_time(std::string);
void start_server(std::string);
void clean_history(std::string, std::string);
void shut_server();

#endif