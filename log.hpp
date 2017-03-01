#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <linux/limits.h>
#include <unistd.h>

namespace dlog
{
    extern void init(bool printpath);
    extern void put(std::string str);
};