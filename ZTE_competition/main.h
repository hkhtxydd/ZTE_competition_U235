#pragma once
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#define inputDataFile ("../testData/in.txt")

#define outputDataFile ("../testData/myout.txt")

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

ifstream inFile;
ofstream outFile;

#define myStreamOut outFile


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

struct st_compareBits
{
	UINT32 a;
	UINT32 b;
	UINT32 c;
	UINT8 d;
};

struct st_rule
{
	UINT32 destination_ip;
	UINT32 source_ip;
	UINT16 destination_port;
	UINT16 source_port;
	UINT8 protocol;

	UINT32 destination_ip_mask;
	UINT32 source_ip_mask;
	UINT32 type;

};

void str2int_IpAndMask(
	const string& s,
	UINT32 & ip,
	UINT32 & ip_mask) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");
	vector<string> tmp_2;
	SplitString(tmp_1[0], tmp_2, ".");
	ip = (((UINT32)(stoi(tmp_2[0]))) << 24) + (((UINT32)(stoi(tmp_2[1]))) << 16) + (((UINT32)(stoi(tmp_2[2]))) << 8) + (UINT32)(stoi(tmp_2[3]));

	int source_MASK = INT_MAX;
	ip_mask = source_MASK << (32 - stoi(tmp_1[1]));

}

void str2int_protocol(
	const string& s,
	UINT8 & protocol) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");

	vector<string> tmp_2;
	SplitString(tmp_1[0], tmp_2, "x");
	protocol = (UINT8)stoi(tmp_2[1], nullptr,16);
}

void rule2dataAndMask(st_rule * oneRule) {

	myStreamOut << "data:";
	myStreamOut << "0x" << hex << oneRule->destination_ip << " ";
	myStreamOut << "0x" << hex << oneRule->source_ip << " ";
	myStreamOut << "0x" << hex << oneRule->destination_port << " ";
	myStreamOut << "0x" << hex << oneRule->source_port << " ";
	myStreamOut << "0x" << hex << (UINT32)(oneRule->protocol) << " ";
	myStreamOut << dec << oneRule->type << endl;

	myStreamOut << "mask:";
	myStreamOut << "0x" << hex << oneRule->destination_ip_mask << " ";
	myStreamOut << "0x" << hex << oneRule->source_ip_mask << " ";
	myStreamOut << "0xffff 0xffff 0xff" << endl;
}

struct st_message
{
	UINT32 destination_ip;
	UINT32 source_ip;
	UINT16 destination_port;
	UINT16 source_port;
	UINT8 protocol;
};

void str2ip(
	const string& s,
	UINT32 & ip) {

	vector<string> tmp_1;
	SplitString(s, tmp_1, ".");
	ip = (((UINT32)(stoi(tmp_1[0]))) << 24) + (((UINT32)(stoi(tmp_1[1]))) << 16) + (((UINT32)(stoi(tmp_1[2]))) << 8) + (UINT32)(stoi(tmp_1[3]));

}