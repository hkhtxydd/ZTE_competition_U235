#include "main.h"

int main()
{
	int i, j, k;
	
	inFile.open( inputDataFile );
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

	vector<st_rule * > rules;

	for (i = 0; i < rule_number; i++) {

		UINT32 destination_ip;
		UINT32 destination_ip_mask;
		UINT32 source_ip;
		UINT32 source_ip_mask;
		UINT16 destination_port;
		UINT16 source_port;
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
			int destination_port_begin = stoi(destination_port_split[0]);
			int destination_port_end = stoi(destination_port_split[1]);
			for (j = destination_port_begin; j <= destination_port_end; j++) {
				vector<string> source_port_split;
				SplitString(oneRule_split[3], source_port_split, ":");
				int source_port_begin = stoi(source_port_split[0]);
				int source_port_end = stoi(source_port_split[1]);
				for (k = source_port_begin; k <= source_port_end; k++) {
					st_rule * oneRule=new st_rule;
					oneRule->destination_ip = destination_ip;
					oneRule->destination_ip_mask = destination_ip_mask;
					oneRule->destination_port = j;

					oneRule->source_ip = source_ip;
					oneRule->source_ip_mask = source_ip_mask;
					oneRule->source_port = k;

					oneRule->protocol = protocol;

					oneRule->type = type;
					rules.push_back(oneRule);

					rule2dataAndMask(*(rules.end() - 1));
				}
			}
		}
	}


	vector<st_message * > messages;

	vector<string> oneMessage_split;
	for (i = 0; i < message_number; i++) {
		SplitString(message_str[i], oneMessage_split, " ");
		st_message * oneMessage = new st_message;
		str2ip(oneMessage_split[0], oneMessage->destination_ip);
		str2ip(oneMessage_split[1], oneMessage->source_ip);
		oneMessage->destination_port = stoi(oneMessage_split[2]);
		oneMessage->source_port = stoi(oneMessage_split[3]);
		oneMessage->protocol = stoi(oneMessage_split[4]);
		messages.push_back(oneMessage);
	}

	//匹配报文

	for (i = 0; i < message_number; i++){
		st_message * oneMessage = messages[i];
		for (j = 0; j < rule_number; j++) {
			st_rule * oneRule = rules[j];
			st_message * tmp = (*oneMessage) & (*((st_message *)(oneRule)));
		}

	}


	inFile.close();
	outFile.close();
	system("pause");
	return 0;
}