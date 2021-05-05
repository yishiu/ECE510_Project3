#include <iostream>
#include <iomanip>
#include "cpu.h"
using namespace std;

void CPU::DumpReg(){
    //Dump register 0~31
    cout << "========================= Register output (hex format) =========================" << endl;
    for (int i = 0; i < 32; i++){
        cout << "Reg[" << setfill('0') << setw(2) << dec << i ;
        cout << "]: 0x" << setfill('0') << setw(8) << right << hex << IF_ID.Reg[i] << "\t\t";
        if (i % 4 == 3)
            cout << endl;
    }
        //printf("Reg[%d]: %x\t\tReg[%d]: %x\t\tReg[%d]: %x\t\tReg[%d]: %x\n", i, IF_ID.Reg[i], i+1, IF_ID.Reg[i+1], i+2, IF_ID.Reg[i+2], i+3, IF_ID.Reg[i+3]);
    return;
}

void CPU::DumpMem(unsigned char *memory){
    //Dump register 0~31
    cout << "========================== Memory output (hex format) ==========================" << endl;
    for (int i = 0; i < 512; i++){
        //Readmory get 4-byte every time
        unsigned int tmp = Readmemory(memory, i * 4);
        cout << "0x" << setfill('0') << setw(8) << hex << i*4 ;
        cout << ": 0x" << setfill('0') << setw(8) << right << hex << tmp << "\t\t";
        if (i % 4 == 3)
            cout << endl;
    }
    return;
}

void CPU::DumpLatch(){
    //Dump pipeline latches(IF_ID, ID_EX, EX_MEM, MEM_WB)
    cout << "========================== Pipeline latch (hex format) =========================" << endl;
    cout << "IF_ID = {" << endl;
        printf("\tIR = 0x%x,\n", IF_ID.IR);
        printf("\tdata1 = 0x%x,\n", IF_ID.data1);
        printf("\tdata2 = 0x%x,\n", IF_ID.data2);
        printf("\tImm = 0x%x,\n", IF_ID.Imm);
        printf("\tALUoutput = 0x%x,\n", IF_ID.ALUoutput);
        printf("\tMULoutput = 0x%lx,\n", IF_ID.MULoutput);
        printf("\tcond = 0x%x,\n", IF_ID.cond);
        printf("\tLMD = 0x%x\n},", IF_ID.LMD);

    cout << "ID_EX = {" << endl;
        printf("\tIR = 0x%x,\n", ID_EX.IR);
        printf("\tdata1 = 0x%x,\n", ID_EX.data1);
        printf("\tdata2 = 0x%x,\n", ID_EX.data2);
        printf("\tImm = 0x%x,\n", ID_EX.Imm);
        printf("\tALUoutput = 0x%x,\n", ID_EX.ALUoutput);
        printf("\tMULoutput = 0x%lx,\n", ID_EX.MULoutput);
        printf("\tcond = 0x%x,\n", ID_EX.cond);
        printf("\tLMD = 0x%x\n},", ID_EX.LMD);

    cout << "EX_MEM = {" << endl;
        printf("\tIR = 0x%x,\n", EX_MEM.IR);
        printf("\tdata1 = 0x%x,\n", EX_MEM.data1);
        printf("\tdata2 = 0x%x,\n", EX_MEM.data2);
        printf("\tImm = 0x%x,\n", EX_MEM.Imm);
        printf("\tALUoutput = 0x%x,\n", EX_MEM.ALUoutput);
        printf("\tMULoutput = 0x%lx,\n", EX_MEM.MULoutput);
        printf("\tcond = 0x%x,\n", EX_MEM.cond);
        printf("\tLMD = 0x%x\n},", EX_MEM.LMD);

    cout << "MEM_WB = {" << endl;
        printf("\tIR = 0x%x,\n", MEM_WB.IR);
        printf("\tdata1 = 0x%x,\n", MEM_WB.data1);
        printf("\tdata2 = 0x%x,\n", MEM_WB.data2);
        printf("\tImm = 0x%x,\n", MEM_WB.Imm);
        printf("\tALUoutput = 0x%x,\n", MEM_WB.ALUoutput);
        printf("\tMULoutput = 0x%lx,\n", MEM_WB.MULoutput);
        printf("\tcond = 0x%x,\n", MEM_WB.cond);
        printf("\tLMD = 0x%x\n}\n", MEM_WB.LMD);

    return;
}
