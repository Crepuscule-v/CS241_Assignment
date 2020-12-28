#ifndef __HEADERFILE_H__
#define __HEADERFILE_H__

#include "checker.h"
#include "sys/time.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <vector>
#include <queue>
#include <cstring>
#include <stdio.h>
#include <set>
#include <dirent.h>
#include <ctime>
#include <algorithm>
#include <random>
#include "stdlib.h"

template<class InType, class OutType>
OutType convert(InType val)
{
    std::stringstream ss;
    OutType ans;
    ss << val;
    ss >> ans;
    return ans;
}

struct cmp
{
    bool operator()(std::pair<int, double> A, std::pair<int, double> B) { return A.second > B.second; } 
};

struct cmp1
{
    bool operator() (const std::pair<int, double> &A, const std::pair<int, double> &B) { return A.second < B.second; }
};

struct cmp2
{
    bool operator() (const std::pair<std::pair<int, int>, double> &A, const std::pair<std::pair<int, int>, double> &B) { return A.second < B.second; }
};

struct cmp3
{
    bool operator() (const std::pair<int, int> &A, const std::pair<int, int> &B) { return A.second < B.second; }
};

bool is_num(char c){ return (c>='0' && c<='9') || c=='.';}

bool is_split(char c){ return !is_num(c); }

std::string GetStr(std::string str_, int &pos_)
{
    if (str_[pos_] == '<') pos_ ++;
    while (str_[pos_] == ' ' && pos_ < str_.size()) pos_ ++;
    int i = pos_;
    while (str_[pos_] != ' ' && pos_ < str_.size()) pos_ ++;
    std::string res = str_.substr(i, pos_ - i);
    return res;
}

#endif
