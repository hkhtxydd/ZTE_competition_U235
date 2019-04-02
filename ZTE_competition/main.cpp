#include "main.h"


#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#define inputDataFile ("../testData/int_1.txt")

#define outputDataFile ("../testData/myout.txt")

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

ifstream inFile;
ofstream outFile;

#define myStreamIn inFile
#define myStreamOut outFile

void SplitString(const string& s, vector<string>& v, const string& c)
{
	v.clear();
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
	UINT32 source_ip;

	UINT16 destination_port;
	UINT16 source_port;
	UINT16 destination_port_mask;
	UINT16 source_port_mask;

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
	protocol = (UINT8)stoi(tmp_2[1], nullptr, 16);
}

void rule2dataAndMask(st_rule * oneRule) {
	int i, j;

			myStreamOut << "data:";
			myStreamOut << "0x" << hex << oneRule->destination_ip << " ";
			myStreamOut << "0x" << hex << oneRule->source_ip << " ";
			myStreamOut << "0x" << hex << oneRule->destination_port<< " ";
			myStreamOut << "0x" << hex << oneRule->source_port << " ";
			myStreamOut << "0x" << hex << (UINT32)(oneRule->protocol) << " ";
			myStreamOut << dec << oneRule->type << endl;

			myStreamOut << "mask:";
			myStreamOut << "0x" << hex << oneRule->destination_ip_mask << " ";
			myStreamOut << "0x" << hex << oneRule->source_ip_mask << " ";
			myStreamOut << "0x" << hex << oneRule->destination_port_mask << " ";
			myStreamOut << "0x" << hex << oneRule->source_port_mask << " ";
			myStreamOut << endl;

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

int main()
{
	int i, j, k;

	inFile.open( inputDataFile );
	outFile.open(outputDataFile);



	vector<string> rule_str;
	string rule_number_str;
	getline(myStreamIn, rule_number_str);
	int rule_number = stoi(rule_number_str);

	for (auto i = 0; i < rule_number; i++) {
		string one_rule;
		getline(myStreamIn, one_rule);
		rule_str.push_back(one_rule);
	}

	string message_number_str;
	vector<string> message_str;
	getline(myStreamIn, message_number_str);
	int message_number = stoi(message_number_str);

	for (i = 0; i < message_number; i++) {
		string one_message;
		getline(myStreamIn, one_message);
		message_str.push_back(one_message);
	}

	vector<st_rule * > rules;

	for (i = 0; i < rule_number; i++) {
		cout << "RULE:\t" << i << endl;
		UINT32 destination_ip;
		UINT32 destination_ip_mask;
		UINT32 source_ip;
		UINT32 source_ip_mask;
		UINT16 destination_port_begin;
		UINT16 destination_port_end;
		UINT16 source_port_begin;
		UINT16 source_port_end;
		UINT8 protocol;
		UINT32 type;

		vector<string> oneRule_split;
		SplitString(rule_str[i], oneRule_split, " ");
		{
			str2int_IpAndMask(
				oneRule_split[0],
				destination_ip,
				destination_ip_mask
			);
			str2int_IpAndMask(
				oneRule_split[1],
				source_ip,
				source_ip_mask
			);

			str2int_protocol(oneRule_split[4], protocol);

			type = (UINT32)stoi(oneRule_split[5]);

			vector<string> destination_port_split;
			SplitString(oneRule_split[2], destination_port_split, ":");
			destination_port_begin = stoi(destination_port_split[0]);
			destination_port_end = stoi(destination_port_split[1]);

			vector<string> source_port_split;
			SplitString(oneRule_split[3], source_port_split, ":");
			source_port_begin = stoi(source_port_split[0]);
			source_port_end = stoi(source_port_split[1]);

			vector<st_port_mask*>  destination_port_mask_vector;
			vector<st_port_mask*>  source_port_mask_vector;

			if (destination_port_begin == destination_port_end){
				st_port_mask* port_mask = new st_port_mask;
				port_mask->mask = 0xffff;
				port_mask->port = destination_port_begin;
				destination_port_mask_vector.push_back(port_mask);
			}
			else{
				m_main(destination_port_begin, destination_port_end, destination_port_mask_vector);
			}
			
			if (source_port_begin == source_port_end) {
				st_port_mask* port_mask = new st_port_mask;
				port_mask->mask = 0xffff;
				port_mask->port = source_port_begin;
				source_port_mask_vector.push_back(port_mask);
			}
			else {
				m_main(source_port_begin, source_port_end, source_port_mask_vector);
			}

			for (j = 0; j < destination_port_mask_vector.size(); j++) {
				for (k = 0; k < source_port_mask_vector.size(); k++) {

					st_rule * oneRule = new st_rule;
					oneRule->destination_ip = destination_ip;
					oneRule->destination_ip_mask = destination_ip_mask;
					oneRule->source_ip = source_ip;
					oneRule->source_ip_mask = source_ip_mask;

					oneRule->destination_port = destination_port_mask_vector[j]->port;
					oneRule->destination_port_mask = destination_port_mask_vector[j]->mask;
					oneRule->source_port = source_port_mask_vector[k]->port;
					oneRule->source_port_mask = source_port_mask_vector[k]->mask;

					oneRule->protocol = protocol;

					oneRule->type = type;
					rules.push_back(oneRule);
				}
			}
		}
	}
	cout << "DONE" << endl;

	vector<st_message * > messages;

	vector<string> oneMessage_split;
	for (i = 0; i < message_number; i++) {

		auto tmp = message_str[i];
		SplitString(tmp, oneMessage_split, " ");
		st_message * oneMessage = new st_message;
		str2ip(oneMessage_split[0], oneMessage->destination_ip);
		str2ip(oneMessage_split[1], oneMessage->source_ip);
		oneMessage->destination_port = stoi(oneMessage_split[2]);
		oneMessage->source_port = stoi(oneMessage_split[3]);
		oneMessage->protocol = stoi(oneMessage_split[4]);
		messages.push_back(oneMessage);
	}

	//输出规则
	myStreamOut << "32 32 16 16 8 8" << endl;
	for (i = 0; i < rules.size(); i++) {
		rule2dataAndMask(rules[i]);
	}
	//匹配报文

	for (i = 0; i < message_number; i++) {
		st_message * oneMessage = messages[i];
		for (j = 0; j < rule_number; j++) {
			st_rule * oneRule = rules[j];
			st_message result;

			result.destination_ip = ((oneMessage->destination_ip) ^ (oneRule->destination_ip))&(oneRule->destination_ip_mask);
			result.source_ip = ((oneMessage->source_ip) ^ (oneRule->source_ip))&(oneRule->source_ip_mask);

			result.destination_port= ((oneMessage->destination_port) ^ (oneRule->destination_port))&(oneRule->destination_port_mask);
			result.source_port = ((oneMessage->source_port) ^ (oneRule->source_port))&(oneRule->source_port_mask);

			result.protocol = oneMessage->protocol ^ oneRule->protocol;

			if(result.destination_ip+ result.source_ip+ result.destination_port+ result.source_port + result.protocol==0) {
				j = rule_number + 1;
				myStreamOut << dec << oneRule->type << endl;
			}
		}
		if (j == rule_number) {
			myStreamOut << 0 << endl;
		}

	}


	inFile.close();
	outFile.close();
	//system("pause");
	return 0;
}