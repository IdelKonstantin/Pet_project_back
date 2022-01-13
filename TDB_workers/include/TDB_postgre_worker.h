#ifndef _TDB_POSTGRE_WORKER_H_
#define _TDB_POSTGRE_WORKER_H_

#include <iostream>
#include <memory>
#include <string>
#include <mutex>

#include "TDB_iworkers.h"
#include "libpq-fe.h"		/* /usr/include/postgresql/ */
#include "ini_file_parser.h"
#include "common_defs.h"

#define NO_PGOPTION nullptr
#define NO_PGTTY nullptr

class postgreWorker : public iworker {

private:
	std::mutex m_mtx;
	const pet::params_container_t m_connParams;

	PGconn* m_conn{nullptr};
	PGresult* m_res{nullptr};

private:
	void exitNicely();

public:
	postgreWorker(const pet::params_container_t connParams);
	~postgreWorker();

	iworker::json_t getSongsListAsJson() override;
	bool execQuery(const std::string& TDBquery) override;
	std::string getFilenameByUID(const std::string& songUID) override;

	bool initWorker() override;
	iworker::timestamp_t getTimestamp() override;

	const std::string whoAmI() override;
};

#endif /* _TDB_POSTGRE_WORKER_H_ */
