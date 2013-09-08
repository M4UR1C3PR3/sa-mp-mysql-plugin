#pragma once
#ifndef INC_CLOG_H
#define INC_CLOG_H


#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>


enum e_LogLevel {
	LOG_NONE = 0,
	LOG_ERROR = 1,
	LOG_WARNING = 2,
	LOG_DEBUG = 4
};

enum e_LogType {
	LOG_TYPE_TEXT = 1,
	LOG_TYPE_HTML = 2
};

namespace boost {
class thread;
}

class CLog {
public:
	static inline CLog *Get() {
		if(m_Instance == NULL)
			m_Instance = new CLog;
		return m_Instance;
	}
	static inline void Delete() {
		delete m_Instance;
	}


	void Initialize(const char *logfile);
	void LogFunction(unsigned status, char *funcname, char *msg, ...);
	void StartCallback(const char *cbname);
	void EndCallback();

	void SetLogLevel(unsigned int loglevel) {
		m_LogLevel = loglevel;
	}
	bool IsLogLevel(unsigned int loglevel) {
		return !!(m_LogLevel & loglevel);
	}
	void SetLogType(unsigned int logtype);
	
private:
	static CLog *m_Instance;
	
	struct m_SLogData {
		m_SLogData() :
			Status(LOG_NONE),
			Name(NULL), Msg(NULL),
			IsCallback(false),
			IsThreaded(false)
		{}

		unsigned int Status;
		char *Name, *Msg;
		
		bool IsCallback;
		bool IsThreaded;

		~m_SLogData() {
			free(Name);
			free(Msg);
		}
	};


	CLog() : 
		m_LogLevel(LOG_ERROR | LOG_WARNING), 
		m_LogThread(NULL), 
		m_LogThreadAlive(true),
		m_LogType(LOG_TYPE_TEXT),

		m_MainThreadID(0)
	{}
	~CLog();

	void ProcessLog();
	void TextLog(unsigned int level, char* text);

	
	char m_LogFileName[32];
	unsigned int m_LogType;
	unsigned int m_LogLevel;

	boost::thread *m_LogThread;
	boost::atomic<bool> m_LogThreadAlive;

	unsigned long m_MainThreadID;

	boost::lockfree::queue<
			m_SLogData*, 
			boost::lockfree::fixed_sized<true>,
			boost::lockfree::capacity<32678>
		> m_LogQueue;
};


#endif // INC_CLOG_H
