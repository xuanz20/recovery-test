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
![50%read_50%update_noRDB](https://github.com/xuanz20/recovery-test/blob/phx/result/50%25read_50%25update_noRDB.png?raw=true)

### 90%read, 10%update, RDB off:
![90%read_10%update_noRDB](https://github.com/xuanz20/recovery-test/blob/phx/result/90%25read_10%25update_noRDB.png?raw=true)

We crash the redis server at 1 min, 2 min, 4 min and 8 min.

## Conclusions:

+ The performance fluctuates a lot on the VM. We may take the key-value pair smaller.

## Week Plan:

+ Improve the existing experiment to get better result.

+ Read the Phoenix kernel and user api code.
