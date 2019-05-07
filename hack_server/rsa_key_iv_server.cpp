#include "stdafx.h"
#include "rsa_key_iv_server.h"

std::string GLOBAL_PUB = "MIGdMA0GCSqGSIb3DQEBAQUAA4GLADCBhwKBgQC1aYEoc5PqadIC5gP9UlXLnf0F1LVsdUOg\n3pdsogrCWMet4Gy8pbrvR6QGbx2lGj99uKBx0R8yTRCxqdNnBbaBVtGVZB5U67zA72xy0RPo\ntcGX1S3o77uz3dD58J5EROMCLeYoTgQBjmT5o50ZrXn3KXybtX5Mb0lr0XqgKLNlPQIBEQ==";

void Save(const std::string& filename, const BufferedTransformation& bt)
{
	FileSink file(filename.c_str());

	bt.CopyTo(file);
	file.MessageEnd();
}

void SaveBase64(const std::string& filename, const BufferedTransformation& bt)
{
	Base64Encoder encoder;

	bt.CopyTo(encoder);
	encoder.MessageEnd();

	Save(filename, encoder);
}

void SavePrivateKey(const std::string& filename, const PrivateKey& key)
{
	ByteQueue queue;
	key.Save(queue);

	Save(filename, queue);
}

void SavePublicKey(const std::string& filename, const PublicKey& key)
{
	ByteQueue queue;
	key.Save(queue);

	Save(filename, queue);
}

void SaveBase64PrivateKey(const std::string& filename, const PrivateKey& key)
{
	ByteQueue queue;
	key.Save(queue);

	SaveBase64(filename, queue);
}

void SaveBase64PublicKey(const std::string& filename, const PublicKey& key)
{
	ByteQueue queue;
	key.Save(queue);

	SaveBase64(filename, queue);
}

void LoadPrivateKey(const std::string& filename, PrivateKey& key)
{
	ByteQueue queue;

	Load(filename, queue);
	key.Load(queue);
}

void LoadPublicKey(const std::string& filename, PublicKey& key)
{
	ByteQueue queue;

	Load(filename, queue);
	key.Load(queue);
}

void Load(const std::string& filename, BufferedTransformation& bt)
{
	FileSource file(filename.c_str(), true /*pumpAll*/);

	file.TransferTo(bt);
	bt.MessageEnd();
}

//void Load_str(const std::string& str_key, BufferedTransformation& bt)
//{
//	StringSource str_tmp();
//
//	str_tmp.TransferTo(bt);
//	bt.MessageEnd();
//}

void LoadBase64(const std::string& filename, BufferedTransformation& bt)
{
	Base64Decoder decoder;
	Load(filename, decoder);

	decoder.CopyTo(bt);
	bt.MessageEnd();
}

void LoadBase64_str(const std::string& str_key, BufferedTransformation& bt)
{
	Base64Decoder decoder;
	//Load_str(str_key, decoder);
	decoder.PutMessageEnd((const byte *)str_key.data(), str_key.size(), -1, true);

	decoder.CopyTo(bt);
	bt.MessageEnd();
}

void LoadBase64PrivateKey(const std::string& filename, PrivateKey& key)
{
	ByteQueue queue;

	LoadBase64(filename, queue);//file to ByteQueue
	key.Load(queue);
}

void LoadBase64PublicKey(const std::string& filename, PublicKey& key)
{
	ByteQueue queue;

	LoadBase64(filename, queue);
	key.Load(queue);
}

void LoadBase64PrivateKey_str(const std::string& str_key, PrivateKey& key)
{
	ByteQueue queue;

	LoadBase64_str(str_key, queue);//file to ByteQueue
	key.Load(queue);
}

void LoadBase64PublicKey_str(const std::string& str_key, PublicKey& key)
{
	ByteQueue queue;

	LoadBase64_str(str_key, queue);
	key.Load(queue);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetPUB(PublicKey& key) {
	LoadBase64PublicKey_str(GLOBAL_PUB, key);
}

void GetPRIVATE(PrivateKey& key, const std::string private_path = "rsa-base64-private.key") {
	try
	{
		LoadBase64PrivateKey(private_path, key);
	}
	catch (CryptoPP::Exception& e)
	{
		std::cerr << "Caught Exception..." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void key_iv_encode_rsa(std::string &skey, std::string &siv, PublicKey& public_Key) {
	try
	{
		AutoSeededRandomPool rng;
		RSAES_OAEP_SHA_Encryptor encryptor(public_Key);
		std::string tmp1;
		std::string tmp2;

		StringSource(skey, true,
			new PK_EncryptorFilter(rng, encryptor,
				new StringSink(tmp1)
			)
		);
		skey = tmp1;
		
		StringSource(siv, true,
			new PK_EncryptorFilter(rng, encryptor,
				new StringSink(tmp2)
			)
		);
		siv = tmp2;
	}
	catch (CryptoPP::Exception& e)
	{
		std::cerr << "Caught Exception..." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void key_encode_rsa(std::string &skey, PublicKey& public_Key) {
	try
	{
		AutoSeededRandomPool rng;
		RSAES_OAEP_SHA_Encryptor encryptor(public_Key);
		std::string tmp;

		StringSource(skey, true,
			new PK_EncryptorFilter(rng, encryptor,
				new StringSink(tmp)
			)
		);
		skey = tmp;
	}
	catch (CryptoPP::Exception& e)
	{
		std::cerr << "Caught Exception..." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void key_iv_decode_rsa(std::string &skey, std::string &siv, PrivateKey& private_Key) {
	try
	{
		AutoSeededRandomPool rng;
		RSAES_OAEP_SHA_Decryptor decryptor(private_Key);
		std::string tmp1;
		std::string tmp2;
		
		StringSource(skey, true,
			new PK_DecryptorFilter(rng, decryptor,
				new StringSink(tmp1)
			)
		);
		skey = tmp1;
		
		StringSource(siv, true,
			new PK_DecryptorFilter(rng, decryptor,
				new StringSink(tmp2)
			)
		);
		siv = tmp2;
	}
	catch (CryptoPP::Exception& e)
	{
		std::cerr << "Caught Exception..." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void key_decode_rsa(std::string &skey, PrivateKey& private_Key) {
	try
	{
		AutoSeededRandomPool rng;
		RSAES_OAEP_SHA_Decryptor decryptor(private_Key);
		std::string tmp;

		StringSource(skey, true,
			new PK_DecryptorFilter(rng, decryptor,
				new StringSink(tmp)
			)
		);
		skey = tmp;
	}
	catch (CryptoPP::Exception& e)
	{
		std::cerr << "Caught Exception..." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}


void key_iv_encode_rsa(std::string &skey, std::string &siv) {
	RSA::PublicKey  public_Key;
	GetPUB(public_Key);
	key_iv_encode_rsa(skey, siv, public_Key);
}
void key_encode_rsa(std::string &skey) {
	RSA::PublicKey  public_Key;
	GetPUB(public_Key);
	key_encode_rsa(skey, public_Key);
}

void key_iv_decode_rsa(std::string &skey, std::string &siv,const std::string private_path = "rsa-base64-private.key") {
	RSA::PrivateKey private_Key;
	GetPRIVATE(private_Key, private_path);
	key_iv_decode_rsa(skey, siv, private_Key);
}

void key_decode_rsa(std::string &skey, const std::string private_path = "rsa-base64-private.key") {
	RSA::PrivateKey private_Key;
	GetPRIVATE(private_Key, private_path);
	key_decode_rsa(skey, private_Key);
}