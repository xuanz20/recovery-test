# Recovery Test

## Dependencies:

To run the codes in this repo, you should have the following:

+ g++ support C++17 standard
+ [Redis](https://redis.io)
+ [Mysql](https://www.geeksforgeeks.org/how-to-install-mysql-on-linux/)
+ [redis-plus-plus client](https://linuxhint.com/connect-redis-with-cpp/)

+ [MySQL Connector/C++ (8.0)](https://dev.mysql.com/doc/connector-cpp/8.0/en/connector-cpp-installation-binary.html)

## Notices:

+ Since we need to test Redis recovery performance, we need to disable Redis persistence, which can be done by setting `set ""` in `redis.conf`.
+ To use MySQL Connector/C++ x DevAPI, we need X Plugin enabled for MySQL, which is by default in MySQL 8.

## Usage:

+ `python3 gen.py`: generate initial file.

+ `make`: compile the source code into an executable file.

+ `./run.sh [user] [password]`: run the experiment. 

    `[user]` and `[password]` stand for the username and password to connect to MySQL server. Note that we need a user to have privileges for tables.

+ `python3 plot.py`: plot the data and save it.

## Result:

The current data size is about 512MB with 2M key-value pairs, each pair 256B.

### 50%read, 50%update, RDB off:
![50%read_50%update_noRDB](https://github.com/xuanz20/recovery-test/blob/main/result/50%25read_50%25update_noRDB.png?raw=true)

### 90%read, 10%update, RDB off:
![90%read_10%update_noRDB](https://github.com/xuanz20/recovery-test/blob/main/result/90%25read_10%25update_noRDB.png?raw=true)

### 50%read, 50%update, RDB on:
![50%read_50%update_RDB](https://github.com/xuanz20/recovery-test/blob/main/result/50%25read_50%25update_RDB.png?raw=true)

### 90%read, 10%update, RDB on:
![90%read_10%update_RDB](https://github.com/xuanz20/recovery-test/blob/main/result/90%25read_10%25update_RDB.png?raw=true)

We crash the redis server at 1 min, 2 min and 4 min.

## Conclusions:

+ The recovery time is close to warm-up time, which is quite long(about 200 seconds).

+ The experiment result is more stable with 90% read, 10% update than 50% read, 50% update.

+ With RDB on, the performance drop comes mainly from the black out time, and the recovery performance is high.

## Week Plan:

+ Try to crash at longer time-intervals so that the system can fully warm up.

+ Test it with Phoenix as frontend.
