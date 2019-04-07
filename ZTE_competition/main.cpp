#define MEM_CHECK false
#define USING_FILE false
#define MEM_CLEAR false

#if MEM_CHECK
#include <crtdbg.h> 
#define New   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define CRTDBG_MAP_ALLOC
#else
#define New   new
#endif

#if USING_FILE
#define inputDataFile ("../testData/big.txt")
#define outputDataFile ("../testData/myout.txt")
#define myStreamIn inFile
#define myStreamOut outFile
#else
#define myStreamIn cin
#define myStreamOut cout
#endif


#include <stdlib.h>  
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ostream>
#include <istream>

using namespace std;

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

#define UINT32_MAX ((UINT32)0xffffffff)
#define UINT16_MAX ((UINT16)0xffff)
#define UINT8_MAX  ((UINT8)0xff)

#if USING_FILE
ifstream inFile;
ofstream outFile;
#endif

struct st_port_mask
{
	UINT16 port;
	UINT16 mask;
};

struct st_rule
{
	UINT32 destination_ip;
	UINT32 source_ip;

	UINT8 destination_ip_mask;
	UINT8 source_ip_mask;

	UINT16 destination_port;
	UINT16 source_port;
	UINT16 destination_port_mask;
	UINT16 source_port_mask;

	UINT8 protocol;

	UINT16 type;

};

UINT8 findOne(UINT16 num) {
	for (UINT8 i = 0; i < 16; i++)
	{
		if ((num&(0x0001 << i)) != 0)
			return i;
	}
	return 0;
}

void Simplify(vector<st_port_mask*> & result, UINT16 m_begin, UINT16 m_end) {
	bool T = true;
	UINT16 port;
	UINT16 mask;
	UINT16 shift;
	UINT16 currentMax;
	UINT16 currentMin;
	if (m_begin == 0) {
		m_begin = 1;
		st_port_mask* pm = New st_port_mask;
		pm->port = 0;
		pm->mask = UINT16_MAX;
	}

	port = m_begin;
	mask = UINT16_MAX;
	shift = 0x0001;

	while ((port&(~mask)) == 0) {
		mask = mask << 1;
	}
	mask = (mask >> 1) + 0x8000;

	currentMax = port | (~mask);
	currentMin = port & mask;

	while (T) {
		T = false;
		while ((currentMax <= m_end) && (currentMin >= m_begin)) {
			st_port_mask* pm = New st_port_mask;
			pm->port = port;
			pm->mask = mask;
			result.push_back(pm);
			port = currentMax + 1;
			mask = 0xffff << findOne(port);
			T = true;

			currentMax = port | (~mask);
			currentMin = port & mask;
		}
		while (currentMax > m_end) {
			port = currentMin & mask;
			mask = (mask >> 1) + 0x8000;
			currentMax = port | (~mask);
			currentMin = port & mask;
			if (currentMax == currentMin)
				break;
		}
	}
}




void SplitString(const string& s, vector<string>& v, const string& c){
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




void str2ip(const string& s, UINT32 & ip) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, ".");
	ip = (((UINT32)(stoi(tmp_1[0]))) << 24) + (((UINT32)(stoi(tmp_1[1]))) << 16) +
		(((UINT32)(stoi(tmp_1[2]))) << 8) + (UINT32)(stoi(tmp_1[3]));
}

void str2int_IpAndMask(const string& s,UINT32 & ip,UINT8 & ip_mask) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");
	str2ip(tmp_1[0], ip);
	ip_mask = stoi(tmp_1[1]);
}

void str2int_protocol(const string& s,UINT8 & protocol) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");

	vector<string> tmp_2;
	SplitString(tmp_1[0], tmp_2, "x");
	protocol = (UINT8)stoi(tmp_2[1], nullptr, 16);
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
	myStreamOut << "0x" << hex << ( UINT32_MAX << (32 - (oneRule->destination_ip_mask))) << " ";
	myStreamOut << "0x" << hex << ( UINT32_MAX << (32 - (oneRule->source_ip_mask))) << " ";
	myStreamOut << "0x" << hex << oneRule->destination_port_mask << " ";
	myStreamOut << "0x" << hex << oneRule->source_port_mask << " ";
	myStreamOut << "0xff" << endl;
}

struct st_message
{
	UINT32 destination_ip;
	UINT32 source_ip;
	UINT16 destination_port;
	UINT16 source_port;
	UINT8 protocol;
};


void str2rules(vector<st_rule * > & rules, const string rule_str) {
	UINT32 i, j, k;
	UINT32 destination_ip;
	UINT8 destination_ip_mask;
	UINT32 source_ip;
	UINT8 source_ip_mask;
	UINT16 destination_port_begin;
	UINT16 destination_port_end;
	UINT16 source_port_begin;
	UINT16 source_port_end;
	UINT8 protocol;
	UINT16 type;

	vector<string> oneRule_split;
	SplitString(rule_str, oneRule_split, " ");
	str2int_IpAndMask(oneRule_split[0], destination_ip, destination_ip_mask);
	str2int_IpAndMask(oneRule_split[1], source_ip, source_ip_mask);
	str2int_protocol(oneRule_split[4], protocol);

	type = (UINT16)stoi(oneRule_split[5]);

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

	if (destination_port_begin == destination_port_end) {
		st_port_mask* port_mask = New st_port_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->port = destination_port_begin;
		destination_port_mask_vector.push_back(port_mask);
	}
	else {
		Simplify(destination_port_mask_vector, destination_port_begin, destination_port_end);
	}

	if (source_port_begin == source_port_end) {
		st_port_mask* port_mask = New st_port_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->port = source_port_begin;
		source_port_mask_vector.push_back(port_mask);
	}
	else {
		Simplify(source_port_mask_vector, source_port_begin, source_port_end);
	}

	for (j = 0; j < destination_port_mask_vector.size(); j++) {
		for (k = 0; k < source_port_mask_vector.size(); k++) {
			st_rule * oneRule = New st_rule;
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
	for (auto & x : destination_port_mask_vector) {
		delete x;
		x = nullptr;
	}
	destination_port_mask_vector.clear();

	for (auto & x : source_port_mask_vector) {
		delete x;
		x = nullptr;
	}
	source_port_mask_vector.clear();
}


void compareMessageAndRules(const ostream & output, const vector<st_rule * > rules, const string message_str) {
	UINT32 i, j, k;
	vector<string> oneMessage_split;
	SplitString(message_str, oneMessage_split, " ");

	UINT32  oneMessage_destination_ip;
	UINT32  oneMessage_source_ip;
	UINT32  oneMessage_destination_port;
	UINT16  oneMessage_source_port;
	UINT16  oneMessage_protocol;

	str2ip(oneMessage_split[0], oneMessage_destination_ip);
	str2ip(oneMessage_split[1], oneMessage_source_ip);
	oneMessage_destination_port = stoi(oneMessage_split[2]);
	oneMessage_source_port = stoi(oneMessage_split[3]);
	oneMessage_protocol = stoi(oneMessage_split[4]);

	for (j = 0; j < rules.size(); j++) {
		st_rule * oneRule = rules[j];
		st_message result;

		result.destination_ip = ((oneMessage_destination_ip) ^ (oneRule->destination_ip))& (UINT32_MAX << (32 - (oneRule->destination_ip_mask)));
		result.source_ip = ((oneMessage_source_ip) ^ (oneRule->source_ip)) & (UINT32_MAX << (32 - (oneRule->source_ip_mask)));

		result.destination_port = (((oneMessage_destination_port) ^ (oneRule->destination_port))) & (oneRule->destination_port_mask);
		result.source_port = (((oneMessage_source_port) ^ (oneRule->source_port)))&(oneRule->source_port_mask);

		result.protocol = (oneMessage_protocol) ^ (oneRule->protocol);

		if (result.destination_ip + result.source_ip + result.destination_port + result.source_port + result.protocol == 0) {
			j = rules.size() + 1;
			myStreamOut << dec << oneRule->type << endl;
		}
	}
	if (j == rules.size()) {
		myStreamOut << dec << 0 << endl;
	}

}

int main()
{
	//_CrtSetBreakAlloc(172);
	{
		UINT32 i = 0;

#if USING_FILE
		inFile.open(inputDataFile);
		outFile.open(outputDataFile);
#endif

		vector<st_rule * > rules;

		string rule_number_str;
		getline(myStreamIn, rule_number_str);
		UINT16 rule_number = stoi(rule_number_str);

		for (i = 0; i < rule_number; i++) {
			string rule_str;
			getline(myStreamIn, rule_str);
			str2rules(rules, rule_str);
		}

		//输出规则
		myStreamOut << "32 32 16 16 8 " << rules.size() << endl;
		for (i = 0; i < rules.size(); i++) {
			rule2dataAndMask(rules[i]);
		}

		vector<string> oneMessage_split;

		string message_number_str;
		vector<string> message_str;
		getline(myStreamIn, message_number_str);
		UINT16 message_number = stoi(message_number_str);

		for (i = 0; i < message_number; i++) {
			string message_str;
			getline(myStreamIn, message_str);
			compareMessageAndRules(myStreamOut, rules, message_str);
		}

#if MEM_CLEAR
		for (auto & x : rules) {
			delete x;
			x = nullptr;
		}
		rules.clear();
#endif

#if USING_FILE
		inFile.close();
		outFile.close();
#endif
	}

#if MEM_CHECK
	_CrtDumpMemoryLeaks();
#endif

	//system("pause");
	return 0;
}