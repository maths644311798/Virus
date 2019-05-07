#pragma once

#define EMPTY ""

//#include <boost/filesystem/operations.hpp>

#include <cryptopp/rsa.h>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/SecBlock.h>
#include <cryptopp/cryptlib.h>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <cryptopp/queue.h>
#include <cryptopp/integer.h>
#include <cryptopp/base64.h>
#include <assert.h>

using namespace CryptoPP;
typedef unsigned char byte;
#define IV_LEN CryptoPP::AES::BLOCKSIZE

struct crypt_data
{
	byte key[KEY_LEN];
	byte iv[IV_LEN];
};



//保存公钥和私钥
void SavePrivateKey(const std::string& filename, const PrivateKey& key);
void SavePublicKey(const std::string& filename, const PublicKey& key);


//base64编码
void SaveBase64PrivateKey(const std::string& filename, const PrivateKey& key);
void SaveBase64PublicKey(const std::string& filename, const PublicKey& key);


//保存
void Save(const std::string& filename, const BufferedTransformation& bt);
void SaveBase64(const std::string& filename, const BufferedTransformation& bt);


//读取key
void LoadPrivateKey(const std::string& filename, PrivateKey& key);
void LoadPublicKey(const std::string& filename, PublicKey& key);

void LoadBase64PrivateKey(const std::string& filename, PrivateKey& key);
void LoadBase64PublicKey(const std::string& filename, PublicKey& key);

void LoadBase64PrivateKey_str(const std::string& str_key, PrivateKey& key);
void LoadBase64PublicKey_str(const std::string& str_key, PublicKey& key);

void LoadBase64(const std::string& filename, BufferedTransformation &bt);
void Load(const std::string& filename, BufferedTransformation &bt);

void LoadBase64_str(const std::string& str_key, BufferedTransformation &bt);
//void Load_str(const std::string& str_key, BufferedTransformation &bt);


//////////////////////////////////////////////////////////////////////////////////////////////
//处理接口
void GetPUB(PublicKey& key);
void GetPRIVATE(PrivateKey& key, const std::string private_path);

void key_iv_encode_rsa(std::string &skey, std::string &siv,PublicKey& public_Key);//加密key和iv
void key_encode_rsa(std::string &skey,PublicKey& public_Key);//只加密key

void key_iv_decode_rsa(std::string &skey, std::string &siv, PrivateKey& private_Key);//解密key和iv
void key_decode_rsa(std::string &skey, PrivateKey& private_Key);//解密key

//final
void key_iv_encode_rsa(std::string &skey, std::string &siv);//加密key和iv
void key_encode_rsa(std::string &skey);//只加密key
void key_iv_decode_rsa(std::string &skey, std::string &siv, const std::string private_path);
void key_decode_rsa(std::string &skey, const std::string private_path);