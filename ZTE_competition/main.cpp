/*
MEM_CHECK 是否检测内存泄漏，仅用于vs
USING_FILE 是否使用文件替代标准输入与标准输出
MEM_CLEAR 是否在程序结束时清理内存（规则）
*/

/*
个人认为题目的核心在于逻辑函数化简。假如端口为10：50，那么需要对逻辑函数  F=m10+m11+m12+...+m49+m50  进行化简。
通过卡诺图与QM算法可以求得逻辑函数的最简形式，但是np的。查找资料得知，实际应用中一般使用Espresso heuristic logic minimizer
对逻辑函数化简。
本程序基于“连续”的特性，对其进行化简。并与Espresso heuristic logic minimizer求得的结果进行比较。实验得知，
本程序的方法非最优解，但与最优解接近。

*/

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

//定义无符号整型
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

//定义无符号整型的最大值
#define UINT32_MAX ((UINT32)0xffffffff)
#define UINT16_MAX ((UINT16)0xffff)
#define UINT8_MAX  ((UINT8)0xff)

//将前缀匹配位数的形式转化为掩码，减小内存占用
#define prefix2Mask(prefix)  (UINT32_MAX << (32 - (prefix)))

/*
使用D_port_num与S_port_num控制data/mask的格式，当D_port_num=i，S_port_num=j时，data/mask格式如下：
目的IP | 源IP | 目的端口_1 | 目的端口_2 | ...... | 目的端口_i | 源端口_1 | 源端口_2 | ...... | 源端口_j | 协议
(假如i=4，目的端口为1：3，那么目的端口_1为1，目的端口_2为2，目的端口_3为3，目的端口_4也为3以进行填充。)
其中目的端口_1至目的端口_i被合并输出，源端口_1至源端口_j被合并输出，即输出的第一行为：
32 32 16*i 16*j 8 。
在匹配时，将报文转换为：
目的IP | 源IP | 目的端口 | 目的端口 | ...... | 目的端口 | 源端口 | 源端口 | ...... | 源端口 | 协议
假如报文的目的端口为2，那么就可以与规则的目的端口_2匹配，则认为报文与规则的目的端口匹配了。源端口的匹配方法相同。
*/
#define D_port_num  4
#define S_port_num  5

#define T_match(A,B,C)   ((((A)^(B))&(C))==0)
// T内存匹配模式
// ((Input xor data) and mask)

#if USING_FILE
ifstream inFile;
ofstream outFile;
#endif

//data_mask对
struct st_data_mask
{
	UINT16 data;
	UINT16 mask;
	st_data_mask(UINT16 in_data, UINT16 in_mask) {
		data = in_data;
		mask = in_mask;
	}
	st_data_mask() {
	}
};

//报文
struct st_message
{
	UINT32 destination_ip;
	UINT32 source_ip;
	UINT16 destination_port;
	UINT16 source_port;
	UINT8 protocol;
};


//存储规则的结构体
struct st_rule
{
	UINT32 destination_ip;
	UINT32 source_ip;

	//保存前缀匹配的长度，减小内存占用·
	UINT8 destination_ip_prefixLength;
	UINT8 source_ip_prefixLength;

	vector<st_data_mask*> destination_port;
	vector<st_data_mask*> source_port;

	UINT8 protocol;

	UINT16 type;

};

//找到最右侧的1
UINT8 findOne(UINT16 num) {
	for (UINT8 i = 0; i < 16; i++)
	{
		if ((num&(0x0001 << i)) != 0)
			return i;
	}
	return 0;
}

//核心，逻辑函数化简。
void Simplify(vector<st_data_mask*> & result, UINT16 m_begin, UINT16 m_end) {
	bool T = true;
	UINT16 port;
	UINT16 mask;
	UINT16 shift;
	UINT16 currentMax;
	UINT16 currentMin;
	if (m_begin == 0) {
		m_begin = 1;
		st_data_mask* pm = New st_data_mask(0, UINT16_MAX);
		result.push_back(pm);
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
			st_data_mask* pm = New st_data_mask(port, mask);
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



//工具，字符串分割
void SplitString(const string& s, vector<string>& v, const string& c) {
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



//字符串转ip
void str2ip(const string& s, UINT32 & ip) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, ".");
	ip = (((UINT32)(stoi(tmp_1[0]))) << 24) + (((UINT32)(stoi(tmp_1[1]))) << 16) +
		(((UINT32)(stoi(tmp_1[2]))) << 8) + (UINT32)(stoi(tmp_1[3]));
}

//字符串转ip与掩码
void str2int_IpAndMask(const string& s, UINT32 & ip, UINT8 & prefixLength) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");
	str2ip(tmp_1[0], ip);
	prefixLength = stoi(tmp_1[1]);
}

//字符串转协议
void str2int_protocol(const string& s, UINT8 & protocol) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");

	vector<string> tmp_2;
	SplitString(tmp_1[0], tmp_2, "x");
	protocol = (UINT8)stoi(tmp_2[1], nullptr, 16);
}

//格式化输出规则
void rule2dataAndMask(st_rule * oneRule) {
	int i = 0;
	myStreamOut << "data:";
	myStreamOut << "0x" << hex << oneRule->destination_ip << " ";
	myStreamOut << "0x" << hex << oneRule->source_ip << " ";

	myStreamOut << "0x";
	for (i = 0; i < D_port_num; i++) {
		myStreamOut << hex << oneRule->destination_port[i]->data;
	}
	myStreamOut << " ";

	myStreamOut << "0x";
	for (i = 0; i < S_port_num; i++) {
		myStreamOut <<hex << oneRule->source_port[i]->data;
	}
	myStreamOut << " ";

	myStreamOut << "0x" << hex << (UINT32)(oneRule->protocol) << " ";
	myStreamOut << dec << oneRule->type << endl;

	myStreamOut << "mask:";
	myStreamOut << "0x" << hex << (UINT32_MAX << (32 - (oneRule->destination_ip_prefixLength))) << " ";
	myStreamOut << "0x" << hex << (UINT32_MAX << (32 - (oneRule->source_ip_prefixLength))) << " ";

	//合并输出端口
	myStreamOut << "0x";
	for (i = 0; i < D_port_num; i++) {
		myStreamOut << hex << oneRule->destination_port[i]->mask;
	}
	myStreamOut << " ";

	myStreamOut << "0x";
	for (i = 0; i < S_port_num; i++) {
		myStreamOut << hex << oneRule->source_port[i]->mask;
	}
	myStreamOut << " ";

	myStreamOut << "0xff" << endl;
}



//字符串转规则
void str2rules(vector<st_rule * > & rules, const string rule_str) {
	UINT32 i, j, k, l;
	UINT32 destination_ip;
	UINT8 destination_ip_prefixLength;
	UINT32 source_ip;
	UINT8 source_ip_prefixLength;
	UINT16 destination_port_begin;
	UINT16 destination_port_end;
	UINT16 source_port_begin;
	UINT16 source_port_end;
	UINT8 protocol;
	UINT16 type;

	vector<string> oneRule_split;
	SplitString(rule_str, oneRule_split, " ");
	str2int_IpAndMask(oneRule_split[0], destination_ip, destination_ip_prefixLength);
	str2int_IpAndMask(oneRule_split[1], source_ip, source_ip_prefixLength);
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

	vector<st_data_mask*>  destination_port_mask_vector;
	vector<st_data_mask*>  source_port_mask_vector;

	//若开始端口等于结束端口，则直接将其假如用于匹配的端口
	if (destination_port_begin == destination_port_end) {
		st_data_mask* port_mask = New st_data_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->data = destination_port_begin;
		destination_port_mask_vector.push_back(port_mask);
	}
	else {
		//调用逻辑函数简化函数
		Simplify(destination_port_mask_vector, destination_port_begin, destination_port_end);
	}

	if (source_port_begin == source_port_end) {
		st_data_mask* port_mask = New st_data_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->data = source_port_begin;
		source_port_mask_vector.push_back(port_mask);
	}
	else {
		Simplify(source_port_mask_vector, source_port_begin, source_port_end);
	}

	while (destination_port_mask_vector.size() % D_port_num != 0) {
		st_data_mask* port_mask = New st_data_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->data = destination_port_begin;
		destination_port_mask_vector.push_back(port_mask);
	}

	while (source_port_mask_vector.size() % S_port_num != 0) {
		st_data_mask* port_mask = New st_data_mask;
		port_mask->mask = UINT16_MAX;
		port_mask->data = source_port_begin;
		source_port_mask_vector.push_back(port_mask);
	}
	for (i = 0; i < destination_port_mask_vector.size() / D_port_num; i++) {
		for (j = 0; j < source_port_mask_vector.size() / S_port_num; j++) {
			st_rule * oneRule = New st_rule;
			oneRule->destination_ip = destination_ip;
			oneRule->destination_ip_prefixLength = destination_ip_prefixLength;
			oneRule->source_ip = source_ip;
			oneRule->source_ip_prefixLength = source_ip_prefixLength;
			for (k = 0; k < D_port_num; k++) {
				UINT32 A = i * D_port_num + k;
				oneRule->destination_port.push_back(New st_data_mask(destination_port_mask_vector[A]->data, destination_port_mask_vector[A]->mask));
				for (l = 0; l < S_port_num; l++) {
					UINT32 B = j * S_port_num + l;
					oneRule->source_port.push_back(New st_data_mask(source_port_mask_vector[B]->data, source_port_mask_vector[B]->mask));
				}
			}
			oneRule->protocol = protocol;
			oneRule->type = type;
			rules.push_back(oneRule);
		}
	}

	//清理内存
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

//比较报文与规则
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
		bool result_destination_ip = T_match(oneMessage_destination_ip, oneRule->destination_ip, prefix2Mask(oneRule->destination_ip_prefixLength));
		bool result_source_ip = T_match(oneMessage_source_ip, oneRule->source_ip, prefix2Mask(oneRule->source_ip_prefixLength));

		bool result_destination_port = false;
		bool result_source_port = false;

		//多个端口中，只要有一个匹配，即可认为匹配成功
		for (k = 0; k < D_port_num; k++) {
			result_destination_port = result_destination_port || T_match(oneMessage_destination_port, oneRule->destination_port[k]->data, oneRule->destination_port[k]->mask);
		}
		for (k = 0; k < S_port_num; k++) {
			result_source_port = result_source_port || T_match(oneMessage_source_port, oneRule->source_port[k]->data, oneRule->source_port[k]->mask);
		}

		bool result_protocol = T_match(oneMessage_protocol, oneRule->protocol, UINT8_MAX);

		if (result_destination_ip && result_source_ip && result_destination_port && result_source_port && result_protocol) {
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
#if MEM_CHECK
	//_CrtSetBreakAlloc(172);
#endif
	//使用程序块，减少内存泄漏检测的误报
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
		myStreamOut << "32 32 64 80 8 " << rules.size() << endl;
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