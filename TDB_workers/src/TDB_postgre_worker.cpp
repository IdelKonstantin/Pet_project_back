#include "TDB_postgre_worker.h"

/****** PRIVATES ******/

void postgreWorker::exitNicely() {

	if(m_conn) {

		PQfinish(m_conn);
	}	
}

/****** PUBLICS ******/

postgreWorker::postgreWorker(const pet::params_container_t connParams) : m_connParams(connParams) {}

postgreWorker::~postgreWorker() {

	exitNicely();
}

iworker::json_t postgreWorker::getSongsListAsJson() {

	std::lock_guard<std::mutex> lg(m_mtx);

	const char* getSongsRequest = "SELECT to_json(r) AS RESULT FROM (SELECT json_agg(row_to_json(t)) "
	"AS songs FROM (SELECT * FROM song_list)t)r;";

	m_res = PQexec(m_conn, getSongsRequest);

	if (PQresultStatus(m_res) != PGRES_TUPLES_OK) {

		std::cerr << "Ошибка в выполнении запроса: [" << getSongsRequest << "] -> " << PQerrorMessage(m_conn) << std::endl;
		PQclear(m_res);
		return pet::errorTDBResponce;
	}

	auto json = iworker::json_t{PQgetvalue(m_res, 0, 0)};

	return (json == "{\"songs\":null}") ? "[]" : json;
}

bool postgreWorker::execQuery(const std::string& TDBquery) {

	std::lock_guard<std::mutex> lg(m_mtx);

	m_res = PQexec(m_conn, TDBquery.c_str());
	return PQresultStatus(m_res) == PGRES_COMMAND_OK;
}

std::string postgreWorker::getFilenameByUID(const std::string& songUID) {

	std::lock_guard<std::mutex> lg(m_mtx);

	std::string getFilenameByUID{"SELECT song_name FROM song_list WHERE song_uid = '" + songUID + "';"};

	m_res = PQexec(m_conn, getFilenameByUID.c_str());

	if (PQresultStatus(m_res) != PGRES_TUPLES_OK) {

		std::cerr << "Ошибка в выполнении запроса: [" << getFilenameByUID << "] -> " << PQerrorMessage(m_conn) << std::endl;
		PQclear(m_res);
		return pet::errorTDBResponce;
	}

	std::string fileName{};

	try {

		fileName = std::string{PQgetvalue(m_res, 0, 0)};
		return fileName;
	}
	catch(...) {

		return std::string{};
	}
}

bool postgreWorker::initWorker() {

	try {

		m_conn = PQsetdbLogin(
			m_connParams.at("host").c_str(),
			m_connParams.at("port").c_str(),
			NO_PGOPTION, 
			NO_PGTTY,
			m_connParams.at("dbname").c_str(),
			m_connParams.at("user").c_str(),
			m_connParams.at("passwd").c_str()
		);
	}
	catch(const std::out_of_range& ex) {

		std::cerr << "Ошибка в параметрах подключения к ТБД: " << ex.what() << std::endl;
		return false;
	}

	if (PQstatus(m_conn) != CONNECTION_OK) {

		std::cerr << "Не удалось создать соединение с ТБД" << std::endl;
		exitNicely();
		return false;
	}

	return true;
}

iworker::timestamp_t postgreWorker::getTimestamp() {
	
	return "now()";
}

const std::string postgreWorker::whoAmI() {

	return common_defs::postgreTDB;
}