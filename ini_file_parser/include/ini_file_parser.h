#ifndef _INI_FILE_PARSER_H_
#define _INI_FILE_PARSER_H_

#include <map>
#include <vector>
#include <iostream>
#include <boost/property_tree/ini_parser.hpp>

/********************************************

	Парсер файлов *.ini вида:

	[HTTP_DATA]				<-- параметры для соединения с http-сервером
	host=127.0.0.1
	port=6667
	db_type=SQLITE

	[FILE_OBSERVER]			<-- путь к хранилищу песен
	dir=./
	db_type=POSTGRESQL

	[LOGS_TDB]				<-- параметры для соединения с ТБД логов
	host=127.0.0.1
	port=5432
	user=postgres
	dbname=pet_project
	passwd=postgres

	[CONTENT_TDB]			<-- параметры для соединения с ТБД контента
	host=127.0.0.1
	port=5432
	user=postgres
	dbname=pet_project
	passwd=postgres

	[PET_LOGGER]			<-- параметры для логгера
	dir=../LOGS
	max_size_mb=1

********************************************/

namespace pet_params {

	const uint8_t HTTP_DATA = 0;
	const uint8_t FILE_OBSERVER = 1;
	const uint8_t LOGS_TDB = 2;
	const uint8_t CONTENT_TDB = 3; 
	const uint8_t PET_LOGGER = 4;
};

namespace pet {

	using params_pair_t = std::pair<std::string, std::vector<std::string>>;
	using params_container_t = std::map<std::string, std::string>;
	using params_t = std::pair<bool, pet::params_container_t>;

	const params_pair_t HTTP_DATA_PARAMS {

		"HTTP_DATA", {"host", "port", "db_type"}
	};

	const params_pair_t FILE_OBSERVER_PARAMS {

		"FILE_OBSERVER", {"dir", "db_type"} //db_type = POSTGRESQL or SQLITE
	};

	const params_pair_t LOGS_TDB_PARAMS {

		"LOGS_TDB",
		{
			"host",
			"port",
			"user",
			"dbname",
			"passwd"
		}
	};

	const params_pair_t CONTENT_TDB_PARAMS {

		"CONTENT_TDB",
		{
			"host",
			"port",
			"user",
			"dbname",
			"passwd"
		}
	};

	const params_pair_t PET_LOGGER_PARAMS {

		"PET_LOGGER",
		{
			"dir",
			"max_size_mb"
		}
	};
}

class iniFileParser {

private:
	const std::string m_iniFilePath{};
	pet::params_container_t m_paramsKeeper{};

public:
	iniFileParser() = delete;
	iniFileParser(const iniFileParser&) = delete;
	iniFileParser(const iniFileParser&&) = delete;     
	
	iniFileParser(const std::string iniFilePath);

	pet::params_t getSectionContent(const uint8_t paramsType);
};

#endif /* _INI_FILE_PARSER_H_ */
