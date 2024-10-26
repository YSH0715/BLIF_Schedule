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
    std::vector<std::string> wires;
    std::vector<char> digits;
    vector<string> and_assign;
    vector<string> or_assign;
    vector<string> not_assign;
};


Model readBlifFile(const std::string& filename) {
    Model model;
    std::ifstream file(filename);
    std::string line;
    int count = 0;

    std::vector<std::string> stringArray(100);

    while (std::getline(file, line)) {

        std::istringstream iss(line);
        std::string token;
        std::string token1;
        std::string expression;


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
            if (model.names.size() > 0) {
                model.wires.push_back(model.names[model.names.size() - 1]);
            }
            model.names.clear();
            while (iss >> token) {
                model.names.push_back(token);
            }
            count = 0;
        }
        else if (token == ".end") {
        }
        else {
            count++;
            model.digits.clear();
            for (char ch : token) {
                model.digits.push_back(ch);
            }
            if (count == 1) {
                expression = model.names[model.names.size() - 1] + "=";
                for (size_t i = 0; i < model.digits.size(); ++i) {
                    if (model.digits[i] == '0') {
                        expression = expression + "!" + model.names[i];
                    }
                    else if (model.digits[i] == '1') {
                        expression = expression + model.names[i] + "&";
                    }
                }
                if (expression.back() == '&') {
                    expression.pop_back();
                }
                model.biaodashis.push_back(expression);
                for (char c : expression) {
                    if (c == '!') {
                        model.not_assign.push_back(model.names[model.names.size() - 1]);
                        break;
                    }
                    else if (c == '&') {
                        model.and_assign.push_back(model.names[model.names.size() - 1]);
                        break;
                    }
                }
            }
            else {
                token1 = "";
                expression = model.biaodashis.back();
                model.biaodashis.pop_back();
                for (size_t i = 0; i < model.digits.size(); ++i) {
                    if (model.digits[i] == '0') {
                        token1 = token1 + "!" + model.names[i];
                    }
                    else if (model.digits[i] == '1') {
                        token1 = token1 + model.names[i] + "&";
                    }
                }
                if (token1.back() == '&') {
                    token1.pop_back();
                }
                expression = expression + "|" + token1;
                model.biaodashis.push_back(expression);
                for (char c : expression) {
                    if (c == '!') {
                        model.not_assign.push_back(model.names[model.names.size() - 1]);
                        break;
                    }
                    else if (c == '&') {
                        model.and_assign.push_back(model.names[model.names.size() - 1]);
                        break;
                    }
                    else if (c == '|') {
                        model.or_assign.push_back(model.names[model.names.size() - 1]);
                        break;
                    }
                }
            }
        }
    }
    std::sort(model.names.begin(), model.names.end()); 
    auto last = std::unique(model.names.begin(), model.names.end()); 
    model.names.erase(last, model.names.end()); 

    unique(model.and_assign.begin(), model.and_assign.end());
    unique(model.or_assign.begin(), model.or_assign.end());
    unique(model.not_assign.begin(), model.not_assign.end());

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
    for (const auto& name : model.wires) {
        verilogFile << "wire " << name << ";\n";
    }
    verilogFile << "wire " << model.names[(model.names.size() - 1)] << ";\n";
    verilogFile << "\n";
    for (const auto& biaodashi : model.biaodashis) {
        verilogFile << "assign " << biaodashi << ";\n";
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


