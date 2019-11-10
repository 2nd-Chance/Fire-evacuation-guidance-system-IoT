#include "audio.h"

Audio::Audio() 
{
	int err;
	ao_initialize();
	driver = ao_default_driver_id();
	mpg123_init();
	mpg_handle = mpg123_new(NULL, &err);
	buffer_size = mpg123_outblock(mpg_handle);
	buffer = new unsigned char[buffer_size];
}

void Audio::open(const std::string& file_name)
{
    ao_sample_format format;
    int channels, encoding;
    long rate;
	mpg123_open(mpg_handle, file_name.c_str());
    mpg123_getformat(mpg_handle, &rate, &channels, &encoding);

    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);
}

void Audio::play()
{
	size_t done;
	int value = 0;
	while (mpg123_read(mpg_handle, buffer, buffer_size, &done) == MPG123_OK) {
		/*
		//// HOW TO USE STOP ////
		std::cout << value++ << std::endl;
		sleep(1);
		if (value > 25) {
			this->stop();
			std::cout << "stop" << std::endl;
			break;
		}
		*/
        ao_play(dev, (char *)buffer, done);
	}
}

void Audio::stop()
{
	ao_close(dev);
    mpg123_close(mpg_handle);
	memset(buffer, 0, buffer_size);
}

Audio::~Audio() 
{ 
	free(buffer);
    ao_close(dev);
    mpg123_close(mpg_handle);
    mpg123_delete(mpg_handle);
    mpg123_exit();
    ao_shutdown();
}

#ifdef AUDIO_TEST_MODE_ON
int main(int argc, char *argv[])
{
	std::unique_ptr<Audio> audio(new Audio());

	audio.get()->open("normal.mp3");
	audio.get()->play();
	audio.get()->open("emergency.mp3");
	audio.get()->play();

    return 0;
}
#endif
