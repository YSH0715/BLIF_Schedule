#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "blif2verilog.h"
#include "ASAP.h"
using namespace std;


int main() {
    const std::string blifFilename = "example.blif";
    const std::string verilogFilename = "example.v";

    // 读取BLIF文件
    Model model = readBlifFile(blifFilename);

    // 将BLIF转换为Verilog
    writeVerilogFile(model, verilogFilename);

    //std::cout << "Conversion complete. Verilog file written to " << verilogFilename << std::endl;

    cout << "ASAP: " << endl;
    print(model);
    getGate(model);
    ASAPprintGate(model);




    return 0;
}
