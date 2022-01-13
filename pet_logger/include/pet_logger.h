#ifndef PET_LOGGER_H
#define PET_LOGGER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <mutex>
#include <sstream>
#include <string>
#include <array>
#include <experimental/filesystem>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ini_file_parser.h"
#include "common_defs.h"

namespace fs = std::experimental::filesystem;

#define CODE_POSITION() std::string{std::string(common_defs::logFieldsDelimeter) + \
fs::path{std::string(__FILE__)}.filename().string() + ":" + std::string(__func__) + ":" + \
std::to_string(__LINE__) + std::string(common_defs::logFieldsDelimeter)}

class petLogger final {

private:

	const std::array<std::string, 12> m_monthNames {

		"янв",
		"фев",
		"мар",
		"апр",
		"май",
		"июн",
		"июл",
		"авг",
		"сен",
		"окт",
		"ноя",
		"дек",
	};

	std::mutex m_mtx;

	const pet::params_container_t m_loggerParams;
	const std::string m_serviceName;
	size_t m_maxFileSize;

	std::ofstream m_out;
	std::stringstream m_ss;

	struct timeval m_tv;
	struct tm* m_timeData{nullptr};

private:

	std::thread::id getThreadID() const;

	std::string getHumanReadableDate();

	time_t getTimepointSinceEpoch();

	bool createNewLogfile();

	size_t getFilesize();

	bool createLogDirIfNotExists() const;

public:
	petLogger() = delete;
	petLogger(const petLogger&) = delete;
	petLogger(const petLogger&&) = delete;

	petLogger(const pet::params_container_t loggerParams, const std::string serviceName);

	~petLogger();

	bool init();

	void FAST_LOG(const std::string& codeInfoAndMessage);
};

#endif /*PET_LOGGER_H*/
