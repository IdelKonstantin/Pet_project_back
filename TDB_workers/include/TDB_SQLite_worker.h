#ifndef _TDB_SQLITE_WORKER_H_
#define _TDB_SQLITE_WORKER_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <mutex>
#include <sys/time.h>
#include <stdio.h>

#include "sqlite3.h"
#include "TDB_iworkers.h"
#include "ini_file_parser.h"
#include "common_defs.h"

#define JSON_EXTENSION_SO_PATH "/usr/lib/libjson1.so"

class SQLiteWorker : public iworker {

private:
	std::mutex m_mtx;
	const pet::params_container_t m_connParams;

	sqlite3* m_db{nullptr};
	char* m_zErrMsg{nullptr};
	sqlite3_stmt* m_reqResult{nullptr};

	static int callback(void *NotUsed, int argc, char **argv, char **azColName);

public:
	SQLiteWorker(const pet::params_container_t connParams);
	~SQLiteWorker();

	void openConnection();
	void closeConnection();

	iworker::json_t getSongsListAsJson() override;
	bool execQuery(const std::string& TDBquery) override;
	std::string getFilenameByUID(const std::string& songUID) override;

	bool initWorker() override;
	iworker::timestamp_t getTimestamp() override;

	const std::string whoAmI() override;
};

class SQLiteConnectionWrapper {

private:
	SQLiteWorker* m_worker{nullptr};

public:
	SQLiteConnectionWrapper(SQLiteWorker* worker) : m_worker(worker) {

		m_worker->openConnection();
	}

	~SQLiteConnectionWrapper() {

		m_worker->closeConnection();
	}
};

#endif /* _TDB_SQLITE_WORKER_H_ */
