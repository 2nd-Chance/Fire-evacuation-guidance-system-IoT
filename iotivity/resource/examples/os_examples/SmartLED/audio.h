#ifndef AUDIO_H_
#define AUDIO_H_

#include <iostream>
#ifdef AUDIO_TEST_MODE_ON
#include <thread>
#endif
#include <memory>
#include <string>
#include <unistd.h>
#include <ao/ao.h>
#include <cstring>
#include <mpg123.h>

class Audio {
public:
	Audio();
	Audio(const Audio& audio);
	void open(const std::string& file_name);
	void play();
	void stop();
	~Audio();
private:
	void end_play();

	const size_t BITS = 8;

	mpg123_handle *mpg_handle;
	int driver;
	int buffer_size;
	bool should_kick_stop;
	unsigned char *buffer;

	ao_device *dev;
};

#endif
