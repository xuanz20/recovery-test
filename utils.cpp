#include "utils.h"
#include <fstream>
#include <string>
#include <stdlib.h>

double get_DB_load_time(std::string log_path) {
    std::string line, last_matching_line;
    std::ifstream file(log_path);

    while (getline(file, line)) {
        size_t pos = line.find("DB loaded from disk");

        if (pos != std::string::npos) {
            last_matching_line = line;
        }
    }

    file.close();

    if (!last_matching_line.empty()) {
        size_t start_pos  = last_matching_line.find(": ");
        size_t end_pos = last_matching_line.find(" seconds");

        if (start_pos != std::string::npos && end_pos != std::string::npos) {
            std::string load_time_str = last_matching_line.substr(start_pos + 2, end_pos - start_pos - 2);
            double load_time = stod(load_time_str);
            return load_time;
        }
    }

    return 0;
}

void start_server(std::string conf_path = "") {
    std::string cmd = "redis-server " + conf_path + " &";
    system(cmd.c_str());
    system("sleep 5");
    return;
}

void clean_history(std::string dump_path, std::string log_path) {
    std::string cmd = "rm -f " + dump_path;
    system(cmd.c_str());
    cmd = "rm -f " + log_path;
    system(cmd.c_str());
    return;
}

void shut_server() {
    system("redis-cli shutdown");
    return;
}