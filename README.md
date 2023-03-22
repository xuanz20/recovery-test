# Recovery Test

## Dependencies:

To run the codes in this repo, you should have the following:

+ g++ support C++17 standard
+ [Redis](https://redis.io)
+ [Mysql](https://www.geeksforgeeks.org/how-to-install-mysql-on-linux/)
+ [redis-plus-plus client](https://linuxhint.com/connect-redis-with-cpp/)

+ [MySQL Connector/C++ (8.0)](https://dev.mysql.com/doc/connector-cpp/8.0/en/connector-cpp-installation-binary.html)

## Notices:

+ Since we need to test Redis recovery performance, we need to disable Redis persistence, which can done by setting `set ""` in `redis.conf`.
+ To use MySQL Connector/C++ x DevAPI, we need X Plugin enabled for MySQL, which is by default in MySQL 8.

## Usage:

+ `python3 gen.py`: generate initial file.

+ `make`: compile the source code into an executable file.

+ `./run.sh [user] [password]`: run the experiment. 

    `[user]` and `[password]` stand for the username and password to connect to MySQL server. Note that we need a user to have privileges for tables.

+ `python3 plot.py`: plot the data and save it.

## Result:

The current data size is about 128MB with 1M key-value pairs, each pair 128B.

### 90%read, 10% update:

![90%read_10%update](https://i.postimg.cc/cC552LZ2/90-read-10-update.png)

### 50%read, 50%update:

![50%read_50%update](https://i.postimg.cc/7ZzQt4Rt/50-read-50-update.png)

We crash the redis server at 30 secs, 1 min, 2 min and 4 min.

## Conclusions:

+ The recovery time is close to warm-up time, which is quite long(about 200 seconds).
+ The experiment result is more stable with 90% read, 10% update than 50% read, 50% update.

## Week Plan:

+ Try to crash at longer time-intervals so that the system can fully warm up.

+ Test it with Phoenix as frontend.
