#define key_len 8
#define value_len 24
#define data_size 1000
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

void run (
    double,
    std::string*,
    DB*,
    std::map<std::string, std::string>*,
    std::ofstream*,
    int
);

void crash (
    double,
    std::string*,
    DB*, 
    std::map<std::string, std::string>*,
    int, 
    std::ofstream*
);

int main(int argc, char *argv[]) {
    clean_history(dump_path, log_path);
    start_server(conf_path);
    auto db = new DB(argv[1], argv[2]);
    double true_rate = atof(argv[3]);
    auto test_db = new std::map<std::string, std::string>;
    std::string line, key, value;
    std::ifstream init("init.txt");
    std::ofstream fout;
    fout.open(argv[4]);
    double time = 0;
    auto key_list = new std::string [data_size];
    int i = 0;

    while(getline(init, line)) {
        key = line.substr(0, key_len);
        value = line.substr(key_len + 1, value_len);
        test_db->insert(make_pair(key, value));
        key_list[i++] = key;
    }

    std::cout << "Loading finishes!" << std::endl;

    run(true_rate, key_list, db, test_db, &fout, 60);
    crash(true_rate, key_list, db, test_db, 60, &fout);
    run(true_rate, key_list, db, test_db, &fout, 60);
    crash(true_rate, key_list, db, test_db, 120, &fout);
    run(true_rate, key_list, db, test_db, &fout, 120);
    crash(true_rate, key_list, db, test_db, 240, &fout);
    run(true_rate, key_list, db, test_db, &fout, 240);
    crash(true_rate, key_list, db, test_db, 480, &fout);
    run(true_rate, key_list, db, test_db, &fout, 480);

    fout.close();
    return 0;
}

double get(DB* db, std::map<std::string, std::string> *test_db, std::string key) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string res;
    db->get(key, &res);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto test_res = test_db->find(key)->second;
    if (res != test_res) {
        std::cout << "key: " << key << std::endl;
        std::cout << res << " " << test_res << std::endl;
        assert(res == test_res);
    }
    return diff.count();
}

double set(DB* db, std::map<std::string, std::string> *test_db, std::string key, std::string value) {
    auto start = std::chrono::high_resolution_clock::now();
    db->set(key, value);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    auto res = test_db->insert(make_pair(key, value)); 
    if (!res.second) 
        res.first->second = value;
    return diff.count();
}

std::string generate_value() {
    std::string value;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 9);
    for (int i = 0; i < value_len; i++) {
        value += std::to_string(dis(gen));
    }
    return value;
}

std::string get_random_key(std::string *key_list) {
    int index = std::rand() % data_size;
    return key_list[index];
}

bool random_bool(double true_rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(true_rate);
    return d(gen);
}

void operation(
    double true_rate,
    std::string *key_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    double *start
) {
    if (random_bool(true_rate)) { // read operation
        // std::cout << "get operation" << std::endl;
        std::string key = get_random_key(key_list);
        *start += get(db, test_db, key);
    } else { // update operation
        // std::cout << "set operation" << std::endl;
        std::string key = get_random_key(key_list);
        std::string value = generate_value();
        *start += set(db, test_db, key, value);
    }
}

void eval(
    double true_rate,
    std::string *key_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    std::ofstream *fout
) {
    int sec = 0;
    double time = 0;
    int num = 0;
    while (time < 1) {
        num++;
        operation(true_rate, key_list, db, test_db, &time);
    }
    *fout << num << std::endl;
}

void run(
    double true_rate,
    std::string *key_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    std::ofstream *fout,
    int sec
) {
    for (int i = 0; i < sec; ++i) {
        eval(true_rate, key_list, db, test_db, fout);
    }
}

void crash(
    double true_rate,
    std::string *key_list,
    DB* db,
    std::map<std::string, std::string> *test_db,
    int sec, 
    std::ofstream *fout
) {
    try {
        db->crash();
    } catch (...) {
        std::cout << "Crash at " << sec << " seconds!" << std::endl;
        db->recovery();
        std::cout << "Recovery finishes at " << sec << " seconds!" << std::endl;
    }
} 
