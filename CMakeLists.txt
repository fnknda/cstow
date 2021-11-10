cmake_minimum_required(VERSION 3.5)
project(cstow)

add_executable(cstow src/cstow.c src/options.c)
install(TARGETS cstow)
