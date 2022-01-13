#pragma once 

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <experimental/filesystem>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace inoty {

    using descriptor_t = int;
    using inoty_event_t = size_t;
}

#define BUF_LEN 0xFF*sizeof(struct inotify_event)
#define NOT_FOUND std::string::npos

namespace fs = std::experimental::filesystem;

class inotifyRAIIwrapper final {
	
private:

    std::function<bool(std::string)> m_func;

    inoty::descriptor_t m_fd{};
    inoty::descriptor_t m_wd{};
    std::string m_watchDirPath{};
    std::stringstream m_ss;
    
    bool m_initDone{true};

    char m_buf[BUF_LEN] __attribute__((aligned(4)));
    ssize_t m_i{0};

private:

    void initInotyService(inoty::inoty_event_t eventType);
    void destroyInotyService();
    void createWatchDirIfNotExists() const;

public:	

    inotifyRAIIwrapper() = delete;
    inotifyRAIIwrapper(const inotifyRAIIwrapper&) = delete;
    inotifyRAIIwrapper& operator=(const inotifyRAIIwrapper&) = delete;
    
    inotifyRAIIwrapper(const std::string& watchDirPath, inoty::inoty_event_t eventType);
    ~inotifyRAIIwrapper();

    void setWatchFunction(std::function<bool(std::string)> func);
    void readInotyEvents();
    bool isInitDone() const;
};