#include "base_daemon.h"

/****** PUBLICS ******/

bool baseDaemon::m_canExit = true;

baseDaemon::baseDaemon(const std::string inifilePath, const std::string serviceName) : 
m_ifp{new iniFileParser(inifilePath)}, m_serviceName(serviceName) {

	signal(SIGINT, baseDaemon::signalHandler);
	signal(SIGTERM, baseDaemon::signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

baseDaemon::~baseDaemon() {

	m_pl->FAST_LOG(CODE_POSITION() + "Демон штатно завершен");
}

bool baseDaemon::initBaseDaemon() {

	if(m_ifp == nullptr) {

		std::cerr << "Демон [" << m_serviceName << "] не стартовал (m_ifp == nullptr)" << std::endl;
		return false;
	}

	auto petloggerParamsContent = m_ifp->getSectionContent(pet_params::PET_LOGGER);
	
	if(!petloggerParamsContent.first) {

		std::cerr << "Демон [" << m_serviceName << "] не стартовал (неверные параметры логгера в конфиге)" << std::endl;
		return false;
	}
	auto petloggerParams = petloggerParamsContent.second;

	m_pl = std::make_unique<petLogger>(petloggerParams, m_serviceName);

	if(!m_pl->init()) {
		
		std::cerr << "Демон [" << m_serviceName << "] не стартовал (не удалось инициализировать логгер)" << std::endl;
		return false;
	}

	/* База данных для хранения записей о песнях всегда будет соответствовать типу, 
	указанному в секции [FILE_OBSERVER] конфига *.ini, а настройки для подключения 
	к этой базе - в секции [CONTENT_TDB] для обоих демонов (для HTTP и FILE_OBSERVER) */

	TDB_fabrique tdbf;
	auto TDBtype = m_ifp->getSectionContent(pet_params::FILE_OBSERVER).second.at("db_type");

	if(TDBtype == common_defs::postgreTDB) {
		
		m_tdbworker_content = tdbf.produceTDB(TDB_type::POSTGRESQL, 
		m_ifp->getSectionContent(pet_params::CONTENT_TDB).second);
	}
	else if(TDBtype == common_defs::sqliteTDB) {

		m_tdbworker_content = tdbf.produceTDB(TDB_type::SQLITE, 
		m_ifp->getSectionContent(pet_params::CONTENT_TDB).second);
	}
	else {
	
		m_pl->FAST_LOG(CODE_POSITION() + "Демон не стартовал (указан неверный тип ТБД в конфиге)");
		std::cerr << "Демон [" << m_serviceName << "] не стартовал (указан неверный тип ТБД в конфиге)" << std::endl;
		return false;
	}

	if(!m_tdbworker_content->initWorker()) {

		m_pl->FAST_LOG(CODE_POSITION() + "Демон не стартовал (не удалось инициализировать службу ТБД)");
		std::cerr << "Демон [" << m_serviceName << "] не стартовал (не удалось инициализировать службу ТБД)" << std::endl;
		return false;
	}

	m_pl->FAST_LOG(CODE_POSITION() + "Демон успешно стартовал");
	std::cerr << "Демон [" << m_serviceName << "] успешно стартовал. Демон сохраняет контент в ТБД [" 
	<< m_tdbworker_content->whoAmI() << "]" << std::endl;

	m_canExit = false;
	return true;
}

void baseDaemon::signalHandler(int signal) {

	std::cerr << "Сработал обработчик сигналов." << std::endl;

	switch(signal) {
	
		case SIGINT:
		case SIGTERM:
		{
			std::cerr << "Завершаем работу по сигналу." << std::endl;
			forceExit();
			break;
		}
	};
}

bool baseDaemon::canExit() {

	return m_canExit;
}

/****** PRIVATES ******/

void baseDaemon::forceExit() {

	m_canExit = true;
}