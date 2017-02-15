rm -rf server
g++ -g -std=c++11 -pthread  -o server server.cpp Session.cpp
#valgrind --tool=memcheck --leak-check=full ./test 
./server
