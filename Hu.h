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



// ����ڵ�����翪ʼʱ��
int calculateEarliestStartTime(const Model& model, const Relationships& relationships) {
    unordered_map<string, int> indegrees;
    for (const auto& predList : relationships.predecessors) {
        for (const auto& pred : predList.second) {
            indegrees[pred]++;
        }
    }

    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> minHeap;
    unordered_map<string, int> earliestStartTimes;

    // ��ʼ�����翪ʼʱ��ӳ�䣬����û��ǰ���Ľڵ㣬���翪ʼʱ��Ϊ0
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

        // ���µ�ǰ�ڵ�����翪ʼʱ��
        earliestStartTimes[currentName] = time;

        // �������к�̽ڵ㣬�������ǵ����
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

// Hu �����㷨
void HuScheduling(const Model& model) {
    Relationships totalRelationships;

    // ���������߼��Ÿ�ֵ���
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