#include "GenericControl.h"
#include <iostream>
#include <thread>
#include <memory>
#include <ctime>
#include <csignal>

#define NSEC_TO_MSEC 1000000L

std::shared_ptr<GenericControl<double>> hello;

static void signalHandler(int signum)
{
	std::cout << "Signal ==>" << signum << std::endl;
	exit(signum);
}

static void thread_runner()
{
	struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 10*NSEC_TO_MSEC,
	};

	while (1) {
		std::cout << hello->GetValue() << std::endl;
		nanosleep(&ts,NULL);
	}
}

int main(void)
{
	hello = std::make_shared<GenericControl<double>>();

	signal(SIGINT, signalHandler);

	std::thread myThread(thread_runner);

	while(1)
		;
	return 0;
}
