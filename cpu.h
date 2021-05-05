#include "cpu_latches.h"
class CPU{
    public:
        latchReg IF_ID;
        latchReg ID_EX;
        latchReg EX_MEM;
        latchReg MEM_WB;

        int HAZARD = 0;
        unsigned int PreIR = 0;
        unsigned int PrePreIR = 0;
        int cnt_IR = 0;

        int IF(unsigned char *IMem);
        int ID();
        int EX();
        int MEM(unsigned char *DMem);
        int WB();
        void UpdateLatch(latchReg &pre, latchReg &next);
        unsigned int Readmemory(unsigned char *memory, int addr);
        void Writememory(unsigned char *memory, unsigned int val);

        int Check_Hazard(unsigned int previous, unsigned int current);
        void DumpReg();
        void DumpLatch();
        void DumpMem(unsigned char *memory);

};
