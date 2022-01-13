#include <iostream>
#include <string>
#include <unistd.h>

#include "filewatch_daemon.h"
#include "common_defs.h"

int main (int argc, char* argv[]) { 

	if(argc < 2) {

		std::cerr << "Не хватает аргументов\nИспользование: " << argv[0] << " <путь к конфигу *.ini>" << std::endl;
		return common_defs::exit_code::WRONG_ARGS;
	}

	try {

		std::string iniFilePath{argv[1]};

		iniFileParser ifp{iniFilePath};

		auto content = ifp.getSectionContent(pet_params::FILE_OBSERVER);
		auto result = content.first;

		if(!result) {

			std::cerr << "Отсутствует файл конфига" << std::endl;
 			return common_defs::exit_code::NO_CONFIG;
		}

		auto sectionData = content.second;

		fileWatch fileWatcher(sectionData.at("dir"), iniFilePath, std::string(argv[0]));

		if(!fileWatcher.initFileWatch()) {

			std::cerr << "Сбой инициализации файлового обозревателя" << std::endl;
 			return common_defs::exit_code::CANT_INIT;
		}

		fileWatcher.runService();
	}
	catch (std::exception& e) {

		std::cout << " Обнаружена ошибка в работе демона-наблюдателя: [ " 
		<< e.what() <<" ]. Завершаем работу." << std::endl;
		return common_defs::exit_code::ERROR_AFTER_EXCEPTION;
	}
	return common_defs::exit_code::NORMAL_EXIT;
}