#!/bin/bash

rm a.out out.txt
g++ -std=c++11 lc3101.cpp
./a.out testcase2.txt out.txt
