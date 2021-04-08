cmake_minimum_required(VERSION 3.17)
project(busybeaver)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(busybeaver main.cpp TuringMachine/TuringMachine.cpp TuringMachine/TuringMachine.h Training/Bootcamp.cpp Training/Bootcamp.h Training/BlockingQueue.h TuringMachine/BaseTuringMachine.cpp TuringMachine/BaseTuringMachine.h TuringMachine/BusyBeaver.cpp TuringMachine/BusyBeaver.h)
target_link_libraries (busybeaver ${CMAKE_THREAD_LIBS_INIT})
