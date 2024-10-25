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
struct time_node {
    map<int, int>time_nodes;
    time_node() : time_nodes({ {1, 0} }) {}
};
map<string, time_node> max_resource_needed = {
        {"AND_GATE", time_node()},
        {"OR_GATE", time_node()},
        {"NOT_GATE", time_node()}
};
map<string, time_node> tem_resource = max_resource_needed;  //存储暂时的资源数量可能会变化
map<string, int>ComsumingTime = { {"AND_GATE", 2},{"OR_GATE", 3},{"NOT_GATE", 1} };
void MR_LCS(vector<vector<string>>& output_MR_LCS, map<string, node>& m, int latency_constraint, map<string, int>& last_time, Model model);
void printMR_LCS(const vector<vector<string>>& output, Model model);
void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output_ALAP, int latency_constraint, map<string, node>& m, Model model);
void delete_input(map<string, node>& m, Model model);
int Last_time_node(map<string, node>& m, map<string, int>& last_time, string a);


void printGateMR_LCS(Model model) {
    nodecount = model.and_assign.size() * 2 + model.or_assign.size() * 3 + model.not_assign.size();
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
    //reverse(output_ALAP.begin(), output_ALAP.end());
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

//确定在给定的延迟约束下，每个逻辑门最晚可以在何时被触发
void Last_Cycle(map<string, int>& last_time, vector<vector<string>>& output, int latency_constraint, map<string, node>& m, Model model) {
    if (output.empty()) {
        cout << "Output vector is empty.";
        return;
    }
    for (const auto& i : output) {
        for (const auto& j : i) {
            int time = INT_MAX;
            if (find(model.and_assign.begin(), model.and_assign.end(), j) != model.and_assign.end()) {
                //没有子节点则它的最后可用时间被设置为 latency_constraint - 2
                // 否则最后可用时间是它的子节点的最后可用时间的最小值减去2
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
            if (find(model.or_assign.begin(), model.or_assign.end(), j) != model.or_assign.end()) {
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
            if (find(model.not_assign.begin(), model.not_assign.end(), j) != model.not_assign.end()) {
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
            ALAPcycle = latency_constraint - last_time[j];
        }
    }
}

void delete_input(map<string, node>& m, Model model) {
    vector<string> tmp;//用来存储要删除的元素
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
    if (latency_constraint >= nodecount) {//延迟约束大于节点数量
        cout << "resource_&:" << max_resource_needed["AND_GATE"].time_nodes.size() << " resource_|:" << max_resource_needed["OR_GATE"].time_nodes.size() << " resource_!:" << max_resource_needed["NOT_GATE"].time_nodes.size();
    }
    else if (ALAPcycle <= latency_constraint && latency_constraint < nodecount) {
        map<string, int> tmp_and_time;
        map<string, int> tmp_or_time;
        while (!last_time.empty()) {
            int and_size = 0, or_size = 0, not_size = 0;
            vector<string> tmp;
            vector<string> to_erase;
            for (auto it = tmp_and_time.begin(); it != tmp_and_time.end(); ) {
                tmp.push_back(it->first);
                it = tmp_and_time.erase(it);
            }
            for (auto it = tmp_or_time.begin(); it != tmp_or_time.end(); ) {
                if (it->second == 2) {
                    tmp.push_back(it->first);
                    it = tmp_or_time.erase(it);
                }
                else {
                    ++it->second;
                    ++it;
                }
            }
            for (const auto& b : tem_resource["AND_GATE"].time_nodes) {
                if (b.second == 2) {
                    tem_resource["AND_GATE"].time_nodes[b.first] = 0;
                }
                if (b.second == 1) {
                    tem_resource["AND_GATE"].time_nodes[b.first] = 2;
                }
            }
            for (const auto& b : tem_resource["OR_GATE"].time_nodes) {
                if (b.second == 3) {
                    tem_resource["OR_GATE"].time_nodes[b.first] = 0;
                }
                if (b.second == 2) {
                    tem_resource["OR_GATE"].time_nodes[b.first] = 3;
                }
                if (b.second == 1) {
                    tem_resource["OR_GATE"].time_nodes[b.first] = 2;
                }
            }
            for (const auto& b : tem_resource["AND_GATE"].time_nodes) {
                if (b.second == 0) {
                    and_size++;
                }
            }
            for (const auto& b : tem_resource["OR_GATE"].time_nodes) {
                if (b.second == 0) {
                    or_size++;
                }
            }
            for (const auto& b : tem_resource["NOT_GATE"].time_nodes) {
                if (b.second == 0) {
                    not_size++;
                }
            }
            for (auto it = last_time.begin(); it != last_time.end(); ) {
                if (it->second == 0) {
                    if (find(model.and_assign.begin(), model.and_assign.end(), it->first) != model.and_assign.end()) {
                        if (and_size > 0) {
                            and_size -= 1;
                            for (const auto& b : tem_resource["AND_GATE"].time_nodes) {
                                if (b.second == 0) {
                                    tem_resource["AND_GATE"].time_nodes[b.first] = 1;
                                }
                                break;
                            }
                        }
                        else {
                            int i = tem_resource["AND_GATE"].time_nodes.size() + 1;
                            tem_resource["AND_GATE"].time_nodes.emplace(i, 1);
                        }
                        tmp_and_time[it->first] = 1;
                    }
                    if (find(model.or_assign.begin(), model.or_assign.end(), it->first) != model.or_assign.end()) {
                        if (or_size > 0) {
                            or_size -= 1;
                            for (const auto& b : tem_resource["OR_GATE"].time_nodes) {
                                if (b.second == 0) {
                                    tem_resource["OR_GATE"].time_nodes[b.first] = 1;
                                }
                                break;
                            }
                        }
                        else {
                            int i = tem_resource["OR_GATE"].time_nodes.size() + 1;
                            tem_resource["OR_GATE"].time_nodes.emplace(i, 1);
                        }
                        tmp_or_time[it->first] = 1;
                    }
                    if (find(model.not_assign.begin(), model.not_assign.end(), it->first) != model.not_assign.end()) {
                        if (not_size > 0) {
                            not_size -= 1;
                        }
                        else {
                            int i = tem_resource["NOT_GATE"].time_nodes.size() + 1;
                            tem_resource["NOT_GATE"].time_nodes.emplace(i, 0);
                        }
                        tmp.push_back(it->first);
                    }
                    to_erase.push_back(it->first);
                    it = last_time.erase(it);
                }
                else {
                    --it->second;
                    ++it;
                }
            }
            if (and_size > 0 || or_size > 0 || not_size > 0) {
                multimap<int, string> sorted_and_map;
                multimap<int, string> sorted_or_map;
                multimap<int, string> sorted_not_map;
                for (auto it = last_time.begin(); it != last_time.end(); ) {
                    if (m[it->first].parent.size() == 0) {
                        if (find(model.and_assign.begin(), model.and_assign.end(), it->first) != model.and_assign.end()) {
                            int last = Last_time_node(m, last_time, it->first);
                            sorted_and_map.insert({ last, it->first });
                        }
                        if (find(model.or_assign.begin(), model.or_assign.end(), it->first) != model.or_assign.end()) {
                            int last = Last_time_node(m, last_time, it->first);
                            sorted_or_map.insert({ last, it->first });
                        }
                        if (find(model.not_assign.begin(), model.not_assign.end(), it->first) != model.not_assign.end()) {
                            int last = Last_time_node(m, last_time, it->first);
                            sorted_not_map.insert({ last, it->first });
                        }
                        ++it;
                        continue;
                    }
                    else {
                        ++it;
                        continue;
                    }
                }
                if (sorted_and_map.size() > 0) {
                    for (auto it = sorted_and_map.begin(); it != sorted_and_map.end(); ) {
                        if (and_size > 0) {
                            and_size -= 1;
                            for (const auto& b : tem_resource["AND_GATE"].time_nodes) {
                                if (b.second == 0) {
                                    tem_resource["AND_GATE"].time_nodes[b.first] = 1;
                                }
                                break;
                            }
                            tmp_and_time[it->second] = 1;
                            to_erase.push_back(it->second);
                            it = sorted_and_map.erase(it);
                        }
                        else {
                            ++it;
                            continue;
                        }
                    }
                }
                if (sorted_or_map.size() > 0) {
                    for (auto it = sorted_or_map.begin(); it != sorted_or_map.end(); ) {
                        if (or_size > 0) {
                            or_size -= 1;
                            for (const auto& b : tem_resource["OR_GATE"].time_nodes) {
                                if (b.second == 0) {
                                    tem_resource["OR_GATE"].time_nodes[b.first] = 1;
                                }
                                break;
                            }
                            tmp_or_time[it->second] = 1;
                            to_erase.push_back(it->second);
                            it = sorted_or_map.erase(it);
                        }
                        else {
                            ++it;
                            continue;
                        }
                    }
                }
                if (sorted_not_map.size() > 0) {
                    for (auto it = sorted_not_map.begin(); it != sorted_not_map.end(); ) {
                        if (not_size > 0) {
                            not_size -= 1;
                            tmp.push_back(it->second);
                            to_erase.push_back(it->second);
                            it = sorted_not_map.erase(it);
                        }
                        else {
                            ++it;
                            continue;
                        }
                    }
                }
            }
            if (!tmp.empty()) {
                output_MR_LCS.push_back(tmp);
            }
            for (const auto& key : to_erase) {
                if (last_time.find(key) != last_time.end()) {
                    last_time.erase(key);
                }
            }
            delete_(m, tmp);
        }
        max_resource_needed = tem_resource;
        printMR_LCS(output_MR_LCS, model);
    }
    else {
        cout << "时间太少无法完成整体任务";
    }
}

int Last_time_node(map<string, node>& m, map<string, int>& last_time, string a) {
    int last = INT_MIN;
    for (const string& it : m[a].child) {
        last = max(last, last_time[it]);
    }
    return last;
}

void printMR_LCS(const vector<vector<string>>& output, Model model) {
    cout << "resource_&:" << max_resource_needed["AND_GATE"].time_nodes.size() << " resource_|:" << max_resource_needed["OR_GATE"].time_nodes.size() << " resource_!:" << max_resource_needed["NOT_GATE"].time_nodes.size();
}
