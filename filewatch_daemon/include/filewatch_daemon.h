#ifndef WATCH_DAEMON
#define WATCH_DAEMON

#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>

#include "base_daemon.h"
#include "inoty_wrapper.h"
#include "CUUIDGenerator.h"

class fileWatch : public baseDaemon {

private:
	inotifyRAIIwrapper m_inotyInDelete;
	inotifyRAIIwrapper m_inotyInAdd;

private:
	void loopInDelete();
	void loopInAdd();
	bool deleteSongDataFromTDB(std::string filename) const;
	bool insertSongDataIntoTDB(std::string filename) const;

public:
	fileWatch(std::string watchDirPath, const std::string inifilePath, const std::string serviceName);

	bool initFileWatch();
	void runService();
};

#endif /* WATCH_DAEMON */
