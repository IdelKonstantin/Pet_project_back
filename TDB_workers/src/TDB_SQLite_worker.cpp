#include "TDB_SQLite_worker.h"

/****** PRIVATES ******/

int SQLiteWorker::callback(void *NotUsed, int argc, char **argv, char **azColName) {

	for(int i = 0; i < argc; i++){

		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");
	return 0;
}

/****** PUBLICS ******/

SQLiteWorker::SQLiteWorker(const pet::params_container_t connParams) : m_connParams(connParams) {}

SQLiteWorker::~SQLiteWorker() {

	sqlite3_free(m_zErrMsg);
}

void SQLiteWorker::openConnection() {

	if(sqlite3_open(m_connParams.at("dbname").c_str(), &m_db) != SQLITE_OK) {

		std::stringstream errMsg;
		errMsg << "Не удалось создать соединение с [" << m_connParams.at("dbname") << "]. Ошибка: " << sqlite3_errmsg(m_db);
		throw std::logic_error(errMsg.str());
	}
}

void SQLiteWorker::closeConnection() {

	if(m_reqResult) {

		sqlite3_finalize(m_reqResult);
	}

	if(m_db) { /* ВНИМАНИЕ: Потенциальная причина утечки памяти */

		sqlite3_close(m_db);
		//std::cout << "Соединение с SQLite разорвано [" << sqlite3_errmsg(m_db) << "]" << std::endl;
	}

	m_reqResult = nullptr;
	m_db = nullptr;
}

iworker::json_t SQLiteWorker::getSongsListAsJson() {

	//Внимание - этот метод будет работать только, если есть скомпилированное расширение libjson1.so по пути jsonSOpath 
	//(необходимо сперва собрать его, затем установить в системную папку (/usr/lib), а только потом собирать этот класс)

	std::lock_guard<std::mutex> lg(m_mtx);

	try {
		SQLiteConnectionWrapper scw(this);

		const char* jsonQuery = "SELECT json_group_array(json_object('id', id, 'song name', song_name, 'song uid', song_uid)) "
		"AS result FROM (SELECT * FROM song_list ORDER BY id);";
		
		sqlite3_enable_load_extension(m_db, 1);
		sqlite3_load_extension(m_db, JSON_EXTENSION_SO_PATH, nullptr, nullptr);

		if(sqlite3_prepare_v2(m_db, jsonQuery, std::strlen(jsonQuery), &m_reqResult, 0) == SQLITE_OK) {
			sqlite3_step(m_reqResult);
			std::string jsonFromDB{reinterpret_cast<const char*>(sqlite3_column_text(m_reqResult, 0))};
			return (jsonFromDB == "[]") ? jsonFromDB : iworker::json_t{"{\"songs\":" + jsonFromDB +"}"};
		}
	}
	catch(const std::logic_error& ex) {

		std::cerr << ex.what() << std::endl;
		return pet::errorTDBResponce;
	}
}

bool SQLiteWorker::execQuery(const std::string& TDBquery) {

	std::lock_guard<std::mutex> lg(m_mtx);

	try {
		SQLiteConnectionWrapper scw(this);

		return sqlite3_exec(m_db, TDBquery.c_str(), callback, 0, &m_zErrMsg) == SQLITE_OK;
	}
	catch(const std::logic_error& ex) {

		return false;
	}	
}

std::string SQLiteWorker::getFilenameByUID(const std::string& songUID) {

	std::lock_guard<std::mutex> lg(m_mtx);

	try {
		SQLiteConnectionWrapper scw(this);

		std::string getFilenameByUID{"SELECT song_name FROM song_list WHERE song_uid = '" + songUID + "';"};

		if(sqlite3_prepare_v2(m_db, getFilenameByUID.c_str(), getFilenameByUID.size(), &m_reqResult, 0) == SQLITE_OK) {

			sqlite3_step(m_reqResult);
		}

		try {
			auto filename = std::string{reinterpret_cast<const char*>(sqlite3_column_text(m_reqResult, 0))};
			return filename;
		}
		catch(...) {
			return std::string();
		}
	}
	catch(const std::logic_error& ex) {

		std::cerr << ex.what() << std::endl;
		return std::string();
	}
}

bool SQLiteWorker::initWorker() {

	return true; // Заглушка ;)
}

iworker::timestamp_t SQLiteWorker::getTimestamp() { 

	struct timeval tv;

	if(gettimeofday(&tv, nullptr) == -1) {

		return iworker::timestamp_t{"N/A"};
	}

	auto timeData = localtime(&tv.tv_sec);

	std::stringstream ss;

	ss << "'" << (1900 + timeData->tm_year) << "-" << std::setw(2) << std::setfill('0') << (timeData->tm_mon + 1) 
	<< "-" << std::setw(2) << std::setfill('0') << timeData->tm_mday << " " << timeData->tm_hour 
	<< ":" << timeData->tm_min << ":" << timeData->tm_sec << "." << tv.tv_usec << "'";

	return ss.str();
}

const std::string SQLiteWorker::whoAmI() {

	return common_defs::sqliteTDB;
}