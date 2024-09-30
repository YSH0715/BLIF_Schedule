#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<regex>
#include<map>
#include<algorithm> 
#include "blif2verilog.h"
#include<set>
using namespace std;

/*�洢ͼ�ڵ�*/
struct node {
	string name;
	vector<string> parent;
	vector<string> child;
	int priority = 0;
};

void ASAPprintGate(Model model);
void input(vector<vector<string>>& Vec_Dti, string FileName);
void initData(map<string, node>& map, vector<vector<string>> inData);
void ASAP(vector<vector<string>>& output_ASAP, map<string, node>& m);
void delete_(map<string, node>& m, const vector<string>& key);
void print(const vector<vector<string>>& output,const Model &model);
void printCategory(const set<string>& categorySet, const vector<string>& modelCategory, const string& category);


void ASAPprintGate(Model model) {

	vector<vector<string>> inData;
	string FileName = "output.txt";
	input(inData, FileName);

	map<string, node> m;
	initData(m, inData);

	vector<vector<string>> output_ASAP;
	ASAP(output_ASAP, m);
	print(output_ASAP,model);

	system("PAUSE");
}


void input(vector<vector<string>>& Vec_Dti, string FileName) {
	vector<string> temp_line;
	string line;
	ifstream in(FileName);  //�����ļ�
	regex pat_regex("[a-z]+");  //ƥ��ԭ�� һ������Сд��ĸ

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



void print(const vector<vector<string>>& output, const Model& model) {
	int cycleCount = output.size();
	cout << "Total " << cycleCount -1 << " Cycles" << endl ;

	int count = 0;
	bool flag1 = true;
	for (const auto& cycle : output) {
		if (flag1) {
			flag1 = false;
			continue;
		}
	    cout << "Cycle " << count++ << ":";
		// ���ڴ洢��ʱ������ַ�������
		set<string> andSet;
		set<string> orSet;
		set<string> notSet;

		// ����ÿ���ڵ�
		for (const auto& j : cycle) {
			// ���ڵ������ĸ���𣬲���ӵ���Ӧ�ļ�����
			if (find(model.and_assign.begin(), model.and_assign.end(), j) != model.and_assign.end()) {
				andSet.insert(j);
			}
			if (find(model.or_assign.begin(), model.or_assign.end(), j) != model.or_assign.end()) {
				orSet.insert(j);
			}
			if (find(model.not_assign.begin(), model.not_assign.end(), j) != model.not_assign.end()) {
				notSet.insert(j);
			}
		}

		printCategory(andSet, model.and_assign, "and_assign");

		printCategory(orSet, model.or_assign, "or_assign");

		printCategory(notSet, model.not_assign, "not_assign");

		cout << endl;
	}
}

void printCategory(const set<string>& categorySet, const vector<string>& modelCategory, const string& category) {
	if (categorySet.empty()) {
		cout << "{ } ";
	}
	else {
		cout << "{ ";
		bool first = true;
		for (const auto& item : categorySet) {
			if (!first) {
				cout << " ";
			}
			cout << item;
			first = false;
		}
		cout << " } ";
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