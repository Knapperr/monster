#include "mon_log.h"


namespace Mon
{
	std::ofstream Log::file;
	Log::Log()
	{
		file = std::ofstream("monster.log");
	}

	void Log::shutdown()
	{
		file.close();
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
		file << "[INFO]:" << msg << '\n';
	}

	void Log::print(const char* title, const char* msg)
	{
		printf("[INFO]: %s: %s\n", title, msg);
		file << "[INFO]:" << title << " " << msg <<  '\n';
	}

	void Log::print(const char* title, int val)
	{
		printf("[INFO]: %s: %d\n", title, val);
		file << "[INFO]:" << title << ": " << std::to_string(val) << '\n';
	}
	
	void Log::print(const char* title, int a, int b)
	{
		printf("[INFO]: %s: %d, %d\n", title, a, b);
		file << "[INFO]:" << title << " " << std::to_string(a) <<  ", " << std::to_string(b) << '\n';

	}

	void Log::print(const char* title, float val)
	{
		printf("[INFO]: %s: %f\n", title, val);
		file << "[INFO]:" << title << ": " << std::to_string(val) << '\n';
	}

	void Log::print(const char* title, float a, float b)
	{
		printf("[INFO]: %s: %f, %f\n", title, a, b);
		file << "[INFO]:" << title << " " << std::to_string(a) << ", " << std::to_string(b) << '\n';

	}

	void Log::warn(const char* msg)
	{
		printf("[WARN]: %s\n", msg);
		file << "[WARN]:" << msg << '\n';
	}
}