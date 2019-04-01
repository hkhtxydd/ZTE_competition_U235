#include "main.h"


using namespace std;






void str2int_Port(
	const string& s,
	unsigned int & Port_begin,
	unsigned int & Port_end) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, ":");
	Port_begin = (unsigned int)stoi(tmp_1[0]);
	Port_end = (unsigned int)stoi(tmp_1[1]);
}

void str2int_protocol(
	const string& s,
	unsigned int & protocol,
	unsigned int & protocol_mask) {
	vector<string> tmp_1;
	SplitString(s, tmp_1, "/");
	protocol = (unsigned int)stoi(tmp_1[0]);
	protocol_mask = (unsigned int)stoi(tmp_1[1]);
}


void rule2dataAndMask(st_rule oneRule, string& s){
	int i, j, k;
	for (i = oneRule.destination_port_begin; i <= oneRule.destination_port_end; i++) {
		for (j = oneRule.source_port_begin; j <= oneRule.source_port_end; j++)
		{
			cout << "data:";
			cout << "0x";
			cout << hex << oneRule.destination_ip_0 * 256 * 256 * 256+ oneRule.destination_ip_1 * 256 * 256+ oneRule.destination_ip_2 * 256+ oneRule.destination_ip_3;
			cout << " ";

			cout << "0x";
			cout << hex << oneRule.source_ip_0 * 256 * 256 * 256 + oneRule.source_ip_1 * 256 * 256 + oneRule.source_ip_2 * 256 + oneRule.source_ip_3;
			cout << " ";

			cout << "0x";
			cout << hex << i;
			cout << " ";

			cout << "0x";
			cout << hex << j;
			cout << " ";

			cout <<dec<< oneRule.protocol_type;
			cout << endl;


			cout << "mask:";
			cout << "0x";
			int destination_MASK = INT_MAX;
			destination_MASK = destination_MASK << (32 - oneRule.destination_ip_mask);
			cout << hex << destination_MASK;
			cout << " ";

			cout << "0x";
			int source_MASK = INT_MAX;
			source_MASK = source_MASK << (32 - oneRule.destination_ip_mask);
			cout << hex << source_MASK;
			cout << " ";

			cout << "0xffff 0xffff 0xff";
			cout << endl;
			
			

		}
	}


}

int main()
{
	int i, j, k;
	ifstream inFile;
	inFile.open( inputDataFile );

	ofstream outFile;
	outFile.open(outputDataFile);

	vector<string> rule_str;
	string rule_number_str;
	getline(inFile, rule_number_str);
	int rule_number = stoi(rule_number_str);

	for (auto i = 0; i < rule_number; i++) {
		string one_rule;
		getline(inFile, one_rule);
		rule_str.push_back(one_rule);
	}

	string message_number_str;
	vector<string> message_str;
	getline(inFile, message_number_str);
	int message_number = stoi(message_number_str);

	for (i = 0; i < message_number; i++) {
		string one_message;
		getline(inFile, one_message);
		message_str.push_back(one_message);
	}

	vector<st_rule> rules;

	for (i = 0; i < rule_number; i++) {

		st_rule oneRule;

		vector<string> oneRule_split;
		SplitString(rule_str[i], oneRule_split, " ");
		{
			str2int_IpAndMask(
				oneRule_split[0],
				oneRule.destination_ip_0,
				oneRule.destination_ip_1,
				oneRule.destination_ip_2,
				oneRule.destination_ip_3,
				oneRule.destination_ip_mask
				);
			str2int_IpAndMask(
				oneRule_split[1],
				oneRule.source_ip_0,
				oneRule.source_ip_1,
				oneRule.source_ip_2,
				oneRule.source_ip_3,
				oneRule.source_ip_mask
				);
			str2int_Port(
				oneRule_split[2],
				oneRule.destination_port_begin,
				oneRule.destination_port_end
			);
			str2int_Port(
				oneRule_split[3],
				oneRule.source_port_begin,
				oneRule.source_port_end
			);
			str2int_protocol(
				oneRule_split[4],
				oneRule.protocol,
				oneRule.protocol_mask
			);
			oneRule.protocol_type = stoi(oneRule_split[5]);
		}
		rules.push_back(oneRule);

		string sstr;
		rule2dataAndMask(oneRule, sstr);
		system("pause");


	}



	system("pause");
	return 0;
}