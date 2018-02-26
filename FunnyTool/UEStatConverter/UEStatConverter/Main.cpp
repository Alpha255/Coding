#include "CommandLine.h"
#include "StatFileConverter.h"

#include <intrin.h>
#include <thread>

///int main(int argc, const char *argv[])
///{
///	CommandLine::Parse(argc, argv);
///
///	StatFileConverter converter;
///	converter.BuildStatFileList(CommandLine::GetInFileDir());
///
///	converter.DoConvert();
///
///	return 0;
///}

#include <mutex>
#include <iostream>

int global_counter = 0;
std::mutex counter_mutex;

void five_thread_fn() 
{
	for (int i = 0; i < 5; i++) 
	{
		counter_mutex.lock();
		global_counter++;
		counter_mutex.unlock();
		std::cout << "Updated from five_thread...\r" << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

void ten_thread_fn() 
{
	for (int i = 0; i < 10; i++) 
	{
		counter_mutex.lock();
		global_counter++;
		counter_mutex.unlock();
		std::cout << "Updated from ten_thread...\r" << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
int main() 
{
	std::thread ten_thread(ten_thread_fn);

	std::thread five_thread(five_thread_fn);

	ten_thread.join();
	std::cout << "\nTen Thread is done." << std::endl;
	five_thread.join();
	std::cout << "\nFive Thread is done." << std::endl;
}