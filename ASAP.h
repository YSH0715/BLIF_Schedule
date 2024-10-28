#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include <regex>
#include<map>
#include<algorithm> 
#include "blif2verilog.h"
using namespace std;

/*�洢ͼ�ڵ�*/
struct node {
	string name;
	vector<string> parent;
	vector<string> child;
	int priority = 0;
};

void input(vector<vector<string>>& Vec_Dti, string FileName);
void initData(map<string, node>& map, vector<vector<string>> inData);
void ASAP(vector<vector<string>>& output_ASAP, map<string, node>& m);
void delete_(map<string, node>& m, const vector<string>& key);
void printASAP(const vector<vector<string>>& output, Model model);
void printString(string stri);

void printGateASAP(Model model) {

	vector<vector<string>> inData;
	string FileName = "output.txt";
	input(inData, FileName);

	map<string, node> m;
	initData(m, inData);

	vector<vector<string>> output_ASAP;
	ASAP(output_ASAP, m);
	printASAP(output_ASAP, model);

	//system("PAUSE");
}


void input(vector<vector<string>>& Vec_Dti, string FileName) {
	vector<string> temp_line;
	string line;
	ifstream in(FileName);  //�����ļ�
	regex pat_regex("[a-z]*[0-9]*");  //ƥ��ԭ�� 

	while (getline(in, line)) {  //���ж�ȡ
		temp_line.clear();
		for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it; it != end_it; ++it) {  //���ʽƥ�䣬ƥ��һ�������������������ַ�
			temp_line.push_back(it->str());  //������ת��Ϊstring�Ͳ�����һάvector��
		}
		if (!temp_line.empty()) {
			Vec_Dti.push_back(temp_line);  //������������
		}
	}
}


void initData(map<string, node>& m, vector<vector<string>> inData) {
	for (int i = 0; i < inData.size(); i++) {
		if (inData[i].size() > 0) {
			node tmp;
			tmp.name = inData[i][0];//��¼ÿһ�е�һ���ڵ������
			for (int j = 1; j < inData[i].size(); j++) {
				if (inData[i].size() > j) {
					tmp.child.push_back(inData[i][j]);//��ÿһ�е�һ���ڵ������нڵ��¼���ýڵ�ĺ��ӽڵ�������
				}
			}
			m[tmp.name] = tmp;
		}
	}
	//ÿ�е�һ���ڵ㶼�Ǻ������нڵ�ĸ��ڵ㣬�����ڵ����Ϣ�洢��ÿ���ڵ�ĸ��ڵ�������
	for (int i = 0; i < inData.size(); i++) {
		if (inData[i].size() > 1) {
			for (int j = 1; j < inData[i].size(); j++) {
				if (inData[i].size() > j) {
					m[inData[i][j]].parent.push_back(inData[i][0]);
				}
			}
		}
	}
}


void ASAP(vector<vector<string>>& output_ASAP, map<string, node>& m) {
	while (!m.empty()) {//��ͼ�����нڵ㶼��ɾ��ʱ����
		vector<string> tmp;
		for (auto it = m.begin(); it != m.end(); ) {
			if (it->second.parent.empty()) {//�ҵ�����û�и��ڵ�Ľڵ㣬�������һ��tmp������
				tmp.push_back(it->first);
				it = m.erase(it); // ɾ���ڵ�
			}
			else {
				++it;
			}
		}
		if (!tmp.empty()) {
			output_ASAP.push_back(tmp);
		}
		delete_(m, tmp);
	}
}


void printString(string stri) {
	cout << "{ ";
	cout << stri[0];
	for (size_t i = 1; i < stri.size(); ++i) {
		std::cout << "," << stri[i];
	}
	cout << " }";
}

void printASAP(const vector<vector<string>>& output, Model model) {
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





void delete_(map<string, node>& m, const vector<string>& key) {
	for (const string& node_name : key) {
		auto it = m.find(node_name);
		if (it != m.end()) {
			m.erase(it);
		}
		for (auto& pair : m) {
			auto& node = pair.second;
			auto new_end = std::remove(node.parent.begin(), node.parent.end(), node_name);
			node.parent.erase(new_end, node.parent.end());
			auto new_end2 = std::remove(node.child.begin(), node.child.end(), node_name);
			node.child.erase(new_end2, node.child.end());
		}
	}
}