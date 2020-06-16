#pragma once

#include<string>
#include<vector>
#include<iostream>
#include <fstream>

using namespace std;

vector<string> ReadPassWordFile()
{
	fstream op("UserPassword.txt");
	vector<string> data;
	while (!op.eof())
	{
		string tmp;
		getline(op, tmp);
		data.push_back(tmp);
	}

	op.close();
	return data;
}
void WritePasswordFile(vector<string> account)
{
	vector<string> temp = account;
	fstream out("UserPassword.txt", ios::out);
	for (int i = 0; i < temp.size(); i++)
	{
		if (temp[i][0] == '+')
		{
			temp[i][0] = '=';
		}
		if (i == temp.size() - 1)
		{
			out << temp[i];
		}
		else
		{
			out << temp[i] << endl;
		}
	}
	out.close();
}