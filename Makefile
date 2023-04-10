MYSQL_CONCPP_DIR = /usr/local/mysql/connector-c++-8.0.32
CPPFLAGS = -I $(MYSQL_CONCPP_DIR)/include -L $(MYSQL_CONCPP_DIR)/lib64/debug
LDLIBS = -lmysqlcppconn8 /usr/local/lib/libredis++.a /usr/local/lib/libhiredis.a
CXX = g++
CXXFLAGS = -std=c++17 -pthread

eval : db.cpp eval.cpp utils.cpp

.PHONY : clean
clean :
	-rm eval *.sql *.txt *.png
