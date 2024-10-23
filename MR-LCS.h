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
map<string, int> tem_resource = max_resource_needed;  //存储暂时的资源数量可能会变化
map<string, int>ComsumingTime = { {"AND_GATE", 2},{"OR_GATE", 3},{"NOT_GATE", 1} };
void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node>& m, int latency_constraint, map<string, int>& last_time, Model model);
void printMR_LCS(const vector<vector<string>>& output, Model model);
void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output, int latency_constraint, map<string, node>& m, Model model);
void delete_input(map<string, node>& m, Model model);


void printGateMR_LCS(Model model) {
    nodecount = model.and_assign.size() + model.or_assign.size() + model.not_assign.size();
    vector<vector<string>> inData;
    string FileName = "output.txt";
    input(inData, FileName);

    map<string, node> m;
    initData(m, inData);
    delete_input(m, model);
    map<string, node> n = m;
    map<string, node> a = m;
    vector<vector<string>> output_ALAP;
    ALAP(output_ALAP, m);
    //ALAPcycle = output_ALAP.size();

    vector<vector<string>> output_MR_LCS;
    map<string, int> last_time;
    int latency_constraint;   //cycle数
    cout << "输入cycle数：";
    cin >> latency_constraint;
    Last_Cycle(last_time, output_ALAP, latency_constraint, a, model);


    MR_LCS(output_MR_LCS, n, latency_constraint, last_time, model);
    //printMR_LCS(output_MR_LCS, model);
    // system("PAUSE");
}

void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output, int latency_constraint, map<string, node>& m, Model model) {
    if (output.empty()) {
        cout << "Output vector is empty.";
        return;
    }
    int count = 0;
    for (const auto& i : output) {
        for (const auto& j : i) {
            int time = INT_MAX;
            for (int i = 0; i < model.and_assign.size(); i++) {
                if (std::find(model.and_assign.begin(), model.and_assign.end(), j) != model.and_assign.end()) {
                    if (m[j].child.empty()) {
                        last_time[j] = latency_constraint - 2;
                    }
                    else {
                        for (const auto& str : m[j].child) {
                            time = min(time, last_time[str]);
                        }
                        last_time[j] = time - 2;
                    }
                }
            }
            for (int i = 0; i < model.or_assign.size(); i++) {
                if (std::find(model.or_assign.begin(), model.or_assign.end(), j) != model.or_assign.end()) {
                    if (m[j].child.empty()) {
                        last_time[j] = latency_constraint - 3;
                    }
                    else {
                        for (const auto& str : m[j].child) {
                            time = min(time, last_time[str]);
                        }
                        last_time[j] = time - 3;
                    }
                }
            }
            for (int i = 0; i < model.not_assign.size(); i++) {
                if (std::find(model.not_assign.begin(), model.not_assign.end(), j) != model.not_assign.end()) {
                    if (m[j].child.empty()) {
                        last_time[j] = latency_constraint - 1;
                    }
                    else {
                        for (const auto& str : m[j].child) {
                            time = min(time, last_time[str]);
                        }
                        last_time[j] = time - 1;
                    }
                }
            }
            ALAPcycle = latency_constraint - last_time[j];
        }
        count++;
    }
}

void delete_input(map<string, node>& m, Model model) {
    vector<string> tmp;
    for (auto it = m.begin(); it != m.end(); ) {
        bool flag = false;
        for (const auto& assign : model.and_assign) {
            if (it->first == assign) {
                flag = true;
                break;
            }
        }

        for (const auto& assign : model.or_assign) {
            if (it->first == assign) {
                flag = true;
                break;
            }
        }

        for (const auto& assign : model.not_assign) {
            if (it->first == assign) {
                flag = true;
                break;
            }
        }
        if (flag == false) {
            tmp.push_back(it->first);
            it = m.erase(it);
        }
        else {
            ++it;
        }
    }
    delete_(m, tmp);
}

void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node>& m, int latency_constraint, map<string, int>& last_time, Model model) {
    if (latency_constraint >= nodecount) {
        resource = max_resource_needed["AND_GATE"] + max_resource_needed["OR_GATE"] + max_resource_needed["NOT_GATE"];
    }
    else if (ALAPcycle <= latency_constraint && latency_constraint < nodecount) {
        while (!last_time.empty()) {
            vector<string> tmp;
            vector<string> to_erase;
            for (auto it = last_time.begin(); it != last_time.end(); ) {
                if (it->second == 0) {
                    if (find(model.and_assign.begin(), model.and_assign.end(), it->first) != model.and_assign.end()) {
                        if (tem_resource["AND_GATE"] > 0) {
                            tem_resource["AND_GATE"] -= 1;
                        }
                        else {
                            max_resource_needed["AND_GATE"] += 1;
                        }
                    }
                    if (find(model.or_assign.begin(), model.or_assign.end(), it->first) != model.or_assign.end()) {
                        if (tem_resource["OR_GATE"] > 0) {
                            tem_resource["OR_GATE"] -= 1;
                        }
                        else {
                            max_resource_needed["OR_GATE"] += 1;
                        }
                    }
                    if (find(model.not_assign.begin(), model.not_assign.end(), it->first) != model.not_assign.end()) {
                        if (tem_resource["NOT_GATE"] > 0) {
                            tem_resource["NOT_GATE"] -= 1;
                        }
                        else {
                            max_resource_needed["NOT_GATE"] += 1;
                        }
                    }
                    tmp.push_back(it->first);
                    to_erase.push_back(it->first);
                    it = last_time.erase(it);
                }
                else {
                    --it->second;
                    ++it;
                }
            }
            while (tem_resource["AND_GATE"] > 0 || tem_resource["OR_GATE"] > 0 || tem_resource["NOT_GATE"] > 0) {
                multimap<int, string> sorted_map;
                for (const auto& pair : last_time) {
                    sorted_map.insert({ pair.second, pair.first });
                }
                for (auto it = sorted_map.begin(); it != sorted_map.end(); ) {
                    if (m[it->second].parent.size() == 0) {
                        if (find(model.and_assign.begin(), model.and_assign.end(), it->second) != model.and_assign.end()) {
                            if (tem_resource["AND_GATE"] > 0) {
                                tem_resource["AND_GATE"] -= 1;
                                tmp.push_back(it->second);
                                to_erase.push_back(it->second);
                                it = sorted_map.erase(it);
                            }
                            else {
                                ++it;
                                continue;
                            }
                        }
                        if (find(model.or_assign.begin(), model.or_assign.end(), it->second) != model.or_assign.end()) {
                            if (tem_resource["OR_GATE"] > 0) {
                                tem_resource["OR_GATE"] -= 1;
                                tmp.push_back(it->second);
                                to_erase.push_back(it->second);
                                it = sorted_map.erase(it);
                            }
                            else {
                                ++it;
                                continue;
                            }
                        }
                        if (find(model.not_assign.begin(), model.not_assign.end(), it->second) != model.not_assign.end()) {
                            if (tem_resource["NOT_GATE"] > 0) {
                                tem_resource["NOT_GATE"] -= 1;
                                tmp.push_back(it->second);
                                to_erase.push_back(it->second);
                                it = sorted_map.erase(it);
                            }
                            else {
                                ++it;
                                continue;
                            }
                        }
                    }
                    else {
                        ++it;
                    }
                }
                break;
            }
            if (!tmp.empty()) {
                output_MR_LCS.push_back(tmp);
            }
            tem_resource = max_resource_needed;
            for (const auto& key : to_erase) {
                if (last_time.find(key) != last_time.end()) {
                    last_time.erase(key);
                }
            }
            // tem_resource = max_resource_needed;
            delete_(m, tmp);
        }
        printMR_LCS(output_MR_LCS, model);

    }
    else {
        cout << "时间太少无法完成整体任务";
    }
}



void printMR_LCS(const vector<vector<string>>& output, Model model) {
    int a = max_resource_needed["AND_GATE"] + max_resource_needed["OR_GATE"] + max_resource_needed["NOT_GATE"];
    cout << "resource_&:" << max_resource_needed["AND_GATE"] << " resource_|:" << max_resource_needed["OR_GATE"] << " resource_!:" << max_resource_needed["NOT_GATE"];
}

