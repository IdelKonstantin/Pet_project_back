#ifndef BASE_DAEMON
#define BASE_DAEMON

#include <csignal>
#include <iostream>
#include <memory>

#include "ini_file_parser.h"
#include "pet_logger.h"
#include "TDB_workers_fabrique.h"
#include "common_defs.h"
#include "СThreadPool.h"

class baseDaemon {

protected:
	std::unique_ptr<iniFileParser> m_ifp;
	std::unique_ptr<petLogger> m_pl{nullptr};
	std::unique_ptr<iworker> m_tdbworker_content{nullptr};

	const std::string m_serviceName;
	threadpool::CThreadPool m_th_pool;

	static void forceExit();

public:

	static bool m_canExit;

	static void signalHandler(int signal);
	bool canExit();

	baseDaemon() = delete;
	baseDaemon(const baseDaemon&) = delete;
	baseDaemon(const baseDaemon&&) = delete;

	baseDaemon(const std::string inifilePath, const std::string serviceName);
	bool initBaseDaemon();
	virtual ~baseDaemon();
};

#endif /* BASE_DAEMON */
