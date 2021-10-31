#include "mon_log.h"


namespace Mon
{
	void Log::init()
	{
		// TODO(CK): Init file stream?
		return;
	}

	void Log::print(const char* msg, Severity severity)
	{
		// TODO(CK): Don't print to console unless #define VERBOSE 
		// just write to a file
		switch (severity)
		{
		case Severity::TRACE:
			printf("TRACE: %s\n", msg);
			break;
		case Severity::WARN:
			printf("[WARN]: %s\n", msg);
			break;
		case Severity::ERR:
			printf("ERROR: %s\n", msg);
			break;
		}
	}

	void Log::print(const char* msg)
	{
		printf("[INFO]: %s\n", msg);
	}

	void Log::print(const char* title, const char* msg)
	{
		printf("[INFO]: %s: %s\n", title, msg);
	}

	void Log::print(const char* title, int msg)
	{
		printf("[INFO]: %s: %d\n", title, msg);
	}
	
	void Log::print(const char* title, int a, int b)
	{
		printf("[INFO]: %s: %d, %d\n", title, a, b);
	}

	void Log::warn(const char* msg)
	{
		printf("[WARN]: %s\n", msg);
	}


	// TODO(ck): Log
}