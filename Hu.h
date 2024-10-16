#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include "blif2verilog.h"
using namespace std;



// 计算节点的最早开始时间
int calculateEarliestStartTime(const Model& model, const Relationships& relationships) {
    unordered_map<string, int> indegrees;
    for (const auto& predList : relationships.predecessors) {
        for (const auto& pred : predList.second) {
            indegrees[pred]++;
        }
    }

    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> minHeap;
    unordered_map<string, int> earliestStartTimes;

    // 初始化最早开始时间映射，对于没有前驱的节点，最早开始时间为0
    for (const auto& name : model.names) {
        if (indegrees.find(name) == indegrees.end() || indegrees[name] == 0) {
            minHeap.push({ 0, name });
            earliestStartTimes[name] = 0;
        }
    }

    while (!minHeap.empty()) {
        auto top = minHeap.top();
        minHeap.pop();
        string currentName = top.second;
        int time = top.first;

        // 更新当前节点的最早开始时间
        earliestStartTimes[currentName] = time;

        // 遍历所有后继节点，更新它们的入度
        const auto& succs = relationships.successors.find(currentName);
        if (succs != relationships.successors.end()) {
            for (const auto& succ : succs->second) {
                int newIndegree = --indegrees[succ];
                if (newIndegree == 0) {
                    minHeap.push({ time + 1, succ });
                }
            }
        }
    }

    int maxTime = 0;
    for (const auto& entry : earliestStartTimes) {
        maxTime = max(maxTime, entry.second);
    }

    return maxTime;
}

// Hu 调度算法
void HuScheduling(const Model& model) {
    Relationships totalRelationships;

    // 遍历所有逻辑门赋值语句
    for (const auto& biaodashi : model.biaodashis) {
        Relationships temp = assignRelationship(biaodashi);
        for (const auto& pair : temp.predecessors) {
            totalRelationships.predecessors[pair.first].insert(totalRelationships.predecessors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
        for (const auto& pair : temp.successors) {
            totalRelationships.successors[pair.first].insert(totalRelationships.successors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
    }

    int maxTime = calculateEarliestStartTime(model, totalRelationships);
    cout << "Hu Maximum completion time (makespan): " << maxTime << endl;
}