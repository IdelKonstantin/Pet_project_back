#ifndef _COMMON_STUFF_H_
#define _COMMON_STUFF_H_

#include <iostream>

namespace common_defs {

	const std::string logFieldsDelimeter{" | "};
	constexpr size_t mbToBytes{1024*1024};

	/* Pet logger param names */
	const std::string logDirPathParam{"dir"};
	const std::string logMaxSizeinMbParam{"max_size_mb"};

	/* possible TDB types */
	const std::string postgreTDB{"POSTGRESQL"};
	const std::string sqliteTDB{"SQLITE"};

	namespace exit_code {

		const uint8_t NORMAL_EXIT = 0;
		const uint8_t WRONG_ARGS = 1;
		const uint8_t NO_CONFIG = 2;
		const uint8_t CANT_INIT = 3;
		const uint8_t ERROR_AFTER_EXCEPTION = 4;
	}
}

#endif /* _COMMON_STUFF_H */
