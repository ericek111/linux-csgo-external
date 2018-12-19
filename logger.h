#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <linux/limits.h>
#include <unistd.h>
#include "colors.h"

#define LOG_TITLE BOLD << CYAN << "[" << WHITE << "LOG" << CYAN << "] " << RESET
#define WARNING_TITLE BOLD << CYAN << "[" << YELLOW << "WARNING" << CYAN << "] " << RESET
#define ERROR_TITLE BOLD << CYAN << "[" << RED << "ERROR" << CYAN << "] " << RESET
#define ADDRESS_TITLE BOLD << CYAN << "[" << MAGENTA << "ADDRESS" << CYAN << "] " << RESET
#define TOGGLE_TITLE BOLD << CYAN << "[" << BLUE << "TOGGLE" << CYAN << "] " << RESET

#define LOGGER_CLEAR_ON_START true

namespace Logger {
	extern void init ();
	extern void Log (std::string str);
	extern void normal (std::string str);
	extern void toggle (std::string feature, bool enabled);
	extern void address (std::string str, unsigned long address);
	extern void warning (std::string str);
	extern void error (std::string str);
	extern std::string Endi (bool endi);
};

#endif
