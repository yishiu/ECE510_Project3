class latchReg
{
    public:
        unsigned int IR = 0;                //Instruction
        unsigned int data1 = 0;             //Reg[rs]: 1st source address
        unsigned int data2 = 0;             //Reg[rt]: 2nd source address
        unsigned int Imm = 0;               //Imm: immediate value after sign-extend
        unsigned int ALUoutput = 0;         //ALU output at EX stage
        unsigned long MULoutput = 0;        //MUL output at EX stage
        unsigned int cond = 0;              //branch condition for beq
        unsigned int LMD = 0;               //LMD: Loaded Memory Data, read in MEM stage and write in WB stage
        
        //declare Register and Program counter as static member variable since all latches use the same 
        //Each register is 8 byte long
        static unsigned int Reg[32];
        //Program counter is 8 byte long
        static unsigned int PC;

};
