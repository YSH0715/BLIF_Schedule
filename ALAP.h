#pragma  once
#include<iostream>
#include<fstream>
#include<vector>
#include <regex>
#include<map>
#include<algorithm> 
#include "blif2verilog.h"
#include "ASAP.h"
using namespace std; 
void ALAP(vector<vector<string>>& output_ASAP, map<string, node>& m);
void printALAP(const vector<vector<string>>& output, Model model);
void printString(string stri);
void printGateALAP(Model model) {

	vector<vector<string>> inData;
	string FileName = "output.txt";
	input(inData, FileName);

	map<string, node> m;
	initData(m, inData);

	vector<vector<string>> output_ALAP;
	ALAP(output_ALAP, m);
	std::reverse(output_ALAP.begin(), output_ALAP.end());
	printALAP(output_ALAP, model);

	//system("PAUSE");
}


void ALAP(vector<vector<string>>& output_ALAP, map<string, node>& m) {
	while (!m.empty()) {//��ͼ�����нڵ㶼��ɾ��ʱ����
		vector<string> tmp;
		for (auto it = m.begin(); it != m.end(); ) {
			if (it->second.child.empty()) {//�ҵ�����û���ӽڵ�Ľڵ㣬�������һ��tmp������
				tmp.push_back(it->first);
				it = m.erase(it); // ɾ���ڵ�
			}
			else {
				++it;
			}
		}
		if (!tmp.empty()) {
			output_ALAP.push_back(tmp);
		}
		delete_(m, tmp);
	}
}


void printALAP(const vector<vector<string>>& output, Model model) {
	int cycleCount = output.size();
	cout << "Total " << cycleCount - 1 << " Cycles" << endl;
	int count = 0;
	string str_and;
	string str_or;
	string str_not;
	bool flag1 = true;
	for (const auto& i : output) {
		if (flag1) {
			flag1 = false;
			continue;
		}
		cout << "Cycle " << count << ":";
		count++;
		for (const auto& j : i) {

			for (int i = 0; i < model.and_assign.size(); i++) {
				if (j == model.and_assign.at(i)) {
					for (char ch : j) {
						str_and.push_back(ch); // ���ַ���ӵ� str_and ��  
					}
				}
			}
			for (int i = 0; i < model.or_assign.size(); i++) {
				if (j == model.or_assign.at(i)) {
					for (char ch : j) {
						str_or.push_back(ch); // ���ַ���ӵ� str_and ��  
					}
				}
			}
			for (int i = 0; i < model.not_assign.size(); i++) {
				if (j == model.not_assign.at(i)) {
					for (char ch : j) {
						str_not.push_back(ch); // ���ַ���ӵ� str_and ��  
					}
				}
			}

		}
		printString(str_and);
		cout << ",";
		printString(str_or);
		cout << ",";
		printString(str_not);
		str_and.clear();
		str_or.clear();
		str_not.clear();
		cout << endl;
	}
}

//void printString(string stri) {
//	cout << "{ ";
//	cout << stri[0];
//	for (size_t i = 1; i < stri.size(); ++i) {
//		std::cout << " " << stri[i];
//	}
//	cout << " }";
//}