#ifndef LOG_HG
#define LOG_HG

#include <ostream>
#include <sstream>

namespace Mon 
{
	enum class Severity
	{
		ERR,
		WARN,
		TRACE
	};

	// TODO(ck): add ofstream and write to it through logging process


	class Log
	{
	public:
		static void init();
		static void print(const char* msg);
		static void print(const char* title, const char* msg);
		static void print(const char* title, int msg);
		static void print(const char* title, int a, int b);
		static void print(const char* msg, Severity severity);
		static void warn(const char* msg);

	};
}


#endif