#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <map>
#include <algorithm>
#include "blif2verilog.h"
#include "ALAP.h"
#include "ASAP.h"
using namespace std;

int nodecount;
int resource;
int ALAPcycle;
map<string, int> max_resource_needed = { {"AND_GATE",1},{"OR_GATE",1},{"NOT_GATE",1} };
map<string, int> tem_resource = max_resource_needed;
void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node>& m, int latency_constraint, map<string, int>& last_time);
void printMR_LCS(const vector<vector<string>>& output, Model model);
void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output_ALAP, int latency_constraint, Model model);


void printGateMR_LCS(Model model) {
    nodecount = model.and_assign.size() + model.or_assign.size() + model.not_assign.size();
    vector<vector<string>> inData;
    string FileName = "output.txt";
    input(inData, FileName);

    map<string, node> m;
    initData(m, inData);

    vector<vector<string>> output_ALAP;
    ALAP(output_ALAP, m);
    reverse(output_ALAP.begin(), output_ALAP.end());
    ALAPcycle = output_ALAP.size() - 1;

    vector<vector<string>> output_MR_LCS;
    map<string, int> last_time;
    int latency_constraint = 5;   // 假设延迟约束为5，根据需要调整

    Last_Cycle(last_time, output_ALAP, latency_constraint, model);

    MR_LCS(output_MR_LCS, m, latency_constraint, last_time);
    printMR_LCS(output_MR_LCS, model);
    //system("PAUSE");
}

void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output, int latency_constraint, Model model) {
    int count = 0;
    for (const auto& i : output) {
        for (const auto& j : i) {
            bool flag = false;
            for (int i = 0; i < model.and_assign.size(); i++) {
                if (j == model.and_assign.at(i)) {
                    flag = true;
                }
            }
            for (int i = 0; i < model.or_assign.size(); i++) {
                if (j == model.or_assign.at(i)) {
                    flag = true;
                }
            }
            for (int i = 0; i < model.not_assign.size(); i++) {
                if (j == model.not_assign.at(i)) {
                    flag = true;
                }
            }
            if (flag == true) {
                last_time[j] = latency_constraint - ALAPcycle + count - 1;
            }
        }
        count++;
    }
}

void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node>& m, int latency_constraint, map<string, int>& last_time) {
    if (latency_constraint > nodecount) {
        resource = max_resource_needed["AND_GATE"] + max_resource_needed["OR_GATE"] + max_resource_needed["NOT_GATE"];
    }
    else if (ALAPcycle <= latency_constraint <= nodecount) {

    }
    else {
        cout << "无法在规定时间内完成任务";
    }

    /*int current_latency = 0; // 当前延迟
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
    }*/
}

void printMR_LCS(const vector<vector<string>>& output, Model model) {
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

