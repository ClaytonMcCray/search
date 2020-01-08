#ifndef WRITE_TO_STDOUT
#define WRITE_TO_STDOUT

#include <iostream>

class WriteToStdout {
	public:
	WriteToStdout()=default;


	template <typename T>
	void write(const T &one) const {
		std::cout << one << std::flush;
	}


	template <typename T, typename ...streamable>
	void write(const T &one, const streamable&... Args) const {
		std::cout << one;
		write(Args...);
	}
};

#endif
