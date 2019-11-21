
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/md5.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <iostream>
#include <sstream>

int main (int argc, char* argv[])
{
    using namespace CryptoPP;

    std::stringstream result;

    HexEncoder encoder(new FileSink(result));

    Weak::MD5 hash;
    
    std::string msg = "BluetoothSpeaker";
    std::string crypted;

    hash.Update((const byte*)msg.data(), msg.size());
    crypted.resize(hash.DigestSize());
    hash.Final((byte*)&crypted[0]);
    StringSource(crypted, true, new Redirector(encoder));
    std::cout << "Message: " << msg << std::endl;
    std::cout << "Digest: " << result.str() << std::endl;

    return 0; 
}
