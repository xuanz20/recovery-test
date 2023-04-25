CPPFLAGS = -I $(CPLUS_INCLUDE_PATH) -L $(LD_LIBRARY_PATH)
LDLIBS = -lmysqlcppconn8 -lhiredis
CXX = g++
CXXFLAGS = -std=c++17 -pthread -O2

eval : db.cpp eval.cpp utils.cpp

.PHONY : clean
clean :
	-rm eval *.sql *.txt *.png
