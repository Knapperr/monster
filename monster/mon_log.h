#ifndef LOG_HG
#define LOG_HG

#include <ostream>
#include <sstream>
#include <fstream>
//#include <thread>
//#include <mutex>

namespace Mon 
{
	enum class Severity
	{
		ERR,
		WARN,
		TRACE
	};

	class Log
	{
	public:
		Log();

		static void shutdown();

		static void print(const char* msg);
		static void print(const char* title, const char* msg);
		static void print(const char* title, int val);
		static void print(const char* title, int a, int b);
		static void print(const char* title, float val);
		static void print(const char* title, float a, float b);
		static void print(const char* msg, Severity severity);
		static void warn(const char* msg);
	private:
		static std::ofstream file;

		// TODO(ck): Logger on separate thread
		//std::thread logThread;
		//std::mutex writeMtx;
		//std::vector<std::thread> threads;
		//std::queue<TaskData> tasks;
	};
}


#endif