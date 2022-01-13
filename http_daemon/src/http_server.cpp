#include "http_server.h"

/****** PUBLICS ******/

httpServer::httpServer(std::string host, unsigned short port, const std::string inifilePath, const std::string serviceName) :
baseDaemon(inifilePath, serviceName), m_eventBase(nullptr), m_httpServer{httpStart(host.c_str(), port), &evhttp_free} {}

void httpServer::httpRequestHandler(evhttp_request *request, void *serverInstance) {

	reinterpret_cast<httpServer*>(serverInstance)->ProcessRequest(request);
}

bool httpServer::initTDBLogWorker() {

	/* База данных для хранения логов http-запросов */

	TDB_fabrique tdbf;
	auto TDBtype = m_ifp->getSectionContent(pet_params::HTTP_DATA).second.at("db_type");

	if(TDBtype == common_defs::postgreTDB) {
		
		m_tdbworker_logs = tdbf.produceTDB(TDB_type::POSTGRESQL, 
		m_ifp->getSectionContent(pet_params::LOGS_TDB).second);
	}
	else if(TDBtype == common_defs::sqliteTDB) {

		m_tdbworker_logs = tdbf.produceTDB(TDB_type::SQLITE, 
		m_ifp->getSectionContent(pet_params::LOGS_TDB).second);
	}
	else {
	
		m_pl->FAST_LOG(CODE_POSITION() + "Демону не удалось создать worker для логгирования http-запросов (указан неверный тип ТБД логов в конфиге)");
		std::cerr << "Демон [" << m_serviceName << "] не стартовал (указан неверный тип ТБД логов в конфиге)" << std::endl;
		return false;
	}

	if(!m_tdbworker_logs->initWorker()) {

		m_pl->FAST_LOG(CODE_POSITION() + "Демону не удалось создать worker для логгирования http-запросов (не удалось инициализировать службу ТБД логов)");
		std::cerr << "Демон [" << m_serviceName << "] не стартовал (не удалось инициализировать службу ТБД логов)" << std::endl;
		return false;
	}

	std::cerr << "Демон [" << m_serviceName << "] сохраняет логи запросов в ТБД [" 
	<< m_tdbworker_logs->whoAmI() << "]" << std::endl;
	return true;
}

bool httpServer::initServer() {

	if(!initBaseDaemon()) {

		std::cerr << "Ошибка в инициализации базового демона!" << std::endl;
		return false;
	}

	if(m_pl == nullptr || m_tdbworker_content == nullptr) {

		std::cerr << "Ошибка! Логгер или обработчик ТБД не валидны!" << std::endl;
		std::cerr << "m_pl == nullptr -> " << std::boolalpha << (m_pl == nullptr) 
		<< ", m_tdbworker_content == nullptr -> " << std::boolalpha 
		<< (m_tdbworker_content == nullptr) << std::endl;
		return false;
	}

	if(!initTDBLogWorker() || m_tdbworker_logs == nullptr) {

		return false;
	}

	if (!m_httpServer || !startedOK()) {

		m_pl->FAST_LOG(CODE_POSITION() + "Http-сервер не стартовал");
		return false;
	}

	if(!initializeHTTP_Handler()) {

		m_pl->FAST_LOG(CODE_POSITION() + "Не удалось инициализировать callback");
		return false;
	}

	m_pl->FAST_LOG(CODE_POSITION() + "Http-сервер успешно запущен");
	return true;
}

void httpServer::mainServerLoop() {

	while(!canExit()) {

		if(event_base_loop(getEventBase(), EVLOOP_NONBLOCK)) {
		
			m_pl->FAST_LOG(CODE_POSITION() + "Не удалось запустить обработчик сообщений [event_base_loop]");
			m_canExit = true;
		}
	}
}

/****** PRIVATES ******/

void httpServer::ProcessRequest(evhttp_request *request) const {

	auto *outBuf = evbuffer_new();

	if(!outBuf) {

		m_pl->FAST_LOG(CODE_POSITION() + "Не удалось создать out-буфер");
		return;
	}

	auto requestData = getRequestInfo(request);
	const std::string errorWrongURIMsg{"empty URI (may be: /" + pet::getSongListURI + " or /" + pet::getSongURI + "?uid=SONG_FILE_UID)"};

	if(requestData.URI.empty()) {

		m_pl->FAST_LOG(CODE_POSITION() + "В запросе от [" + requestData.host + ":" 
		+ requestData.port + "] получен пустой URI");

		evbuffer_add_printf(outBuf, "%s", errorWrongURIMsg.c_str());
		evhttp_send_reply(request, HTTP_BADREQUEST, "", outBuf);
		evbuffer_free(outBuf);
		return;
	}

	if(!saveRequestAttemptToTDB(requestData)) {

		m_pl->FAST_LOG(CODE_POSITION() + "Параметры запроса не сохранены в ТБД");
	}

	switch(getRequestType(requestData.URI)) {

		case pet::get_type::GET_SONG_LIST: {

			std::string songList = m_tdbworker_content->getSongsListAsJson();

			m_pl->FAST_LOG(CODE_POSITION() + "JSON со списком песен отправлен получателю [" 
			+ requestData.host + ":" + requestData.port + "]");

			evbuffer_add_printf(outBuf, "%s", songList.c_str());
			evhttp_add_header(request->output_headers, "Content-Type", "application/json");
			evhttp_send_reply(request, HTTP_OK, "", outBuf);
			evbuffer_free(outBuf);
			return;

		}
		case pet::get_type::GET_CONCRETE_SONG: {
			
			std::string songUID = getSongUIDOutOfURI(requestData.URI);
			std::string downloadingFilename = m_tdbworker_content->getFilenameByUID(songUID);

			fs::path downloadingFilePath{std::string(m_ifp->getSectionContent(pet_params::FILE_OBSERVER).second.at("dir") + "/" + downloadingFilename)};

			if(downloadingFilename.empty() || (!fs::exists(downloadingFilePath))) {

				m_pl->FAST_LOG(CODE_POSITION() + "Не найден файл с UID [" + songUID + "]");

				evbuffer_add_printf(outBuf, "File with UID [%s] not found", songUID.c_str());
				evhttp_send_reply(request, HTTP_NOTFOUND, "File not found", outBuf);
				evbuffer_free(outBuf);
				return;
			}

			auto fd = open(downloadingFilePath.c_str(), O_RDONLY);

			if(fd < 0) {

				m_pl->FAST_LOG(CODE_POSITION() + "Не удалось открыть файл [" + downloadingFilePath.string() + "]");
				
				evbuffer_add_printf (outBuf, "Can't open file [%s] for downloading", downloadingFilePath.c_str());
				evhttp_send_reply(request, HTTP_NOTFOUND, "File not found", outBuf);
				evbuffer_free(outBuf);
				return;
			}

			struct stat fileInfo;

			if(fstat(fd, &fileInfo) < 0) {

				m_pl->FAST_LOG(CODE_POSITION() + "Не удалось получить информацию о файле [" + downloadingFilePath.string() + "]");

				evbuffer_add_printf (outBuf, "Can't open file [%s] for downloading", downloadingFilePath.c_str());
				evhttp_send_reply(request, HTTP_NOTFOUND, "File not found", outBuf);
				evbuffer_free(outBuf);
				close (fd);
				return;
			}

			if(fileInfo.st_size == 0 || evbuffer_add_file(outBuf, fd, 0, fileInfo.st_size) != 0) {

				m_pl->FAST_LOG(CODE_POSITION() + "Файл [" + downloadingFilePath.string() + "] имеет нулевой размер");

				evbuffer_add_printf (outBuf, "Can't open file [%s] for downloading", downloadingFilePath.c_str());
				evhttp_send_reply(request, HTTP_NOTFOUND, "File not found", outBuf);
				evbuffer_free(outBuf);
				close (fd);
				return;
			}

			m_pl->FAST_LOG(CODE_POSITION() + "Файл [" + downloadingFilename 
			+ "] отправлен получателю [" + requestData.host + ":" + requestData.port + "]");

			std::string attachmentInfo{"attachment; filename=\"" + downloadingFilename + "\""};

			evhttp_add_header(request->output_headers, "Content-Disposition", attachmentInfo.c_str());
			evhttp_add_header(request->output_headers, "Content-Type", "audio/mpeg");
			evhttp_send_reply(request, HTTP_OK, "", outBuf);
			evbuffer_free(outBuf);
			close (fd);
			return;
		}
		case pet::get_type::GET_UNKNOWN: {
			
			m_pl->FAST_LOG(CODE_POSITION() + "Получен запрос с неверным URI [" + requestData.URI + "]");
			
			evbuffer_add_printf(outBuf, "%s", errorWrongURIMsg.c_str());
			evhttp_send_reply(request, HTTP_BADREQUEST, "", outBuf);
			evbuffer_free(outBuf);
			return;
		}

		default:
		return;
	};
}

bool httpServer::saveRequestAttemptToTDB(const pet::requestInfo& requestData) const {

	std::stringstream ss;

	ss << "INSERT INTO request_logs (hostname, port, rest_method, uri, req_time) VALUES ('" 
	<< requestData.host << "', '" << requestData.port << "', '" << requestData.method 
	<< "', '" << requestData.URI << "', " << m_tdbworker_logs->getTimestamp() << ");";

	return m_tdbworker_logs->execQuery(ss.str());
}

pet::get_type httpServer::getRequestType(const std::string& URI) const {

	if(URI.find(pet::getSongListURI) != std::string::npos) {

		return pet::get_type::GET_SONG_LIST;
	}
	else if(URI.find(pet::getSongURI) != std::string::npos) {

		return pet::get_type::GET_CONCRETE_SONG;
	}
	return pet::get_type::GET_UNKNOWN;
}

bool httpServer::initializeLibevent() {

	m_eventBase = event_init();
	
	if(!m_eventBase) {

		m_pl->FAST_LOG(CODE_POSITION() + "Ошибка инициализации библиотеки libevent");
		return false;
	}
	return true;
}

bool httpServer::initializeHTTP_Handler() {

	if(m_httpStartedOK) {

		evhttp_set_allowed_methods(m_httpServer.get(), EVHTTP_REQ_GET);
		evhttp_set_gencb(m_httpServer.get(), &httpServer::httpRequestHandler, this);
		evhttp_set_default_content_type(getHttpServerInstance(), "text/plain");
		return true;
	}
	return false;
}

struct evhttp* httpServer::httpStart(std::string serverAddress, unsigned short portNumber) {

	if(!initializeLibevent()) {

		return nullptr;
	}

	evhttp* evHttp =  evhttp_start(serverAddress.c_str(), portNumber);
	m_httpStartedOK = evHttp ? true : false;
	return evHttp;
}

bool httpServer::startedOK() const {

	return m_httpStartedOK;	
}

event_base* httpServer::getEventBase() const {

	return m_eventBase;
}

evhttp* httpServer::getHttpServerInstance() const {

	return m_httpServer.get();
}

pet::requestInfo httpServer::getRequestInfo(evhttp_request* const request) const {

	pet::requestInfo reqInfo;
	const std::string notDefined{"N/A"};

	/********** Host & port **********/
	char* clientIP = nullptr;
	unsigned short clientPort = 0;
	
	evhttp_connection_get_peer(evhttp_request_get_connection(request), &clientIP, &clientPort);

	if(clientIP && clientPort) {

		reqInfo.host = clientIP;
		reqInfo.port = std::to_string(clientPort);
	}
	else {

		reqInfo.host = notDefined;
		reqInfo.port = notDefined;
	}

	/********** REST API method **********/
	std::unordered_map<evhttp_cmd_type, const char*> requestType {

		{EVHTTP_REQ_GET, "GET"},
		{EVHTTP_REQ_POST, "POST"},
		{EVHTTP_REQ_HEAD, "HEAD"},
		{EVHTTP_REQ_PUT, "PUT"},
		{EVHTTP_REQ_DELETE, "DELETE"},
		{EVHTTP_REQ_OPTIONS, "OPTIONS"},
		{EVHTTP_REQ_TRACE, "TRACE"},
		{EVHTTP_REQ_CONNECT, "CONNECT"},
		{EVHTTP_REQ_PATCH, "PATCH"}
	};

	try {

		reqInfo.method = requestType.at(evhttp_request_get_command(request));
	}
	catch(const std::out_of_range& ex) {

		reqInfo.method = notDefined;
	}

	/********** URI **********/
	reqInfo.URI = evhttp_request_get_uri(request);
	return reqInfo;
}

std::string httpServer::getSongUIDOutOfURI(const std::string& URI) const {

	const std::string uriTag{"uid="};
	const size_t UIDsize{32};

	auto begPos = URI.find(uriTag);

	if(begPos == std::string::npos) {

		return std::string("N/A");
	}

	begPos += uriTag.length();

	return URI.substr(begPos, begPos + UIDsize);
}