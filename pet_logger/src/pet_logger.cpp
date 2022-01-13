#include "pet_logger.h"

/******* PUBLICS *******/

petLogger::petLogger(const pet::params_container_t loggerParams, const std::string serviceName) : 
m_loggerParams(loggerParams), m_serviceName(serviceName) {};

petLogger::~petLogger() {

	if(m_out.is_open()) {

		FAST_LOG(CODE_POSITION() + "Логгер завершил работу");
		m_out.flush();	
		m_out.close();
	}
}

bool petLogger::init() {

	try {

		m_maxFileSize = std::stol(m_loggerParams.at(common_defs::logMaxSizeinMbParam))*common_defs::mbToBytes;

		if(!createLogDirIfNotExists()) {

			std::cerr << "Ошибка! Не удалось создать директорию для логов." << std::endl;
			return false;
		}
		
		if(!createNewLogfile()) {

			std::cerr << "Не удалось создать файл лога..." << std::endl;
			return false;
		}
		
		FAST_LOG(CODE_POSITION() + "Логгер успешно инициализирован");
		return true;
	}
	catch(...) {

		std::cerr << "Ошибка! Не удалось запустить логгер. Причина неизвестна" << std::endl;
		return false;
	}
}

void petLogger::FAST_LOG(const std::string& codeInfoAndMessage) {

	std::lock_guard<std::mutex> lg(m_mtx);

	if(m_out.is_open()) {

		m_out << getHumanReadableDate() << getThreadID() << codeInfoAndMessage << "\n";
		m_out.flush();

		if(getFilesize() >= m_maxFileSize) {

			if(m_out.is_open()) {

				m_out.flush();	
				m_out.close();
			}

			if(!createNewLogfile()) {

				std::cerr << "Размер файла лога превысил заданный, но новый создать не удалось..." << std::endl;
			}
		}
	}
	else {

		if(!createNewLogfile()) {

			std::cerr << "Файл был внезапно удален. Новый создать не удалось..." << std::endl;
		}
	}
}

/****** PRIVATES ******/

std::thread::id petLogger::getThreadID() const {

	return std::this_thread::get_id();
}

std::string petLogger::getHumanReadableDate() {

	if(gettimeofday(&m_tv, nullptr) == -1) {

		return std::string{"not defined"};
	}

	m_timeData = localtime(&m_tv.tv_sec);

	m_ss.str(std::string());

	m_ss << m_timeData->tm_mday << " " << m_monthNames[m_timeData->tm_mon] << " " 
	<< (1900 + m_timeData->tm_year) << " " << m_timeData->tm_hour << ":" << m_timeData->tm_min 
	<< ":" << m_timeData->tm_sec << ":" << m_tv.tv_usec << common_defs::logFieldsDelimeter;

	return m_ss.str();
}

time_t petLogger::getTimepointSinceEpoch() {

	return time(nullptr);
}

bool petLogger::createNewLogfile() {

	fs::path servicePath{m_serviceName};

	std::string logfileName{servicePath.filename().string() + "_" + std::to_string(getTimepointSinceEpoch()) + ".log"};

	std::string logfilePath = m_loggerParams.at(common_defs::logDirPathParam) + std::string("/") + logfileName;

	m_out.open(logfilePath, std::ofstream::out | std::ofstream::trunc);

	return m_out.is_open();
}

size_t petLogger::getFilesize() {

	return m_out.tellp();
}

bool petLogger::createLogDirIfNotExists() const {

	const std::string logDir{m_loggerParams.at(common_defs::logDirPathParam)};

	if(!fs::exists(logDir)) {

		auto prevUmask = umask(7);
		
		try {
			fs::create_directories(logDir);
			return true;
		}
		catch (std::experimental::filesystem::filesystem_error err) {

			umask(prevUmask);
			return false;
		}
	}
	return true;
}