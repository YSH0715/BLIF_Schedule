#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include <regex>
#include<map>
#include<algorithm> 
#include "blif2verilog.h"
using namespace std;

/*存储图节点*/
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
	ifstream in(FileName);  //读入文件
	regex pat_regex("[a-z]*[0-9]*");  //匹配原则 

	while (getline(in, line)) {  //按行读取
		temp_line.clear();
		for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it; it != end_it; ++it) {  //表达式匹配，匹配一行中所有满足条件的字符
			temp_line.push_back(it->str());  //将数据转化为string型并存入一维vector中
		}
		if (!temp_line.empty()) {
			Vec_Dti.push_back(temp_line);  //保存所有数据
		}
	}
}


void initData(map<string, node>& m, vector<vector<string>> inData) {
	for (int i = 0; i < inData.size(); i++) {
		if (inData[i].size() > 0) {
			node tmp;
			tmp.name = inData[i][0];//记录每一行第一个节点的名字
			for (int j = 1; j < inData[i].size(); j++) {
				if (inData[i].size() > j) {
					tmp.child.push_back(inData[i][j]);//将每一行第一个节点后的所有节点记录到该节点的孩子节点数组中
				}
			}
			m[tmp.name] = tmp;
		}
	}
	//每行第一个节点都是后面所有节点的父节点，将父节点的信息存储到每个节点的父节点数组中
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
	while (!m.empty()) {//当图中所有节点都被删除时结束
		vector<string> tmp;
		for (auto it = m.begin(); it != m.end(); ) {
			if (it->second.parent.empty()) {//找到所有没有父节点的节点，将其存在一个tmp数组中
				tmp.push_back(it->first);
				it = m.erase(it); // 删除节点
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
						str_and.push_back(ch); // 将字符添加到 str_and 中  
					}
				}
			}
			for (int i = 0; i < model.or_assign.size(); i++) {
				if (j == model.or_assign.at(i)) {
					for (char ch : j) {
						str_or.push_back(ch); // 将字符添加到 str_and 中  
					}
				}
			}
			for (int i = 0; i < model.not_assign.size(); i++) {
				if (j == model.not_assign.at(i)) {
					for (char ch : j) {
						str_not.push_back(ch); // 将字符添加到 str_and 中  
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