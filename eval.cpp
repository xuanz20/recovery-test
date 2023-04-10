#define key_len 64
#define value_len 192
#define data_size 1000000
#include "db.h"
#include "utils.h"
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <map>
#include <set>
#include <iterator>

std::string conf_path = "/etc/redis/6379.conf";
std::string log_path = "/var/log/redis_6379.log";
std::string dump_path = "/var/lib/redis/6379/dump.rdb";

void run(
    double,
    std::string*,
    DB*,
    std::map<std::string, std::string>*,
    std::ofstream*,
    int
);

void crash(
    double,
    std::string*,
    DB*, 
    std::map<std::string, std::string>*,
    int, 
    std::ofstream*,
    bool);

int main(int argc, char *argv[]) {
    clean_history(dump_path, log_path);
    start_server(conf_path);
    bool rdb = atoi(argv[3]);
    auto db = new DB(argv[1], argv[2], rdb);
    double true_rate = atof(argv[4]);
    auto test_db = new std::map<std::string, std::string>;
    std::string line, name, isbn;
    std::ifstream init("init.txt");
    std::ofstream fout;
    fout.open(argv[5]);
    double time = 0;
    auto name_list = new std::string [data_size];
    int i = 0;

    while(getline(init, line)) {
        name = line.substr(0, key_len);
        isbn = line.substr(key_len + 1, value_len);
        test_db->insert(make_pair(name, isbn));
        name_list[i] = name;
        i++;
    }

    std::cout << "Loading finishes!" << std::endl;

    run(true_rate, name_list, db, test_db, &fout, 60);
    crash(true_rate, name_list, db, test_db, 60, &fout, rdb);
    run(true_rate, name_list, db, test_db, &fout, 60);
    crash(true_rate, name_list, db, test_db, 120, &fout, rdb);
    run(true_rate, name_list, db, test_db, &fout, 120);
    crash(true_rate, name_list, db, test_db, 240, &fout, rdb);
    run(true_rate, name_list, db, test_db, &fout, 240);
    crash(true_rate, name_list, db, test_db, 480, &fout, rdb);
    run(true_rate, name_list, db, test_db, &fout, 480);

    shut_server();
    fout.close();
    return 0;
}

auto get(DB* db, std::map<std::string, std::string> *test_db, std::string name) {
    auto start = std::chrono::high_resolution_clock::now();
    auto res = db->get(name);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto test_res = test_db->find(name)->second;
    // assert(res == test_res);
    if (res != test_res) {
        std::cout << "name: " << name << std::endl;
        std::cout << res << " " << test_res << std::endl;
        assert(res == test_res);
    }
    return diff;
}

auto set(DB* db, std::map<std::string, std::string> *test_db, std::string name, std::string ISBN) {
    auto start = std::chrono::high_resolution_clock::now();
    db->set(name, ISBN);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto res = test_db->insert(make_pair(name, ISBN));
    if (!res.second) 
        res.first->second = ISBN;
    return diff;
}

std::string generate_isbn() {
    std::string isbn;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 9);
    for (int i = 0; i < value_len; i++) {
        isbn += std::to_string(dis(gen));
    }
    return isbn;
}

std::string get_random_name(std::string *name_list) {
    int index = std::rand() % data_size;
    // auto it = name_list->begin();
    // std::advance(it, std::rand() % name_list->size());
    // std::cout << *it << std::endl;
    return name_list[index];
}

bool random_bool(double true_rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(true_rate);
    return d(gen);
}

void operation(
    double true_rate,
    std::string *name_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    double *start
) {
    if (random_bool(true_rate)) { // read operation
        // std::cout << "get operation" << std::endl;
        std::string name = get_random_name(name_list);
        *start += get(db, test_db, name).count();
    } else { // update operation
        // std::cout << "set operation" << std::endl;
        std::string name = get_random_name(name_list);
        std::string isbn = generate_isbn();
        *start += set(db, test_db, name, isbn).count();
    }
}

void eval(
    double true_rate,
    std::string *name_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    std::ofstream *fout
) {
    int sec = 0;
    double time = 0;
    int num = 0;
    while (time < 1) {
        num++;
        operation(true_rate, name_list, db, test_db, &time);
    }
    *fout << num << std::endl;
}

void run(
    double true_rate,
    std::string *name_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    std::ofstream *fout,
    int sec
) {
    for (int i = 0; i < sec; ++i) {
        eval(true_rate, name_list, db, test_db, fout);
    }
}

void crash(
    double true_rate,
    std::string *name_list,
    DB* db,
    std::map<std::string, std::string> *test_db,
    int sec, 
    std::ofstream *fout,
    bool rdb
) {
    try {
        db->crash();
    } catch (...) {
        std::cout << "Crash at " << sec << " seconds!" << std::endl;
        db->recovery(rdb);
        std::cout << "Recovery finishes at " << sec << " seconds!" << std::endl;
        // double time = get_DB_load_time(log_path);
        // std::cout << "DB loaded from disk: " << time << " seconds" << std::endl;
        /* while (time >= 1) {
            *fout << 0 << std::endl;
            time -= 1;
        } */
        /* int num = 0;
        double time = 0;
        while (time < 1) {
            num++;
            operation(true_rate, name_list, db, test_db, &time);
        }
        *fout << num << std::endl;
        */
    }
} 
