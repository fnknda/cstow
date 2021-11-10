cmake_minimum_required(VERSION 3.15..3.21)
project(cstow LANGUAGES C)

add_executable(cstow src/cstow.c src/options.c)
install(TARGETS cstow)
