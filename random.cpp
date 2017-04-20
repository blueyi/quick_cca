/*
 * random.cpp
 * Copyright (C) 2017 dutoeserver <dutoeserver@A0835-Server-2>
 *
 * Distributed under terms of the MIT license.
 */



#include <random>
#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
//该函数接受三个参数分别指定随机数范围和种子，返回double
double random_unint(unsigned int min, unsigned int max, unsigned int seed = 0)
{
    static std::default_random_engine e(seed);
    static std::uniform_int_distribution<int> u(min, max);
    return u(e);
}
int main(int argc, char*argv[])
{
    if (argc < 5) {
        std::cout << "接受4个参数，分别是列数、行数、随机数最小值、随机数最大值" << std::endl;
        return 1;
    }
    std::stringstream oss;
    for (int i = 1; i <= argc; ++i)
        oss << argv[i] << " ";
    std::istringstream iss(oss.str());
    int row, col, fir, sec;
    iss >> col >> row >> fir >> sec;
    std::cout << "col: " << col << " row: " << row << " range: " << fir << "-" << sec  << std::endl;
    for (int i = 0; i < row; ++i) {
        for (int i = 0; i < col; ++i) {
            std::cout << random_unint(fir, sec, time(NULL)) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    return 0;
}
