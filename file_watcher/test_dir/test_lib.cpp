#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"

#include <functional>
#include <thread>
#include "inoty_wrapper.h"

bool echo(std::string text) {

	std::cerr << "\nProcessing file: " << text << std::endl;
	return true;
}

TEST_CASE("Test of inoty (normal modes)", "") {

	SECTION("Test of inoty (IN_DELETE)") {

		std::cout << "Test of inoty (IN_DELETE)\n";

		inotifyRAIIwrapper irw{"./SONGS", IN_DELETE | IN_MOVED_FROM};
		irw.setWatchFunction(echo);

		REQUIRE(irw.isInitDone());

		for(auto i = 1; i <= 5; i++) {
			
			std::cout << "Test of inoty (IN_DELETE | IN_MOVED_FROM) case = " << i << std::endl;
			irw.readInotyEvents();
		}
		
		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of inoty (IN_MOVED_TO)") {

		std::cout << "Test of inoty (IN_MOVED_TO)\n";

		inotifyRAIIwrapper irw{"./SONGS", IN_CREATE | IN_MOVED_TO};
		irw.setWatchFunction(echo);

		REQUIRE(irw.isInitDone());

		for(auto i = 1; i <= 5; i++) {
			
			std::cout << "Test of inoty (IN_CREATE | IN_MOVED_TO) case = " << i << std::endl;
			irw.readInotyEvents();
		}

		std::cout << " -- PASSED" << std::endl;
	};

	SECTION("Test of inoty (multitasking)") {

		std::cout << "Test of inoty (multitasking)";

		inotifyRAIIwrapper irw_1{"./SONGS", IN_DELETE};
		irw_1.setWatchFunction(echo);

		inotifyRAIIwrapper irw_2{"./SONGS", IN_MOVED_TO | IN_CREATE};
		irw_2.setWatchFunction(echo);

		REQUIRE(irw_1.isInitDone());
		REQUIRE(irw_2.isInitDone());

		std::thread th1{&inotifyRAIIwrapper::readInotyEvents, &irw_1};
		std::thread th2{&inotifyRAIIwrapper::readInotyEvents, &irw_2};

		th1.join();
		th2.join();

		std::cout << " -- PASSED" << std::endl;
	};
}

int main (int argc, char* argv[]) {

	return Catch::Session().run();
}
