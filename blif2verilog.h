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
    std::vector<std::string> wires;
    std::vector<std::string> biaodashis;
    std::vector<char> digits;
    vector<string> and_assign;
    vector<string> or_assign;
    vector<string> not_assign;
};
std::vector<std::string> getnewnode(string a, int count_);


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

    sort(model.and_assign.begin(), model.and_assign.end());
    sort(model.or_assign.begin(), model.or_assign.end());
    sort(model.not_assign.begin(), model.not_assign.end());
    auto last2 = unique(model.and_assign.begin(), model.and_assign.end());
    auto last3 = unique(model.or_assign.begin(), model.or_assign.end());
    auto last4 = unique(model.not_assign.begin(), model.not_assign.end());
    model.and_assign.erase(last2, model.and_assign.end());
    model.or_assign.erase(last3, model.or_assign.end());
    model.not_assign.erase(last4, model.not_assign.end());
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


Model getGate(Model model) {
    std::vector<std::string> assignments;
    int count_ = 1;
    for (int i = 0; i < model.biaodashis.size(); i++) {
        std::string token;
        std::string token1;
        std::string expression;
        std::string node;
        std::vector<std::string> newnode;
        int count = -1;
        if (model.biaodashis.at(i).size() > 5) {
            node = model.biaodashis.at(i);
            while (node.size() > 5) {
                newnode = getnewnode(node, count_);
                if (newnode.size() == 1) {
                    //assignments.push_back(newnode.at(0));
                    break;
                }
                else {
                    token = newnode.at(1);
                    token1 = newnode.at(2);
                    std::string m;
                    m = newnode.at(3);
                    if (newnode.at(0).at(0) == '!') {
                        model.not_assign.push_back(token);
                        auto it = std::find(model.not_assign.begin(), model.not_assign.end(), m);
                        if (it != model.not_assign.end()) {
                            model.not_assign.erase(it); // ɾ���ҵ���Ԫ��  
                        }
                    }
                    else if (newnode.at(0).at(0) == '&') {
                        model.and_assign.push_back(token);
                        auto it = std::find(model.and_assign.begin(), model.and_assign.end(), m);
                        if (it != model.and_assign.end()) {
                            model.and_assign.erase(it); // ɾ���ҵ���Ԫ��  
                        }
                    }
                    else if (newnode.at(0).at(0) == '|') {
                        model.or_assign.push_back(token);
                        auto it = std::find(model.or_assign.begin(), model.or_assign.end(), m);
                        if (it != model.or_assign.end()) {
                            model.or_assign.erase(it); // ɾ���ҵ���Ԫ��  
                        }
                    }
                    assignments.push_back(token1);
                    if (newnode.at(4).at(0) == '!') {
                        model.not_assign.push_back(m);
                    }
                    else if (newnode.at(4).at(0) == '&') {
                        model.and_assign.push_back(m);
                    }
                    else if (newnode.at(4).at(0) == '|') {
                        model.or_assign.push_back(m);
                    }
                    node = newnode.at(5);
                }
                count_++;
            }
            assignments.push_back(node);
        }
        else {
            assignments.push_back(model.biaodashis.at(i));
        }
    }


    Relationships totalRelationships;

    for (const auto& assignment : assignments) {
        Relationships result = assignRelationship(assignment);
        // �ϲ����
        for (const auto& pair : result.predecessors) {
            totalRelationships.predecessors[pair.first].insert(totalRelationships.predecessors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
        for (const auto& pair : result.successors) {
            totalRelationships.successors[pair.first].insert(totalRelationships.successors[pair.first].end(), pair.second.begin(), pair.second.end());
        }
    }
    std::ofstream outFile("output.txt"); // ���������ļ�����д����

    //�ļ�����Ϊ�ڵ㼰���̽ڵ㣬��i k l����ʾi�ĺ�̽ڵ�Ϊk l 
    for (const auto& pair : totalRelationships.predecessors) {
        outFile << pair.first << " ";
        for (const auto& pred : pair.second) {
            outFile << pred << " ";
        }
        outFile << "\n";
    }

    outFile.close(); // �ر��ļ�
    sort(model.and_assign.begin(), model.and_assign.end());
    sort(model.or_assign.begin(), model.or_assign.end());
    sort(model.not_assign.begin(), model.not_assign.end());
    auto last2 = unique(model.and_assign.begin(), model.and_assign.end());
    auto last3 = unique(model.or_assign.begin(), model.or_assign.end());
    auto last4 = unique(model.not_assign.begin(), model.not_assign.end());
    model.and_assign.erase(last2, model.and_assign.end());
    model.or_assign.erase(last3, model.or_assign.end());
    model.not_assign.erase(last4, model.not_assign.end());
    return model;
}

std::vector<std::string> getnewnode(string a, int count_) {
    std::vector<std::string> newnode;
    std::string token;
    std::string token1;
    std::string expression;
    int fucount = 0;
    for (char c : a) {
        if (c == '!' || c == '&' || c == '|') fucount++;
    }
    int count = -1;
    for (char c : a) {
        count++;
        if (c == '!') {
            token = to_string(count_);
            count_++;
            token1 = token + "=" + "!" + a.at(count + 1);
            newnode.push_back("!");
            newnode.push_back(token);
            newnode.push_back(token1);
            std::string m;
            m = a.at(0);
            expression = m + "=" + token + a.substr(count + 2);
            newnode.push_back(m);
            std::string n;
            n = a.at(count + 2);
            newnode.push_back(n);
            newnode.push_back(expression);
            break;
        }
        else if (c == '&') {
            if (a.at(count + 2) == '&' && fucount == 2) {
                newnode.push_back(a);
            }
            else if (a.at(count + 1) == '!') {
                token = to_string(count_);
                count_++;
                token1 = token + "=" + "!" + a.at(count + 2);
                newnode.push_back("!");
                newnode.push_back(token);
                newnode.push_back(token1);
                std::string m;
                m = a.at(0);
                newnode.push_back(m);
                std::string n;
                n = a.at(3);
                newnode.push_back(n);
                if (count + 3 > a.size()) {
                    expression = m + "=" + a.at(2) + a.at(3) + token;
                }
                else {
                    expression = m + "=" + a.at(2) + a.at(3) + token + a.substr(count + 3);
                }
                newnode.push_back(expression);
            }
            else {
                token = to_string(count_);
                count_++;
                token1 = token + "=" + a.at(count - 1) + "&" + a.at(count + 1);
                newnode.push_back("&");
                newnode.push_back(token);
                newnode.push_back(token1);
                std::string m;
                m = a.at(0);
                newnode.push_back(m);
                expression = m + "=" + token + a.substr(count + 2);
                std::string n;
                n = a.at(count + 2);
                newnode.push_back(n);
                newnode.push_back(expression);
            }
            break;
        }
        else if (c == '|') {
            if (a.at(count + 2) == '|' && fucount == 2) {
                newnode.push_back(a);
            }
            else if (a.at(count + 1) == '!') {
                token = to_string(count_);
                count_++;
                token1 = token + "=" + "!" + a.at(count + 2);
                newnode.push_back("!");
                newnode.push_back(token);
                newnode.push_back(token1);
                std::string m;
                m = a.at(0);
                newnode.push_back(m);
                std::string n;
                n = a.at(3);
                newnode.push_back(n);
                if (count + 3 > a.size()) {
                    expression = m + "=" + a.at(2) + a.at(3) + token;
                }
                else {
                    expression = m + "=" + a.at(2) + a.at(3) + token + a.substr(count + 3);
                }
                newnode.push_back(expression);
            }
            else {
                token = to_string(count_);
                count_++;
                token1 = token + "=" + a.at(count - 1) + "|" + a.at(count + 1);
                newnode.push_back("|");
                newnode.push_back(token);
                newnode.push_back(token1);
                std::string m;
                m = a.at(0);
                newnode.push_back(m);
                expression = m + "=" + token + a.substr(count + 2);
                std::string n;
                n = a.at(count + 2);
                newnode.push_back(n);
                newnode.push_back(expression);
            }
            break;
        }
    }
    return newnode;
}