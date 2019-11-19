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
	dev = NULL;
	should_kick_stop = false;
}

Audio::Audio(const Audio& audio)
{
	driver = audio.driver;
	mpg_handle = audio.mpg_handle;
	buffer_size = audio.buffer_size;
	buffer = new unsigned char[buffer_size];
	if (buffer != NULL)
		memcpy(buffer, audio.buffer, buffer_size);
	dev = audio.dev;
	should_kick_stop = audio.should_kick_stop;
}

void Audio::open(const std::string& file_name)
{
    ao_sample_format format;
    int channels, encoding;
    long rate;

	while (should_kick_stop) {
		std::cout << "Wait to stop the song..." << std::endl;
	}

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
	while (mpg123_read(mpg_handle, buffer, buffer_size, &done) == MPG123_OK) {
		if (should_kick_stop) {
			should_kick_stop = false;
			break;
		}
        ao_play(dev, (char *)buffer, done);
	}
}

void Audio::stop()
{
	should_kick_stop = true;
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
void counter(int seconds, std::shared_ptr<Audio> audio)
{
	while(seconds--) {
		sleep(1);
	}
	audio.get()->stop();
}

int main(int argc, char *argv[])
{
	std::shared_ptr<Audio> audio(new Audio());

	std::thread normal_thread(counter, 5, audio);
	audio.get()->open("normal.mp3");
	audio.get()->play();
	normal_thread.join();
	audio.get()->open("emergency.mp3");
	audio.get()->play();

    return 0;
}
#endif
