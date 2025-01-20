# 逻辑综合中的调度方法设计
该项目为UESTC软件学院工业软件方向综设III课程项目
## 任务一：blif文件的读取
读取blif文件并转化为Verilog文件
## 任务二：基于blif的调度算法设计与实现
在读取blif文件后对其进行不同方式的调度，主要有以下几种：
1. 无约束的最小延迟ASAP (As Soon As Possible)
2. 延迟约束的最晚调度ALAP (As Late As Possible)
3. 对单一类型的Hu调度
4. 资源受限情况下的最小延迟ML-RCS (Minimize Latancy-Resource Constrain)
5. 延迟受限情况下的最小资源MR-LCS (Minimize Resource-Latancy Constrain)  

主要实现**ASAP,ALAP,ML-RCS,MR-LCS**四种调度算法。  
对于ASAP和ALAP可以直接给出调度结果。  
对于ML-RCS需要给出资源数，输出调度结果。  
对于MR-LCS需要给定延迟限制，输出调度结果。  
## 任务三：使用ILP（Integer Linear Programming）求解调度问题
手写相关算法的lp文件，使用ILP求解工具cplex进行求解，并与任务二的程序运行结果进行比较。  
该部分内容在代码中没有体现。
## 任务四：实现对给定Verilog的电路进行调度
直接读取Verilog文件，实现任务二中的不同调度方式。
