#ifndef _IWORKER_H_
#define _IWORKER_H_

#include <string>

class iworker {
	
public:
	using json_t = std::string;
	using timestamp_t = std::string;

	virtual ~iworker(){};
	
	virtual json_t getSongsListAsJson() = 0;
	virtual bool execQuery(const std::string& TDBquery) = 0;
	virtual std::string getFilenameByUID(const std::string& songUID) = 0;

	virtual bool initWorker() = 0;
	virtual timestamp_t getTimestamp() = 0;

	virtual const std::string whoAmI() = 0;
};

namespace pet {

	const iworker::json_t errorTDBResponce{"{\"error\":\"Ошибка при получении данных из ТБД\"}"};
}
#endif /* _IWORKER_H_ */
