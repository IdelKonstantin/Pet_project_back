#include "filewatch_daemon.h"

/****** PRIVATES ******/

void fileWatch::loopInDelete() {
	
	while(!canExit()) {

		m_inotyInDelete.readInotyEvents();
		sleep(0);
	}
}

void fileWatch::loopInAdd() {
	
	while(!canExit()) {

		m_inotyInAdd.readInotyEvents();
		sleep(0);
	}
}

bool fileWatch::deleteSongDataFromTDB(std::string filename) const {

	if(filename.find(".sqlite-journal") != std::string::npos) {

		return true;
	}

	m_pl->FAST_LOG(CODE_POSITION() + "Удаляется файл: " + filename);

	std::stringstream ss;
	ss << "DELETE FROM song_list WHERE song_name ='" << filename << "';";
	return m_tdbworker_content->execQuery(ss.str()); 
}

bool fileWatch::insertSongDataIntoTDB(std::string filename) const {
	
	if(filename.find(".sqlite-journal") != std::string::npos) {

		return true;
	}

	m_pl->FAST_LOG(CODE_POSITION() + "Добавлен файл: " + filename);

	std::stringstream ss;
	ss << "INSERT INTO song_list(song_name, song_uid) VALUES ('" 
	<< filename << "', '" << uuid::CUUIDGenerator::getNewUUID() << "');";
	
	return m_tdbworker_content->execQuery(ss.str());
}	

/****** PUBLICS ******/

fileWatch::fileWatch(std::string watchDirPath, const std::string inifilePath, const std::string serviceName) : 
	baseDaemon(inifilePath, serviceName), 
	m_inotyInDelete(watchDirPath, IN_DELETE | IN_MOVED_FROM), 
	m_inotyInAdd(watchDirPath, IN_CREATE | IN_MOVED_TO) {

	m_th_pool.resize(2);
};

bool fileWatch::initFileWatch() {

	if(!initBaseDaemon()) {

		std::cerr << "Ошибка в инициализации базового демона!" << std::endl;
		return false;
	}

	if(m_pl == nullptr || m_tdbworker_content == nullptr) {

		std::cerr << "Ошибка! Логгер или обработчик ТБД не валидны!" << std::endl;
		std::cerr << "m_pl == nullptr -> " << std::boolalpha << (m_pl == nullptr) 
		<< ", m_tdbworker_content == nullptr -> " << std::boolalpha 
		<< (m_tdbworker_content == nullptr) << std::endl;
		return false;
	} 

	if(!m_inotyInDelete.isInitDone() || (!m_inotyInAdd.isInitDone())) {
		
		m_pl->FAST_LOG(CODE_POSITION() + "Не удалось инициализировать файловый наблюдатель");
		return false;
	}

	m_inotyInDelete.setWatchFunction([this](std::string filename) {
	
		return deleteSongDataFromTDB(filename);
	});
	
	m_inotyInAdd.setWatchFunction([this](std::string filename) {
	
		return insertSongDataIntoTDB(filename);
	});

	m_pl->FAST_LOG(CODE_POSITION() + "Файловый наблюдатель инициализирован");
	return true;	
}

void fileWatch::runService() {

	m_th_pool.push([this](int){loopInDelete();});
	m_th_pool.push([this](int){loopInAdd();});

	m_pl->FAST_LOG(CODE_POSITION() + "Файловый наблюдатель стартовал");

	while(!canExit());
}