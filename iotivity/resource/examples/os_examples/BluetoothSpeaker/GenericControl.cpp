#ifndef GENERIC_CONTROL_CPP
#define GENERIC_CONTROL_CPP


/**
 * reference: https://webnautes.tistory.com/952
 */

#include "GenericControl.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>

#include <unistd.h>
#include <dirent.h>

#define PROBE_PATH	 "/sys/bus/w1/devices"
#define BUFFER_SIZE  256

static std::string probe_path;
static std::string probe_name;

template <class T>
GenericControl<T>::GenericControl(void)
{
	std::stringstream stream;
	struct dirent *dirent_ptr;
	DIR *dir_ptr;
	bool is_find = false;

	dir_ptr = opendir(PROBE_PATH);
	if (dir_ptr == NULL) {
		std::cerr << "Cannot open directory " << PROBE_PATH << std::endl;
		exit(-1);
	}

	while ((dirent_ptr = readdir(dir_ptr)) != NULL) {
		bool is_ds18b20 = false;
		stream << dirent_ptr->d_name[0] << dirent_ptr->d_name[1] << dirent_ptr->d_name[2];
		is_ds18b20 = (stream.str() == "28-");
		stream.str(std::string());
		if (is_ds18b20) { /* Currently, only one temp module can be supported */
			stream << PROBE_PATH << "/" << dirent_ptr->d_name << "/w1_slave";
			probe_path = stream.str();
			probe_name = dirent_ptr->d_name;
			is_find = true;
		}
	}
	closedir(dir_ptr);

	if (!is_find) {
		throw std::runtime_error("cannot find the DS18B20 device...");
	}
}

template <class T>
T GenericControl<T>::GetValue(void)
{
	FILE *probe_fd = fopen(probe_path.c_str(), "r");
	char buffer[BUFFER_SIZE];
	char *tokenizer;
	
	float temperature;

	if (probe_fd == NULL) {
		std::stringstream stream;
		stream << "Unable to open '" << probe_path << "': " << strerror(errno);
		throw std::runtime_error(stream.str());
	}

	/* Ignore the first line */
	fgets(buffer, sizeof(buffer) - 1, probe_fd);
	memset(buffer, 0, sizeof(buffer));

	fgets(buffer, sizeof(buffer) - 1, probe_fd);
	tokenizer = strtok(buffer, "t=");
	tokenizer = strtok(NULL, "t=");

	temperature = atof(tokenizer) / 1000;
	m_value = static_cast<T>(temperature);

	return m_value; 
}

template <class T>
void GenericControl<T>::SetValue(const T& value)
{
	std::stringstream stream;
	stream << "You currently use the NOT SUPPORTED method (param: " << value << ")";
	throw std::runtime_error(stream.str());
}

template <class T>
void GenericControl<T>::Control(void)
{
	std::stringstream stream;
	stream << "You currently use the NOT SUPPORTED method";
	throw std::runtime_error(stream.str());
}

#endif
