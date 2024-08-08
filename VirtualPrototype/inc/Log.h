/*!
 \file Log.h
 \brief Class to manage Log
 \author Màrius Montón
 \date Aug 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LOG_H
#define LOG_H

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <string>
#include <fstream>
#include <sstream>
#include "systemc"
#include "tlm.h"

/**
 * @brief Log management class
 *
 * Singleton class to be shared among all other classes
 */
class Log {
public:

	enum LogLevel {
		ERROR = 0, DEBUG, WARNING, INFO
	} currentLogLevel;

	/**
	 * @brief Constructor
	 * @return pointer to Log class
	 */
	static Log* getInstance();

	/**
	 * @brief method to log some string
	 * @param msg   mesasge string
	 * @param level level of the log (LogLevel)
	 */
	void SC_log(const std::string& msg, enum LogLevel level);

	/**
	 * @brief method to log some string
	 * @param  level level of the log (LogLevel)
	 * @return       streaming
	 *
	 * This function can be used in the following way:
	 * \code
	 * my_log->SC_log(Log::WARNING) << "some warning text"
	 * \endcode
	 */
	std::ofstream& SC_log(enum LogLevel level);

	/**
	 * @brief Sets log level
	 * @param newLevel Level of the log
	 */
	void setLogLevel(enum LogLevel newLevel);

	/**
	 * @brief Returns log level
	 * @return Current log level
	 */
	enum LogLevel getLogLevel() const;

private:
	static Log *instance;
	Log(const char *filename);
	std::ofstream m_stream;
	std::ofstream m_sink;
};

#endif
