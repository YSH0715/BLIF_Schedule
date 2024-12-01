#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "blif2verilog.h"
#include "ASAP.h"
#include "ALAP.h"
#include "MR-LCS.h"
using namespace std;


int main() {
    std::string blifFilename;
    const std::string verilogFilename = "example.v";
    cout << "Enter Blif File Name:";
    cin >> blifFilename;

    Model model2;
    char lastChar = blifFilename.back();
    if (lastChar == 'f') {
        // 读取BLIF文件
        Model model = readBlifFile(blifFilename);
        // 将BLIF转换为Verilog
        writeVerilogFile(model, verilogFilename);
        model2 = getGate(model);
    }
    else if (lastChar == 'v') {
        Model model = readVerilogFile(blifFilename);
        model2 = getGate(model);
    }

    //std::cout << "Conversion complete. Verilog file written to " << verilogFilename << std::endl;

    cout << "ASAP: " << endl;
    //print(model);
    printGateASAP(model2);
    cout << "ALAP: " << endl;
   //print(model);
    printGateALAP(model2);
    cout << "MR_LCS: " << endl;
    printGateMR_LCS(model2);



    system("PAUSE");
    return 0;
}