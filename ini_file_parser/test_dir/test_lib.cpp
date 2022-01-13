#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

#include "ini_file_parser.h"

TEST_CASE("Test of ini_parser", "") {

	SECTION("Test of section content for [HTTP_DATA]") {

		std::cout << "Test of section content for [HTTP_DATA]";

		iniFileParser ifp{"ini_example.ini"};
		auto content = ifp.getSectionContent(pet_params::HTTP_DATA);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == true);
		REQUIRE(sectionData.at("host") == "172.23.137.31");
		REQUIRE(sectionData.at("port") == "6667");
		REQUIRE(sectionData.at("db_type") == "SQLITE");

		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of section content for [FILE_OBSERVER]") {

		std::cout << "Test of section content for [FILE_OBSERVER]";

		iniFileParser ifp{"ini_example.ini"};
		auto content = ifp.getSectionContent(pet_params::FILE_OBSERVER);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == true);
		REQUIRE(sectionData.at("dir") == "./");
		REQUIRE(sectionData.at("db_type") == "POSTGRESQL");

		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of section content for [LOGS_TDB]") {

		std::cout << "Test of section content for [LOGS_TDB]";

		iniFileParser ifp{"ini_example.ini"};
		auto content = ifp.getSectionContent(pet_params::LOGS_TDB);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == true);
		REQUIRE(sectionData.at("host") == "127.0.0.1");
		REQUIRE(sectionData.at("port") == "5432");
		REQUIRE(sectionData.at("user") == "pguser");
		REQUIRE(sectionData.at("dbname") == "pet_project_logs");
		REQUIRE(sectionData.at("passwd") == "pgpassword");

		std::cout << " -- PASSED" << std::endl;
	};
	
	SECTION("Test of section content for [CONTENT_TDB]") {

		std::cout << "Test of section content for [CONTENT_TDB]";

		iniFileParser ifp{"ini_example.ini"};
		auto content = ifp.getSectionContent(pet_params::CONTENT_TDB);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == true);
		REQUIRE(sectionData.at("host") == "172.23.137.32");
		REQUIRE(sectionData.at("port") == "7778");
		REQUIRE(sectionData.at("user") == "postgres");
		REQUIRE(sectionData.at("dbname") == "pet_project_songs");
		REQUIRE(sectionData.at("passwd") == "my_password");

		std::cout << "-- PASSED" << std::endl;
	};

	SECTION("Test of section content for [PET_LOGGER]") {

		std::cout << "Test of section content for [PET_LOGGER]";

		iniFileParser ifp{"ini_example.ini"};
		auto content = ifp.getSectionContent(pet_params::PET_LOGGER);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == true);
		REQUIRE(sectionData.at("dir") == "../LOGS");
		REQUIRE(sectionData.at("max_size_mb") == "1");

		std::cout << "-- PASSED" << std::endl;
	};
}

TEST_CASE("Test of ini_parser fails", "") {

		SECTION("Test of fail section content for [HTTP_DATA]") {

		std::cout << "Test of fail section content for [HTTP_DATA]";

		iniFileParser ifp{"blablabla.ini"};
		auto content = ifp.getSectionContent(pet_params::HTTP_DATA);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == false);
		
		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of fail section content for [FILE_OBSERVER]") {

		std::cout << "Test of fail section content for [FILE_OBSERVER]";

		iniFileParser ifp{"blablabla.ini"};
		auto content = ifp.getSectionContent(pet_params::FILE_OBSERVER);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == false);
		
		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of fail section content for [LOGS_TDB]") {

		std::cout << "Test of fail section content for [LOGS_TDB]";

		iniFileParser ifp{"blablabla.ini"};
		auto content = ifp.getSectionContent(pet_params::LOGS_TDB);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == false);
		
		std::cout << " -- PASSED" << std::endl;
	};
	
	SECTION("Test of fail section content for [CONTENT_TDB]") {

		std::cout << "Test of fail section content for [CONTENT_TDB]";

		iniFileParser ifp{"blablabla.ini"};
		auto content = ifp.getSectionContent(pet_params::CONTENT_TDB);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == false);
		
		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of fail section content for [PET_LOGGER]") {

		std::cout << "Test of fail section content for [PET_LOGGER]";

		iniFileParser ifp{"blablabla.ini"};
		auto content = ifp.getSectionContent(pet_params::PET_LOGGER);
		auto result = content.first;
		auto sectionData = content.second;

		REQUIRE(result == false);
		
		std::cout << "-- PASSED" << std::endl;
	};
}

int main () {

	return Catch::Session().run();
}