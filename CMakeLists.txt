cmake_minimum_required(VERSION 3.10)
project(SlownikTabliceMieszajace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Flaga -O3 zapewnia optymalizacje szybkosci, istotna podczas badan czasowych.
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(projekt
    main.cpp
    HashTableList.cpp HashTableList.hpp
    HashTableAVL.cpp HashTableAVL.hpp
    HashTableOpen.cpp HashTableOpen.hpp
    Benchmark.cpp Benchmark.hpp
    IDictionary.hpp
    utility.hpp
)
