#include "compress.h"

std::string Compress::execute(const std::string &str,  const COMPRESS_OPTION option,
		const int comp_lv = Z_BEST_COMPRESSION)
{
	z_stream stream;
	std::string out_string = "";
	char out_buffer[out_buf_size];
	int ret, flush;

	int (*flate)(z_streamp, int);
	int (*flateEnd)(z_streamp);

	bool is_success_init = false;

	memset(&stream, 0, sizeof(stream));

	switch(option) {
	case COMPRESS_OPTION::COMPRESS:
		flate = deflate;
		flateEnd = deflateEnd;
		is_success_init = deflateInit(&stream, comp_lv) == Z_OK;
		flush = Z_FINISH;
		break;
	case COMPRESS_OPTION::DECOMPRESS:
		flate = inflate;
		flateEnd = inflateEnd;
		is_success_init = inflateInit(&stream) == Z_OK;
		flush = 0;
		break;
	default:
		std::clog << "invalid option! execution failed...." << std::endl;
		return out_string;
	}

	if (!is_success_init)
		throw(std::runtime_error("flateInit failed while compressing...."));

	stream.next_in = (Bytef*)str.data();
	stream.avail_in = str.size();


	do {
		stream.next_out = reinterpret_cast<Bytef*>(out_buffer);
		stream.avail_out = sizeof(out_buffer);

		ret = flate(&stream, flush);

		if (out_string.size() < stream.total_out) {
			out_string.append(out_buffer, stream.total_out - out_string.size());
		}
	} while (ret == Z_OK);

	flateEnd(&stream);

	if (ret != Z_STREAM_END) {
		std::ostringstream oss;
		oss << "Exception during zlib compression: (" << ret << ") " << stream.msg;
		throw(std::runtime_error(oss.str()));
	}

	return out_string;
}

#ifdef COMPRESS_TEST_MODE_ON

int main(void)
{
	std::string src = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum";

	std::cout << src.size() << std::endl;
	src = Compress::execute(src, COMPRESS_OPTION::COMPRESS);
	std::cout << src.size() << std::endl;
	std::cout << (src = Compress::execute(src, COMPRESS_OPTION::DECOMPRESS)) << std::endl;
	std::cout << src.size() << std::endl;

	return 0;
}

#endif
