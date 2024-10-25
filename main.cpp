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
#include "Hu.h"
#include "MR-LCS.h"
using namespace std;


int main() {
    const std::string blifFilename = "example.blif";
    const std::string verilogFilename = "example.v";

    // 读取BLIF文件
    Model model = readBlifFile(blifFilename);
    // 将BLIF转换为Verilog
    writeVerilogFile(model, verilogFilename);
    getGate(model);
    model=getAssign(blifFilename);

    
    

    //std::cout << "Conversion complete. Verilog file written to " << verilogFilename << std::endl;

    cout << "ASAP: " << endl;
    print(model);
    ASAPprintGate(model);
    cout << "ALAP: " << endl;
    print(model);
    printGateALAP(model);
    //cout << "Hu: " << endl;
    //HuScheduling(model);
    cout << "MR_LCS: " << endl;
    printGateMR_LCS(model);

    


    return 0;
}