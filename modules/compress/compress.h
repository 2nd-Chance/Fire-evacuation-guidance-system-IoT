#ifndef COMPRESS_H_
#define COMPRESS_H_

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sstream>
#include <memory>
#include <zlib.h>

enum class COMPRESS_OPTION {
	COMPRESS,
	DECOMPRESS,
};

class Compress
{
public:
	Compress(void);
	static std::string execute(const std::string&, const COMPRESS_OPTION, const int);
private:
	static const size_t out_buf_size = 32768;
};

#endif
