#include "inoty_wrapper.h"

/****** PRIVATES ******/

void inotifyRAIIwrapper::initInotyService(inoty::inoty_event_t eventType) {

    m_fd = inotify_init();

    if(m_fd == -1) {

        m_initDone = false;
        throw std::logic_error("Ошибка инициализации службы inotify");
    }

    m_wd = inotify_add_watch(m_fd, m_watchDirPath.c_str(), eventType);

    if(m_wd == -1) {

        m_initDone = false;
        throw std::logic_error("Ошибка при создании наблюдателя inotify");
    }
}

void inotifyRAIIwrapper::destroyInotyService() {

    if(m_initDone) {

        auto ret = inotify_rm_watch(m_fd, m_wd);

        if(ret) {

            throw std::logic_error("Ошибка при удалении наблюдателя inotify");
        }

        if(m_fd == -1) {

            throw std::logic_error("Ошибка деструкции inotify ->  invalid fd == -1");
        }
        else {

            ret = close(m_fd);

            if(ret) {

                throw std::logic_error("Ошибка деструкции inotify -> !close(fd)");
            }
        }
    }
}

void inotifyRAIIwrapper::createWatchDirIfNotExists() const {

    if(!fs::exists(m_watchDirPath)) {

        auto prevUmask = umask(7);
        
        try {
            
            fs::create_directories(m_watchDirPath);
            std::cout << "Создана директория для наблюдения [" << m_watchDirPath << "]" << std::endl;
        }
        catch (std::experimental::filesystem::filesystem_error err) {

            umask(prevUmask);
            std::cerr << "Не удалось создать директорию для наблюдения [" 
            << m_watchDirPath << "]. Причина: " << err.what() << std::endl;
        }
    }   
}

/****** PUBLICS ******/

inotifyRAIIwrapper::inotifyRAIIwrapper(const std::string& watchDirPath, inoty::inoty_event_t eventType) : m_watchDirPath(watchDirPath) {

    try {

        createWatchDirIfNotExists();
        initInotyService(eventType);
    }
    catch(std::logic_error& le) {

        std::cerr << "Ошибка в работе файлового наблюдателя: " << le.what() << std::endl;
    }
}

inotifyRAIIwrapper::~inotifyRAIIwrapper() {

    try {

        destroyInotyService();
    }
    catch(std::logic_error& le) {

        std::cerr <<  "Ошибка в работе файлового наблюдателя: " << le.what() << std::endl;
    }
}

void inotifyRAIIwrapper::setWatchFunction(std::function<bool(std::string)> func) {

    m_func = func;
}

bool inotifyRAIIwrapper::isInitDone() const {

    return m_initDone;
}

void inotifyRAIIwrapper::readInotyEvents() {

    m_i = 0;

    auto len = read(m_fd, m_buf, BUF_LEN);
    if(len != -1) {

        while(m_i < len) {

            struct inotify_event *event = (struct inotify_event*)&m_buf[m_i];

            if(event->len) {

                if(!m_func(std::string(event->name))) {

                    std::cerr << "Не удалось обработать событие связанное с файлом [" 
                    << std::string(event->name) << "]" << std::endl;
                }
            }

            m_i += sizeof(struct inotify_event) + event->len;
        }
    }  
}