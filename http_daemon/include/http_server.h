#ifndef HTTP_DAEMON_H
#define HTTP_DAEMON_H

#include <iostream>
#include <memory>
#include <sstream>
#include <experimental/filesystem>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <evhttp.h>

#include "base_daemon.h"
#include "TDB_workers_fabrique.h"

namespace fs = std::experimental::filesystem;

namespace pet {

	enum class get_type : uint8_t {

		GET_SONG_LIST = 0,
		GET_CONCRETE_SONG = 1,
		GET_UNKNOWN = 2,
	};

	const std::string getSongListURI{"get_song_list"};
	const std::string getSongURI{"get_by_uid"};

	struct requestInfo {

		std::string host{};
		std::string port{};
		std::string method{};		
		std::string URI{};
	};
};

class httpServer : public baseDaemon {

private:
	bool m_httpStartedOK{false};
	event_base* m_eventBase;
	std::unique_ptr<evhttp, decltype(&evhttp_free)> m_httpServer;

	std::unique_ptr<iworker> m_tdbworker_logs{nullptr};

private:
	bool initializeLibevent();
	bool initializeHTTP_Handler();
	bool startedOK() const;
	bool initTDBLogWorker();

	event_base* getEventBase() const;
	evhttp* getHttpServerInstance() const;
	struct evhttp* httpStart(std::string serverAddress, unsigned short portNumber);

	pet::get_type getRequestType(const std::string& URI) const;
	pet::requestInfo getRequestInfo(evhttp_request* const request) const;
	std::string getSongUIDOutOfURI(const std::string& URI) const;
	bool saveRequestAttemptToTDB(const pet::requestInfo& requestData) const;
	void ProcessRequest(evhttp_request *request) const;

public:
	httpServer(std::string host, unsigned short port, const std::string inifilePath, const std::string serviceName);

	static void httpRequestHandler(evhttp_request *request, void *serverInstance);

	bool initServer();
	void mainServerLoop();
};

#endif /* HTTP_DAEMON_H */
