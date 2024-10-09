#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <algorithm>
#include "blif2verilog.h"
using namespace std;

/* 存储图节点 */
struct node2 {
    string name;
    vector<string> parent;
    vector<string> child;
    int resource_needed ; //=2  假设每个节点需要1个资源
    
};

void input(vector<vector<string>>& Vec_Dti, string FileName);
void initData(map<string, node2>& map, vector<vector<string>> inData);
void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node2>& m, int available_resources, int latency_constraint);
void delete_(map<string, node2>& m, const vector<string>& key);
void MR_LCS_print(const vector<vector<string>>& output, Model model);

void MR_LCS_printGate(Model model) {
    vector<vector<string>> inData;
    string FileName = "output.txt";
    input(inData, FileName);

    map<string, node2> m;
    initData(m, inData);

    vector<vector<string>> output_MR_LCS;
    int available_resources = 1;// 假设可用资源数量为2，根据需要调整
    int latency_constraint = 1;   // 假设延迟约束为5，根据需要调整
    MR_LCS(output_MR_LCS, m, available_resources, latency_constraint);
    MR_LCS_print(output_MR_LCS, model);

    system("PAUSE");
}

void input(vector<vector<string>>& Vec_Dti, string FileName) {
    vector<string> temp_line;
    string line;
    ifstream in(FileName);  // 打开文件
    regex pat_regex("[a-z]+");  // 匹配模式 一串连续的小写字母

    while (getline(in, line)) {  // 按行读取
        temp_line.clear();
        for (sregex_iterator it(line.begin(), line.end(), pat_regex), end_it; it != end_it; ++it) {  // 正则表达式匹配
            temp_line.push_back(it->str());  // 将匹配结果转换为字符串并存储到一维vector中
        }
        if (!temp_line.empty()) {
            Vec_Dti.push_back(temp_line);  // 存储到二维vector中
        }
    }
}

void initData(map<string, node2>& m, vector<vector<string>> inData) {
    for (const auto& line : inData) {
        if (!line.empty()) {
            node2 tmp;
            tmp.name = line[0]; // 记录每一行的第一个节点名称
            for (size_t j = 1; j < line.size(); j++) {
                tmp.child.push_back(line[j]); // 将每一行的第一个节点名称的子节点记录到该节点的子节点列表中
            }
            m[tmp.name] = tmp;
        }
    }
    // 每行的第一个节点都是其子节点的父节点，将父节点信息存储到每个子节点的父节点列表中
    for (const auto& line : inData) {
        if (line.size() > 1) {
            for (size_t j = 1; j < line.size(); j++) {
                m[line[j]].parent.push_back(line[0]);
            }
        }
    }
}

void MR_LCS_print(const vector<vector<string>>& output, Model model) {
    int count = 0;
    cout << "PIs :";
    for (const auto& input : model.inputs) {
        cout << " " << input;
    }
    cout << " Output :";
    for (const auto& output_name : model.outputs) {
        cout << " " << output_name;
    }
    cout << endl;

    for (const auto& cycle : output) {
        cout << "Cycle " << count << ":";
        for (const auto& j : cycle) {
            cout << " " << j;
        }
        cout << endl;
        count++;
    }
}

void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node2>& m, int available_resources, int latency_constraint) {
    int current_latency = 0; // 当前延迟
    while (!m.empty()) { // 当图中的所有节点都被删除时结束
        vector<string> tmp;
        int used_resources = 0;

        for (auto it = m.begin(); it != m.end();) {
            // 找到没有父节点且可用资源充足的节点
            if (it->second.parent.empty() && (used_resources + it->second.resource_needed <= available_resources)) {
                tmp.push_back(it->first);
                used_resources += it->second.resource_needed; // 更新已用资源
                it = m.erase(it); // 删除节点
            }
            else {
                ++it;
            }
        }

        if (!tmp.empty()) {
            output_MR_LCS.push_back(tmp);
            current_latency++; // 增加当前延迟
            if (current_latency >= latency_constraint) {
                break; // 达到延迟约束，停止调度
            }
        }
        delete_(m, tmp); // 删除节点及其关系
    }
}

void delete_(map<string, node2>& m, const vector<string>& key) {
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
