#include "logger.h"

std::string g_cwd;

void Logger::init() {
	char cwd[PATH_MAX];

	if (getcwd(cwd, PATH_MAX) == NULL) {
		g_cwd = "~";
	}

	g_cwd = cwd;
	g_cwd.append("/");

	#if LOGGER_CLEAR_ON_START
	std::ofstream file(g_cwd + "hack.log", std::ios::out | std::ios::app);
	// Clear the log file every time we start the hack
	file.clear ();
	#endif

	normal ("Log path -> " + g_cwd);
}

void Logger::Log (std::string str) {
	std::ofstream file(g_cwd + "hack.log", std::ios::out | std::ios::app);

	str.append("\r\n");

	if (file.good()) {
		file << str;
	}

	file.close();
}

void Logger::normal(std::string str) {
	Log (std::string("[LOG] ") + str);
	std::cout << LOG_TITLE << RESET << str << RESET << std::endl;
}

void Logger::address (std::string str, unsigned long address) {
	std::stringstream ss;
	ss << std::hex << address;

	Log (std::string("[ADDRESS] ") + str + ss.str ());
	std::cout << ADDRESS_TITLE << RESET << str << BOLD CYAN " [" MAGENTA "0x" << std::hex << address << CYAN  "]" << RESET << std::endl;
}

void Logger::warning (std::string str) {
	Log (std::string("[WARNING] ") + str);
	std::cout << ERROR_TITLE << YELLOW << str << RESET << std::endl;
}

void Logger::toggle (std::string feature, bool enabled) {
	std::string endi = Endi (enabled);
	Log (std::string("[TOGGLE] ") + feature + " " + endi);
	std::cout << TOGGLE_TITLE << BOLD WHITE
				  << feature << RESET BOLD CYAN "[ " << (enabled ? GREEN : RED)
				  << endi << RESET BOLD << (enabled ? "  " : " ") << CYAN << "]"
				  << RESET << std::endl;
}

void Logger::error (std::string str) {
	Log (std::string("[ERROR] ") + str);

	std::cout << ERROR_TITLE << RED << str << RESET << std::endl;
}

std::string Logger::Endi (bool endi) {
	return endi ? "Enabled" : "Disabled";
}
