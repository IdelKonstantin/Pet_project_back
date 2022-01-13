#include <iostream>
#include <string>
#include <unistd.h>

#include "http_server.h"
#include "common_defs.h"

int main (int argc, char* argv[]) { 

	if(argc < 2) {

		std::cerr << "Не хватает аргументов\nИспользование: " << argv[0] << " <путь к конфигу *.ini>" << std::endl;
		return common_defs::exit_code::WRONG_ARGS;
	}

	try {

		std::string iniFilePath{argv[1]};

		iniFileParser ifp{iniFilePath};

		auto content = ifp.getSectionContent(pet_params::HTTP_DATA);
		auto result = content.first;

		if(!result) {

			std::cerr << "Отсутствует файл конфига" << std::endl;
 			return common_defs::exit_code::NO_CONFIG;
		}

		auto sectionData = content.second;

		std::string host, port;
			
		host = sectionData.at("host");
		port = sectionData.at("port");

		httpServer httpDaemon(host, static_cast<unsigned short>(std::strtoul(port.c_str(), nullptr, 0)), iniFilePath, std::string(argv[0]));

		if(!httpDaemon.initServer()) {

			std::cerr << "Сбой инициализации http-сервера" << std::endl;
 			return common_defs::exit_code::CANT_INIT;
		}

		httpDaemon.mainServerLoop();
	}
	catch (std::exception& e) {

		std::cout << " Обнаружена ошибка в работе http-демона: [ " 
		<< e.what() <<" ]. Завершаем работу." << std::endl;
		return common_defs::exit_code::ERROR_AFTER_EXCEPTION;
	}
	return common_defs::exit_code::NORMAL_EXIT;
}
