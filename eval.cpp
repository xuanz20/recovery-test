#define key_len 32
#define value_len 96
#define data_size 1000000
#include "db.h"
#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <map>
#include <set>
#include <iterator>

void run(
    double,
    std::string*,
    DB*,
    std::map<std::string, std::string>*,
    std::ofstream*,
    int
);

void crash(DB*, int);

int main(int argc, char *argv[]) {
    auto db = new DB(argv[1], argv[2]);
    double true_rate = atof(argv[3]);
    auto test_db = new std::map<std::string, std::string>;
    std::string line, name, isbn;
    // std::set<std::string> name_list;
    std::ifstream init("init.txt");
    std::ifstream read("name.txt");
    std::ofstream fout;
    fout.open(argv[4]);
    double time = 0;

    while(getline(init, line)) {
        name = line.substr(0, key_len);
        isbn = line.substr(key_len + 1, value_len);
        test_db->insert(make_pair(name, isbn));
    }

    // std::string name_list[data_size];
    auto name_list = new std::string [data_size];
    int i = 0;
    while(getline(read, line)) {
        name = line.substr(0, key_len);
        // name_list.insert(name);
        name_list[i] = name;
        i++;
    }

    std::cout << "Loading finishes!" << std::endl;

    run(true_rate, name_list, db, test_db, &fout, 30);
    crash(db, 30);
    run(true_rate, name_list, db, test_db, &fout, 30);
    crash(db, 60);
    run(true_rate, name_list, db, test_db, &fout, 60);
    crash(db, 120);
    run(true_rate, name_list, db, test_db, &fout, 120);
    crash(db, 240);
    run(true_rate, name_list, db, test_db, &fout, 240);

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

void eval(
    double true_rate,
    std::string *name_list,
    DB *db, 
    std::map<std::string, std::string> *test_db,
    std::ofstream *fout
) {
    int sec = 0;
    std::string name, isbn, line;
    double time = 0;
    int num = 0;
    while (time < 1) {
        num++;
        if (random_bool(true_rate)) { // read operation
            // std::cout << "get operation" << std::endl;
            name = get_random_name(name_list);
            time += get(db, test_db, name).count();
        } else { // update operation
            // std::cout << "set operation" << std::endl;
            name = get_random_name(name_list);
            isbn = generate_isbn();
            time += set(db, test_db, name, isbn).count();
        }
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

void crash(DB* db, int sec) {
    try {
        db->crash();
    } catch (...) {
        std::cout << "Crash at " << sec << "seconds!" << std::endl;
        db->recovery();
        std::cout << "Recovery finishes at " << sec << "seconds!" << std::endl;
    }
} 