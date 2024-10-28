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
    std::string blifFilename ;
    const std::string verilogFilename = "example.v";
    cout << "Enter Blif File Name:";
    cin >> blifFilename;

    // ��ȡBLIF�ļ�
    Model model = readBlifFile(blifFilename);
    // ��BLIFת��ΪVerilog
    writeVerilogFile(model, verilogFilename);
    Model model2 = getGate(model);

    
    

    //std::cout << "Conversion complete. Verilog file written to " << verilogFilename << std::endl;

    cout << "ASAP: " << endl;
    print(model);
    printGateASAP(model2);
    cout << "ALAP: " << endl;
    print(model);
    printGateALAP(model2);
    cout << "MR_LCS: " << endl;
    printGateMR_LCS(model2);

    


    return 0;
}