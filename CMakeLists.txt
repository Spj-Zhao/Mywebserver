cmake_minimum_required(VERSION 3.10)
project(Mywebserver)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/lib/x86_64-linux-gnu)

add_executable(Mywebserver main.cpp threadpool/threadpool.h lock/locker.h mysqlpool/sql_connection_pool.cpp mysqlpool/sql_connection_pool.h log/log.cpp log/log.h log/block_queue.h)

#需要导入pthread库文件:3种方法
#1.set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")与下面的语句是一样的意思
#find_package(Threads)与target_link_libraries(${PROJECT_NAME} Threads::Threads)
set(CMAKE_CXX_FLAGS  -pthread)
#链接libmysqlclient.so   (libmysqlclient.so位于/usr/lib/x86_64-linux-gnu)
target_link_libraries(${PROJECT_NAME}  libmysqlclient.so)

