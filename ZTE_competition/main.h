#pragma once
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#define inputDataFile ("C:/Users/LJY/source/repos/ZTE_competition/in.txt")

#define outputDataFile ("C:/Users/LJY/source/repos/ZTE_competition/out.txt")

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

void SplitString(const string& s, vector<string>& v, const string& c)
{

	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

struct st_rule
{
	UINT32 destination_ip;
	UINT32 destination_ip_mask;

	UINT32 source_ip;
	UINT32 source_ip_mask;
	
	UINT16 destination_port;
	UINT16 source_port;

	UINT8 protocol;
};

void str2int_IpAndMask(
	const string& s,
	UINT32 & ip,
	UINT32 & ip_mask) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");
	vector<string> tmp_2;
	SplitString(tmp_1[0], tmp_2, ".");
	ip = (UINT32)stoi(tmp_2[0])<<24 + (UINT32)stoi(tmp_2[1]) << 16 + (UINT32)stoi(tmp_2[2]) << 8 + (UINT32)stoi(tmp_2[3]) << 0;

	int source_MASK = INT_MAX;
	ip_mask = source_MASK << (32 - stoi(tmp_1[1]));

}