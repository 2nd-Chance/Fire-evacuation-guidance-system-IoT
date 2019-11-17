#include "GenericControl.h"
#include <iostream>
#include <thread>
#include <memory>
#include <ctime>
#include <csignal>

#define NSEC_TO_MSEC 1000000L

std::shared_ptr<GenericControl<int>> hello;

static void signalHandler(int signum)
{
	std::cout << "Signal ==>" << signum << std::endl;
	hello->SetValue(-1);
	hello->Control();
	exit(signum);
}

static void thread_runner()
{
	struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 500*NSEC_TO_MSEC,
	};

	while (1) {
		hello->Control();
		nanosleep(&ts,NULL);
	}
}

int main(void)
{
	int value = 0;
	struct timespec ts = {
		.tv_sec = 1,
		.tv_nsec = 0
	};


	hello = std::make_shared<GenericControl<int>>();

	signal(SIGINT, signalHandler);

	std::thread myThread(thread_runner);

	while(1) {
		hello->SetValue((value++ % 3));
		nanosleep(&ts,NULL);
	}
	return 0;
}
