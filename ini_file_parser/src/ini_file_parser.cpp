#include "ini_file_parser.h"

iniFileParser::iniFileParser(const std::string iniFilePath) : m_iniFilePath(iniFilePath) {}

pet::params_t iniFileParser::getSectionContent(const uint8_t paramsType) {

	const pet::params_pair_t* selectedParamsPair{nullptr};

	switch(paramsType) {

		case pet_params::HTTP_DATA:
		selectedParamsPair = &pet::HTTP_DATA_PARAMS;
		break;

		case pet_params::FILE_OBSERVER:
		selectedParamsPair = &pet::FILE_OBSERVER_PARAMS;
		break;

		case pet_params::LOGS_TDB:
		selectedParamsPair = &pet::LOGS_TDB_PARAMS;
		break;

		case pet_params::CONTENT_TDB:
		selectedParamsPair = &pet::CONTENT_TDB_PARAMS;
		break;

		case pet_params::PET_LOGGER:
		selectedParamsPair = &pet::PET_LOGGER_PARAMS;
		break;

		default:
		std::cerr << "\n Отсутствует запрошенная секция" << std::endl;
		return std::make_pair(false, pet::params_container_t{});
	}

	try {
		m_paramsKeeper.clear();

		auto sectionName = selectedParamsPair->first;
		auto sectionValueNames = selectedParamsPair->second;

		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini(m_iniFilePath, pt);

		for(const auto& valueName : sectionValueNames) {

			auto paramValue = pt.get<std::string>(std::string(sectionName + "." + valueName));
			m_paramsKeeper.insert(std::make_pair(valueName, paramValue));
		}

		return std::make_pair(true, m_paramsKeeper);	
	}
	catch(const std::exception& ex) {

		std::cerr << "\nОшибка при обработке файла конфигурации [" << m_iniFilePath << "]. Причина: " 
		<< ex.what() << ". Проверьте права доступа или правильность его заполнения (Необходимые секции: " 
		<< pet::HTTP_DATA_PARAMS.first << ", " << pet::FILE_OBSERVER_PARAMS.first << ", " 
		<< pet::LOGS_TDB_PARAMS.first << ", " << pet::CONTENT_TDB_PARAMS.first << ", " 
		<< pet::PET_LOGGER_PARAMS.first << ")" << std::endl;
		
		return std::make_pair(false, pet::params_container_t{});		
	}
}