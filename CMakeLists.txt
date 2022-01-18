cmake_minimum_required(VERSION 3.20)
project(Mywebserver)

set(CMAKE_CXX_STANDARD 14)

add_executable(Mywebserver main.cpp threadpool/threadpool.h lock/locker.h mysqlpool/sql_connection_pool.cpp mysqlpool/sql_connection_pool.h)
