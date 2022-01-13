#include "TDB_workers_fabrique.h"
	
std::unique_ptr<iworker> TDB_fabrique::produceTDB(TDB_type type, const pet::params_container_t& connParams) const {

	std::unique_ptr<iworker> TDworker{nullptr};

	switch(type) {
		
		case TDB_type::POSTGRESQL:
			
			TDworker = std::make_unique<postgreWorker>(connParams);
			break;

		case TDB_type::SQLITE:
			
			TDworker = std::make_unique<SQLiteWorker>(connParams);
			break;

		default:
			break;
	}

	return TDworker;
}