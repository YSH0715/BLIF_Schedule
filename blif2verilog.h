#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;




struct Model {
    std::string name;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> names;
    std::vector<std::string> latches;
    std::vector<std::string> biaodashis;
    std::vector<char> digits;
    vector<string> and_assign;
    vector<string> or_assign;
    vector<string> not_assign;
};


Model getAssign(const std::string& filename) {
    Model model;
    std::ifstream file(filename);
    std::string line;
    int count = 0;
    int count0 = 0, count1 = 0, countDash = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    int n = 0;
    std::vector<std::string> stringArray(100);

    while (std::getline(file, line)) {

        std::istringstream iss(line);
        std::string token;
        std::string token1;
        std::string token2;
        std::string token3;

        iss >> token;
        if (token == ".model") {
            iss >> model.name;
        }
        else if (token == ".inputs") {
            while (iss >> token) {
                model.inputs.push_back(token);
            }
        }
        else if (token == ".outputs") {
            while (iss >> token) {
                model.outputs.push_back(token);
            }
        }
        else if (token == ".names") {
            while (iss >> token) {
                model.names.push_back(token);
            }
            count2 = 0;
        }
        else if (token == ".latch") {
            std::string input, output, type, control;
            iss >> input >> output;
            model.latches.push_back("latch (" + input + ", " + output + ");");
        }
        else if (token == ".end") {
        }
        else {
            count2++;
            for (char ch : token) {
                if (ch == '0') {
                    model.digits.push_back(ch);
                    count++;
                    count0++;
                }
                else if (ch == '1') {
                    model.digits.push_back(ch);
                    count++;
                    count1++;
                }
                else if (ch == '-') {
                    model.digits.push_back(ch);
                    count++;
                    countDash++;
                }
            }
            if (countDash == 0 && count2 == 1) {
                if (count0 == 1) {
                    token = model.names[model.names.size() - 1] + "=!" + model.names[model.names.size() - 2];
                    model.biaodashis.push_back(token);
                    //存储非门
                    model.not_assign.push_back(model.names[model.names.size() - 1]);
                }
                else {
                    token1 += model.names[model.names.size() - count1 - 1];
                    for (int i = 1; i < count1; i++) {
                        token1 += "&" + model.names[model.names.size() - i - 1];
                    }
                    token = model.names[model.names.size() - 1] + "=" + token1;
                    model.biaodashis.push_back(token);
                    //存储与门
                    model.and_assign.push_back(model.names[model.names.size() - 1]);
                }
                count0 = 0;
                count1 = 0;
                countDash = 0;
            }
            else {
                for (int i = count; i > 0; i--) {
                    char digit = model.digits[model.digits.size() - i];
                    count3++;
                    if (digit == '1') {
                        token1 += model.names[model.names.size() - count - 2 + count3];
                        count4++;
                    }
                    if (count4 > 1) {
                        token1 += "&" + model.names[model.names.size() - count - 2 + count3];
                    }
                }
                stringArray[static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(count2) - 1] = token1;
                if (count2 > 1) {
                    token2 += stringArray[0];
                    for (int i = 0; i < count2 - 1; i++) {
                        token2 += "|" + stringArray[static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(i) + 1];
                    }
                    token = model.names[model.names.size() - 1] + "=" + token2;
                    model.biaodashis.push_back(token);
                    //存储或门
                    model.or_assign.push_back(model.names[model.names.size() - 1]);
                    sort(model.or_assign.begin(), model.or_assign.end());
                    model.or_assign.erase(unique(model.or_assign.begin(), model.or_assign.end()), model.or_assign.end());
                    if (model.biaodashis.size() > 1) {
                        token3 = model.biaodashis[model.biaodashis.size() - 2];
                        if (token[0] == token3[0]) {
                            model.biaodashis[model.biaodashis.size() - 2] = token;
                            model.biaodashis.pop_back();
                        }
                    }
                }
                count3 = 0;
                count4 = 0;
            }
            count = 0;
            count0 = 0;
            count1 = 0;
            countDash = 0;
        }
    }
    return model;
}


Model readBlifFile(const std::string& filename) {
    Model model;
    std::ifstream file(filename);
    std::string line;
    int count = 0;

    while (std::getline(file, line)) {

        std::istringstream iss(line);
        std::string token;

        iss >> token;
        if (token == ".model") {
            iss >> model.name;
        }
        else if (token == ".inputs") {
            while (iss >> token) {
                model.inputs.push_back(token);
            }
        }
        else if (token == ".outputs") {
            while (iss >> token) {
                model.outputs.push_back(token);
            }
        }
        else if (token == ".names") {
            model.names.clear();
            while (iss >> token) {
                model.names.push_back(token);
            }
            count = 0;
        }
        else if (token == ".latch") {
            std::string input, output, type, control;
            iss >> input >> output;

            model.latches.push_back("latch (" + input + ", " + output + ");");
        }
        else if (token == ".end") {
            // Do nothing
        }
        else {
            count++;
            model.digits.clear();
            for (char ch : token) {
                model.digits.push_back(ch);
            }
            std::string expression;
            if (count == 1) {
                expression = model.names[model.names.size() - 1] + "=";
                for (size_t i = 0; i < model.digits.size(); ++i) {
                    if (model.digits[i] == '0') {
                        expression += "!" + model.names[i];
                    }
                    else if (model.digits[i] == '1') {
                        expression += model.names[i] + "&";
                    }
                }
                if (expression.back() == '&') {
                    expression.pop_back();
                }
                model.biaodashis.push_back(expression);
            }
            else {
                std::string token1 = "";
                expression = model.biaodashis.back();
                model.biaodashis.pop_back();
                for (size_t i = 0; i < model.digits.size(); ++i) {
                    if (model.digits[i] == '0') {
                        token1 += "!" + model.names[i];
                    }
                    else if (model.digits[i] == '1') {
                        token1 += model.names[i] + "&";
                    }
                }
                if (token1.back() == '&') {
                    token1.pop_back();
                }
                expression += "|" + token1;
                model.biaodashis.push_back(expression);
            }
        }
    }
    std::sort(model.names.begin(), model.names.end());
    auto last = std::unique(model.names.begin(), model.names.end());
    model.names.erase(last, model.names.end());

    return model;
}


void writeVerilogFile(const Model& model, const std::string& filename) {
    std::ofstream verilogFile(filename);


    verilogFile << "module " << model.name << ";\n";
    for (const auto& input : model.inputs) {
        verilogFile << "input " << input << ";\n";
    }
    verilogFile << "\n";
    for (const auto& output : model.outputs) {
        verilogFile << "output " << output << ";\n";
    }
    verilogFile << "\n";
    for (const auto& name : model.names) {
        verilogFile << "wire " << name << ";\n";
    }
    verilogFile << "\n";
    for (const auto& biaodashi : model.biaodashis) {
        verilogFile << "assign " << biaodashi << ";\n";
    }
    verilogFile << "\n";
    for (const auto& latch : model.latches) {
        verilogFile << latch << "\n";
    }
    verilogFile << "\n";
    verilogFile << "endmodule\n";

    verilogFile.close();
}



void print(Model model) {
    cout << "Input :";
    for (const auto& input : model.inputs)
    {
        cout << input << " ";
    }
    cout << "Output :";
    for (const auto& output : model.outputs) {
        cout << output << " ";
    }
    cout << endl;

}



struct Relationships {
    std::unordered_map<std::string, std::vector<std::string>> predecessors;
    std::unordered_map<std::string, std::vector<std::string>> successors;
};

Relationships assignRelationship(const std::string& assignment) {
    Relationships relationships;
    std::istringstream iss(assignment);
    std::string leftSide, rightSide;


    std::getline(iss, leftSide, '=');
    std::getline(iss, rightSide);

    std::string leftNode = leftSide;  
    std::istringstream rightStream(rightSide);
    std::string rightNode;


    while (std::getline(rightStream, rightNode, '|')) {
        rightNode.erase(remove(rightNode.begin(), rightNode.end(), ' '), rightNode.end());
        if (rightNode.front() == '!') {
            std::string negatedNode = rightNode.substr(1);  
            negatedNode.erase(remove(negatedNode.begin(), negatedNode.end(), ' '), negatedNode.end());
            relationships.predecessors[negatedNode].push_back(leftNode);
            relationships.successors[leftNode].push_back(negatedNode);
        }

        else if (rightNode.find('&') != std::string::npos) {
            std::istringstream andStream(rightNode);
            std::string andNode;
            while (std::getline(andStream, andNode, '&')) {
                andNode.erase(remove(andNode.begin(), andNode.end(), ' '), andNode.end());
                relationships.predecessors[andNode].push_back(leftNode);
                relationships.successors[leftNode].push_back(andNode);
            }
        }
        else {
            relationships.predecessors[rightNode].push_back(leftNode);
            relationships.successors[leftNode].push_back(rightNode);
        }
    }

    return relationships;
}



void getGate(Model model) {
    std::vector<std::string> assignments;
    for (int i = 0;i < model.biaodashis.size();i++) {
        assignments.push_back(model.biaodashis.at(i));
    }


    Relationships totalRelationships;

    for (const auto& assignment : assignments) {
        Relationships result = assignRelationship(assignment);
        // 合并结果
        for (const auto& pair : result.predecessors) {
            totalRelationships.predecessors[pair.first].insert(totalRelationships.predecessors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
        for (const auto& pair : result.successors) {
            totalRelationships.successors[pair.first].insert(totalRelationships.successors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
    }
    std::ofstream outFile("output.txt"); // 创建并打开文件用于写操作

    //文件内容为节点及其后继节点，如i k l，表示i的后继节点为k l 
    for (const auto& pair : totalRelationships.predecessors) {
        outFile << pair.first << " ";
        for (const auto& pred : pair.second) {
            outFile << pred << " ";
        }
        outFile << "\n";
    }

    outFile.close(); // 关闭文件

}


