#ifndef AUDIO_H_
#define AUDIO_H_

#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <ao/ao.h>
#include <cstring>
#include <mpg123.h>

class Audio {
public:
	Audio();
	void open(const std::string& file_name);
	void play();
	void stop();
	~Audio();
private:
	const size_t BITS = 8;

	mpg123_handle *mpg_handle;
	int driver;
	int buffer_size;
	unsigned char *buffer;

	ao_device *dev;
};

#endif
