#ifndef _TDB_WORKERS_FABRIQUE_H_
#define _TDB_WORKERS_FABRIQUE_H_

#include <memory>

#include "TDB_iworkers.h"
#include "TDB_postgre_worker.h"
#include "TDB_SQLite_worker.h"
#include "ini_file_parser.h"

enum class TDB_type : uint8_t {

	POSTGRESQL = 0,
	SQLITE = 1
};

class TDB_fabrique final {

public:

	TDB_fabrique() = default;
	std::unique_ptr<iworker> produceTDB(TDB_type type, const pet::params_container_t& connParams) const;
};

#endif /* _TDB_WORKERS_FABRIQUE_H_ */