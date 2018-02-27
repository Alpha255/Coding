//#include "CommandLine.h"
//#include "StatFileConverter.h"
//
//#include <intrin.h>
//#include <thread>
//
/////int main(int argc, const char *argv[])
/////{
/////	CommandLine::Parse(argc, argv);
/////
/////	StatFileConverter converter;
/////	converter.BuildStatFileList(CommandLine::GetInFileDir());
/////
/////	converter.DoConvert();
/////
/////	return 0;
/////}
//
//#include <mutex>
//#include <iostream>
//
//int global_counter = 0;
//std::mutex counter_mutex;
//
//void five_thread_fn() 
//{
//	for (int i = 0; i < 5; i++) 
//	{
//		counter_mutex.lock();
//		global_counter++;
//		counter_mutex.unlock();
//		std::cout << "Updated from five_thread...\r" << std::flush;
//		std::this_thread::sleep_for(std::chrono::seconds(5));
//	}
//}
//
//void ten_thread_fn() 
//{
//	for (int i = 0; i < 10; i++) 
//	{
//		counter_mutex.lock();
//		global_counter++;
//		counter_mutex.unlock();
//		std::cout << "Updated from ten_thread...\r" << std::flush;
//		std::this_thread::sleep_for(std::chrono::seconds(1));
//	}
//}
//int main() 
//{
//	std::thread ten_thread(ten_thread_fn);
//
//	std::thread five_thread(five_thread_fn);
//
//	ten_thread.join();
//	std::cout << "\nTen Thread is done." << std::endl;
//	five_thread.join();
//	std::cout << "\nFive Thread is done." << std::endl;
//}

//#include <iostream>
//#include <thread>
//#include <mutex>
//
//std::once_flag flag1, flag2;
//
//void simple_do_once()
//{
//	std::call_once(flag1, []() { std::cout << "Simple example: called once\n"; });
//	///std::cout << "Simple example: called once\n";
//}
//
//void may_throw_function(bool do_throw)
//{
//	if (do_throw) 
//	{
//		std::cout << "throw: call_once will retry\n"; // this may appear more than once
//		throw std::exception();
//	}
//	std::cout << "Didn't throw, call_once will not attempt again\n"; // guaranteed once
//}
//
//void do_once(bool do_throw)
//{
//	try 
//	{
//		std::call_once(flag2, may_throw_function, do_throw);
//	}
//	catch (...) 
//	{
//	}
//}
//
//int main()
//{
//	std::thread st1(simple_do_once);
//	std::thread st2(simple_do_once);
//	std::thread st3(simple_do_once);
//	std::thread st4(simple_do_once);
//	st1.join();
//	st2.join();
//	st3.join();
//	st4.join();
//
//	std::thread t1(do_once, true);
//	std::thread t2(do_once, true);
//	std::thread t3(do_once, false);
//	std::thread t4(do_once, true);
//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//
//	system("pause");
//}

#include <condition_variable>
#include <thread>
#include <iostream>
#include <mutex>
#include <future>

std::mutex g_Mutex;
std::condition_variable g_CondVar;

void ThreadFunc()
{
	std::cout << "[" << std::this_thread::get_id() << "] thread started." << std::endl;

	std::unique_lock<std::mutex> lock(g_Mutex);

	g_CondVar.wait(lock);
	///g_CondVar.wait_for(lock, std::chrono::seconds(1));
	///g_CondVar.wait_until(lock, std::chrono::system_clock::now() + std::chrono::seconds(5));

	std::cout << "[" << std::this_thread::get_id() << "] thread end." << std::endl;
}

//int main()
//{
//	std::cout << "[Main] create new thread." << std::endl;
//
//	std::thread t(ThreadFunc);
//
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//
//	std::cout << "[Main] send notify." << std::endl;
//
//	g_CondVar.notify_all();
//
//	std::cout << "[Main] wait thread stop." << std::endl;
//
//	t.join();
//
//	system("pause");
//}

bool IsPrime(int32_t x)
{
	std::cout << "Calculating, please wait..." << std::endl;

	for (int32_t i = 2U; i < x; ++i)
	{
		if (x % i == 0)
		{
			return false;
		}
	}

	return true;
}

int main()
{
	std::future<bool> fut = std::async(IsPrime, 313222313);

	std::cout << "Checking whether 313222313 is prime." << std::endl;

	if (fut.get())
	{
		std::cout << "It's prime" << std::endl;
	}
	else
	{
		std::cout << "It's not a prime" << std::endl;
	}

	system("pause");
}