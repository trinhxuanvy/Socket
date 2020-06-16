// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "ReadFile.h"
#include <string>
#include <vector>
#include <bitset>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

//==============================================================================

bool CheckLoginOrSignIn(string userLogin);
bool CheckUser(string username, vector<string> account, int& check);
bool CheckPassWord(string password, vector<string> account, int& check);
string getUsername(string userLogin);
string getUser(string userstring);
string getPassWord(string userstring);

//==============================================================================

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.

		// Khoi tao thu vien Socket
		if (AfxSocketInit() == FALSE)
		{
			cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}
		// Máy chủ
		CSocket Server;
		// Tao socket cho server, dang ky port la 1234, giao thuc TCP
		if (Server.Create(1234, SOCK_STREAM, NULL) == 0) //SOCK_STREAM or SOCK_DGRAM.
		{
			cout << "Server creation failed ???" << endl;
			cout << Server.GetLastError();
			return FALSE;
		}
		else
		{
			cout << "Create server successfully !!!" << endl;

			if (Server.Listen(1) == FALSE)
			{
				cout << "Can't listen to this port !!!" << endl;
				Server.Close();
				return FALSE;
			}
		}
		// Nhập số lượng Client
		int num_client;
		cout << "Input Number Client: ";
		cin >> num_client;
		cout << "Waiting for Client ..." << endl;
		// Tạo mảng chứa các Socket Client
		CSocket* Client = new CSocket[num_client];
		for (int i = 0; i < num_client; i++)
		{
			if (Server.Accept(Client[i]))
			{
				cout << "Client Connected: " << i + 1 << "/" << num_client << endl;
				vector<string> userpassword = ReadPassWordFile();
				do
				{
					// Nhận thông tin đăng nhập
					// Kích thước chuỗi nhận
					int usersize;
					Client[i].Receive(&usersize, sizeof(int), 0);
					// Chuỗi nhận
					char* user = new char[usersize + 1];
					Client[i].Receive(user, usersize, 0);
					user[usersize] = 0;
					// Gán cho userstring --> sử dụng biến này để thực hiện
					string userstring = user;
					// Xóa con trỏ đã cấp phát cho chuỗi nhận
					delete[] user;

					// 2 biến kiểm tra user và password có trùng hay không
					int checkuser = 0, checkpassword = 0;
					// Đọc file chứa thông tin tài khoản
					vector<string> readfile = ReadPassWordFile();

					// Lấy tên từ chuỗi đăng nhập client
					string _user = getUser(userstring);
					// Lấy mật khẩu từ chuỗi đăng nhập client
					string _pass = getPassWord(userstring);

					// Kiểm tra đăng nhập
					// Tài khoản tồn tại
					if (userstring[0] == '=')
					{
						// Nhập đúng user và password đã tồn tại
						if (CheckUser(_user, readfile, checkuser) == 1 && CheckPassWord(_pass, readfile, checkpassword) == 1 && checkuser == checkpassword)
						{
							// Xuất ra tên tài khoản đã nhập
							cout << _user << " Log in!" << endl;
							// Gửi thông tin đăng nhập thành công (1)
							Client[i].Send("1", 1);
							break;
						}
						// Nhập sai user hoặc password đã tồn tại
						else
						{
							// Gửi thông tin đăng nhập thất bại (0)
							Client[i].Send("0", 1);
						}
					}
					// Tài khoản chưa có và tạo tài khoản mới
					else if(userstring[0] == '+')
					{
						size_t newsize = userpassword.size();
						userpassword.push_back(userstring);
						if (CheckUser(_user, readfile, checkuser) == 0)
						{
							cout << _user << " Log in!" << endl;
							// Khởi tạo thành công và thông báo cho client
							Client[i].Send("3", 1);
							// Thêm vào file chứa thông tin tài khoản người dùng
							WritePasswordFile(userpassword);
							break;
						}
						else
						{
							// Khởi tạo thất bại và thông báo cho client vì đã trùng tài khoản
							Client[i].Send("2", 1);
						}
					}
				} while (1);
				/*int filenamesize(0);
				Client[i].Receive((char*)&filenamesize, sizeof(int), 0);
				char* filename = new char[filenamesize + 1];
				Client[i].Receive(filename, filenamesize, 0);
				filename[filenamesize] = '\0';
				string filenamestring = filename;
				delete[] filename;

				filenamestring = filenamestring.substr(1, filenamestring.size());

				ofstream outfile(filenamestring, ios::binary);
				while (1)
				{
					int buffsize(0);
					Client[i].Receive((char*)&buffsize, sizeof(int), 0);
					char* buff = new char[buffsize];
					Client[i].Receive(buff, buffsize, 0);
					if (strlen(buff) == 0)
					{
						break;
					}

					for (int i = 0; i < strlen(buff); i++)
					{
						cout << buff[i];
					}
					outfile << buff;
					delete[] buff;
				}*/
				// Báo cho Client biết là Client thứ mấy
				//Client[i].Send((char*)&i, sizeof(int), 0);
			}
			for (i = 0; i < num_client; i++)
			{
				Client[i].Close();
			}
			Server.Close();
		}

		return nRetCode;
	}
}

bool CheckLoginOrSignIn(string userLogin)
{
	if(userLogin[0] = '=')
	{
		return 1;
	}
	return 0;
}

string getUsername(string userLogin)
{
	string tmp = userLogin.substr(1, userLogin.size());
	for (int i = 0; i < tmp.size(); i++)
	{
		if (tmp[i] == '|')
		{
			tmp = tmp.substr(0, i);
		}
	}
	return tmp;
}

string getUser(string userstring)
{
	bool flag = 0;
	string merge = "";
	for (int i = 1; i < userstring.size(); i++)
	{
		if (userstring[i] == '|')
		{
			break;
		}
		merge += userstring[i];
	}
	return merge;
}

string getPassWord(string userstring)
{
	bool flag = 0;
	string merge = "";
	for (int i = 1; i < userstring.size(); i++)
	{
		if (userstring[i] == '|')
		{
			for (int j = i + 1; j < userstring.size(); j++)
			{
				merge += userstring[j];
			}
		}
		
	}
	return merge;
}

bool CheckUser(string username, vector<string> account, int& check)
{
	bool flag = 0;
	for (int i = 0; i < account.size(); i++)
	{
		string merge = "";
		for (int j = 1; j < account[i].size(); j++)
		{
			if (account[i][j] == '|')
			{
				break;
			}
			merge += account[i][j];
		}
		if (merge == username)
		{
			flag = 1;
			check = i;
			break;
		}
	}
	return flag;
}

bool CheckPassWord(string password, vector<string> account, int& check)
{
	bool flag = 0;
	for (int i = 0; i < account.size(); i++)
	{
		string merge = "";
		for (int j = 1; j < account[i].size(); j++)
		{
			if (account[i][j] == '|')
			{
				for (int l = j + 1; l < account[i].size(); l++)
				{
					merge += account[i][l];
				}
				if (merge == password)
				{
					flag = 1;
					check = i;
				}
				break;
			}
		}
		if (flag == 1)
			break;
	}
	return flag;
}