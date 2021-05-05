#include <iostream> 
#include "cpu.h"
#define Getbit(val, s, k) ((1u << k) - 1) & (val >> s)
//This macro gets k bits from position s, which start from the Least Significant bit as index 0.
//The val is always (unsigned int) IR, so val >> s will do shift right logical 
using namespace std;

void CPU::UpdateLatch(latchReg &pre, latchReg &next){
    next.IR = pre.IR;
    next.data1 = pre.data1;
    next.data2 = pre.data2;
    next.Imm = pre.Imm;
    next.ALUoutput = pre.ALUoutput;
    next.MULoutput = pre.MULoutput;
    next.cond = pre.cond;
    next.LMD = pre.LMD;
    return;
}

unsigned int CPU::Readmemory(unsigned char *memory, int addr){
    return (memory[addr] << 24) + (memory[addr+1] << 16) + (memory[addr+2] << 8) + (memory[addr+3]);  
}

void CPU::Writememory(unsigned char *memory, unsigned int val){
    memory[0] = Getbit(val, 24, 8);
    memory[1] = Getbit(val, 16, 8);
    memory[2] = Getbit(val, 8, 8);
    memory[3] = Getbit(val, 0, 8);
    return ;
}

int CPU::Check_Hazard(unsigned int previous, unsigned int current){
    //Hazard will not occurs if previous is bubble.
    if (previous == 0) return 0;
    unsigned int opcode = Getbit(current, 26, 6);
    unsigned int rs = Getbit(current, 21, 5);
    unsigned int rt = Getbit(current, 16, 5);
    unsigned int rd = Getbit(current, 11, 5);
    unsigned int prev_opcode = Getbit(previous, 26, 6);
    unsigned int prev_rs = Getbit(previous, 21, 5);
    unsigned int prev_rt = Getbit(previous, 16, 5);
    unsigned int prev_rd = Getbit(previous, 11, 5);

    //data hazard: Instructions after R-format instruction
    if (prev_opcode == 0){
        unsigned int prev_funct  = Getbit(previous, 0, 6);
        if (opcode == 0){
            unsigned int funct  = Getbit(current, 0, 6);
            if (funct == 0b100000 || funct == 0b100010 || funct == 0b011000 || funct == 0b100100 || funct == 0b100101){
                //add, sub, mul, and, or
                if (prev_rd == rs || prev_rd == rt){
                    //cout << "DATA HAZARD: R - format (1)" << endl;
                    return 1;
                }
                if (prev_funct == 0b011000 && (prev_rd + 1 == rs || prev_rd + 1 == rt)){
                    //check for rd+1 for previous mul
                    //cout << "DATA HAZARD: R - format (1)" << endl;
                    return 1;
                }
            }
            else if (funct == 0b000010 || funct == 0b000000){
                //srl, sll
                if (prev_rd == rt) {
                    //cout << "DATA HAZARD: R - format (2)" << endl;
                    return 1;
                }
                if (prev_funct == 0b011000 && prev_rd + 1 == rt){
                    //check for rd+1 for previous mul
                    //cout << "DATA HAZARD: R - format (2)" << endl;
                    return 1;
                }
            }
        }
        else if (opcode == 0b001000 || opcode == 0b001100 || opcode == 0b001101 || opcode == 0b001010 || opcode == 0b001011 || opcode == 0b100011){
            //I-format immediate value: addi, andi, ori, slti, sltiu
            //lw
            if (prev_rd == rs){
                //cout << "DATA HAZARD: R - format (3)" << endl;
                return 1;
            }
            if (prev_funct == 0b011000 && prev_rd + 1 == rs){
                //check for rd+1 for previous mul
                //cout << "DATA HAZARD: R - format (3)" << endl;
                return 1;
            }
        }
        else if (opcode == 0b101011 || opcode ==  0b000100){ 
            //sw & beq
            if (prev_rd == rs || prev_rd == rt){
                //cout << "DATA HAZARD: R - format (4)" << endl;
                return 1;
            }
            if (prev_funct == 0b011000 && (prev_rd + 1 == rs || prev_rd + 1 == rt)){
                //check for rd+1 for previous mul
                //cout << "DATA HAZARD: R - format (4)" << endl;
                return 1;
            }
        }
    }
    //data hazard: Instructions after I-format instruction
    else if (prev_opcode == 0b001000 || prev_opcode == 0b001100 || prev_opcode == 0b001101 || prev_opcode == 0b001010 || prev_opcode == 0b001011 || prev_opcode == 0b100011 || prev_opcode == 0b001111){
    
        if (opcode == 0){
            unsigned int funct  = Getbit(current, 0, 6);
            if (funct == 0b100000 || funct == 0b100010 || funct == 0b011000 || funct == 0b100100 || funct == 0b100101){
                //add, sub, mul, and, or
                if (prev_rt == rs || prev_rt == rt){
                    //cout << "DATA HAZARD: I - format (1)" << endl;
                    return 1;
                }
            }
            else if (funct == 0b000010 || funct == 0b000000){
                //srl, sll
                if (prev_rt == rt) {
                    //cout << "DATA HAZARD: I - format (2)" << endl;
                    return 1;
                }
            }
        }
        else if (opcode == 0b001000 || opcode == 0b001100 || opcode == 0b001101 || opcode == 0b001010 || opcode == 0b001011 || opcode == 0b100011){
            //I-format immediate value: addi, andi, ori, slti, sltiu
            //lw
            if (prev_rt == rs){
                //cout << "DATA HAZARD: I - format (3)" << endl;
                return 1;
            }
        }
        else if (opcode == 0b101011 || opcode ==  0b000100){ 
            //sw & beq
            if (prev_rt == rs || prev_rt == rt){
                //cout << "DATA HAZARD: I - format (3)" << endl;
                return 1;
            }
        }
    }
    //control hazard
    else if (prev_opcode == 0b000100){
        //cout << "CONTROL HAZARD" << endl;
        return 1;
    }
    return 0;
}

int CPU::IF(unsigned char *IMem){
    PreIR = ID_EX.IR;
    PrePreIR = EX_MEM.IR;

    if (HAZARD > 0){
        HAZARD --;
        IF_ID.IR = 0;
        return 0;
    }

    IF_ID.IR = Readmemory(IMem, IF_ID.PC); 
    //finish all instructions
    if (IF_ID.IR == 0) return -1;

    if (Check_Hazard(PrePreIR, IF_ID.IR) == 1){
        //stall 1 bubble because of the previous of the previous instruction.
        HAZARD = 0;
        IF_ID.IR = 0;
        return 0;
    }
    if (Check_Hazard(PreIR, IF_ID.IR) == 1){
        //stall 2 bubbles because of the previous instruction.
        HAZARD = 1;
        IF_ID.IR = 0;
        return 0;
    }
    
    //beq branch
    if (MEM_WB.cond == 1){
        IF_ID.PC = MEM_WB.ALUoutput;
        IF_ID.IR = Readmemory(IMem, IF_ID.PC); 
    }
    
    IF_ID.PC += 4;
    
    return 1 ;
}

int CPU::ID(){
    //Update all IF_ID register to ID_EX register
    UpdateLatch(IF_ID, ID_EX);
    
    //Detect bubble
    if (IF_ID.IR == 0)
        return 0;


    unsigned int opcode = Getbit(IF_ID.IR, 26, 6);
    
    //Update rs/rt/td as data1/data2
    if (opcode == 0){
        //R-format instruction
        unsigned int rs  = Getbit(IF_ID.IR, 21, 5);
        ID_EX.data1 = IF_ID.Reg[rs]; 
        unsigned int rt = Getbit(IF_ID.IR, 16, 5);
        ID_EX.data2 = IF_ID.Reg[rt]; 
    }
    else {
        //I-format instruction
        unsigned int rs  = Getbit(IF_ID.IR, 21, 5);
        ID_EX.data1 = IF_ID.Reg[rs]; 
        unsigned int rt = Getbit(IF_ID.IR, 16, 5);
        ID_EX.data2 = IF_ID.Reg[rt]; 
    }

    //Update Immdiate value of different instruction
    switch (opcode){
        case 0: //R type format
        {
        }
        break;

        case 0b001000: //addi
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;

        }
        break;

        case 0b001100: //andi
        {
            unsigned short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will NOT do sign extension since short is unsigned.
            //Imm is unsigned int(32-bit) and immedaite is unsigned short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;

        case 0b001101: //ori
        {
            unsigned short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will NOT do sign extension since short is unsigned.
            //Imm is unsigned int(32-bit) and immedaite is unsigned short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;

        case 0b001010: //slti
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;

        case 0b001011: //sltiu
        {
            unsigned short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will NOT do sign extension since short is unsigned.
            //Imm is unsigned int(32-bit) and immedaite is unsigned short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;
        
        case 0b100011: //lw
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;

        case 0b101011: //sw
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;
        case 0b001111: //lui
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;
        
        case 0b000100: //beq
        {
            short immediate = Getbit(IF_ID.IR, 0, 16); 
            //This will do sign extension since short is default signed.
            //Imm is unsigned int(32-bit) and immedaite is short(16-bit)
            ID_EX.Imm = immediate;
        }
        break;

        default:
            cout << "default"<<endl;
        break;

    }
    return 1;
}

int CPU::EX(){
    //Update IR
    EX_MEM.IR = ID_EX.IR;
    //Update all ID_EX register to EX_MEM register
    UpdateLatch(ID_EX, EX_MEM);

    //Detect bubble
    if (ID_EX.IR == 0)
        return 0;

    unsigned int opcode = Getbit(ID_EX.IR, 26, 6);
    
    switch (opcode){
        case 0: //R type format
        {
            unsigned int shamt  = Getbit(ID_EX.IR, 6, 5);
            unsigned int funct  = Getbit(ID_EX.IR, 0, 6);
            if (funct == 0b100000){
                //add
                EX_MEM.ALUoutput = ID_EX.data1 + ID_EX.data2;
            }
            else if (funct == 0b100010){
                //sub
                EX_MEM.ALUoutput = ID_EX.data1 - ID_EX.data2;
            }
            else if (funct == 0b011000){
                //mul
                //mul will do sign extension
                EX_MEM.MULoutput = (long)(signed) (ID_EX.data1) * (long)(signed) (ID_EX.data2);
            }
            else if (funct == 0b100100){
                //and
                EX_MEM.ALUoutput = ID_EX.data1 & ID_EX.data2;
            }
            else if (funct == 0b100101){
                //or
                EX_MEM.ALUoutput = ID_EX.data1 | ID_EX.data2;

            }
            else if (funct == 0b000000){
                //sll
                EX_MEM.ALUoutput = ID_EX.data2 << shamt;

            }
            else if (funct == 0b000010){
                //srl
                EX_MEM.ALUoutput = ID_EX.data2 >> shamt;

            }
            else { 
                cout << "func wrong" << endl;
            }
            
        }
        break;

        case 0b001000: //addi
        {
            EX_MEM.ALUoutput = ID_EX.data1 + ID_EX.Imm;
        }
        break;

        case 0b001100: //andi
        {
            EX_MEM.ALUoutput = ID_EX.data1 & ID_EX.Imm;
        }
        break;

        case 0b001101: //ori
        {
            EX_MEM.ALUoutput = ID_EX.data1 | ID_EX.Imm;
        }
        break;

        case 0b001010: //slti
        {
            if ((signed) ID_EX.data1 < (signed) ID_EX.Imm)
                EX_MEM.ALUoutput = 1;
            else
                EX_MEM.ALUoutput = 0;
        }
        break;

        case 0b001011: //sltiu
        {
            if (ID_EX.data1 < ID_EX.Imm)
                EX_MEM.ALUoutput = 1;
            else
                EX_MEM.ALUoutput = 0;
        }
        break;
        
        case 0b100011: //lw
        {
            EX_MEM.ALUoutput = ID_EX.data1 + ID_EX.Imm;
        }
        break;

        case 0b101011: //sw
        {
            EX_MEM.ALUoutput = ID_EX.data1 + ID_EX.Imm;
        }
        break;
        case 0b001111: //lui
        {
            EX_MEM.ALUoutput = ID_EX.Imm << 16;
        }
        break;
        
        case 0b000100: //beq
        {
            EX_MEM.ALUoutput = ID_EX.PC + (ID_EX.Imm << 2);
            if (ID_EX.data1 == ID_EX.data2)
                EX_MEM.cond = 1;
        }
        break;

        default:
            cout << "default"<<endl;
        break;
    }

    return 1;
}

int CPU::MEM(unsigned char *DMem){
    //Update all EX_MEM register to MEM_WB register
    UpdateLatch(EX_MEM, MEM_WB);

    //Detect bubble
    if (EX_MEM.IR == 0)
        return 0;

    //only lw, sw, lui will be used in this stage
    unsigned int opcode = Getbit(EX_MEM.IR, 26, 6);
    if (opcode == 0b100011){
        //lw
        //LMD will be used in the WB stage
        MEM_WB.LMD = Readmemory(DMem, EX_MEM.ALUoutput);
        return 1;
    }
    else if (opcode == 0b101011){
        //sw
        //Writememory here
        Writememory(&DMem[EX_MEM.ALUoutput], EX_MEM.data2);
        return 1;
    }
    else if (opcode == 0b001111){
        //lui
        //LMD will be used in the WB stage
        MEM_WB.LMD = EX_MEM.ALUoutput;
        return 0;
    }

    return 0;
}
int CPU::WB(){
    
    //Detect bubble
    if (MEM_WB.IR == 0)
        return 0;
    else 
        cnt_IR += 1;
    
    //Utilized: R-format, I-format (Immediate value, lw, lui)
    //Not utilized: sw, beq
    unsigned int opcode = Getbit(MEM_WB.IR, 26, 6);

    switch (opcode){
        case 0: 
        {
            //R-format
            //add, sub, and, or, sll, srl stores ALUoutput at Reg[rd]
            //mul stores MULoutput. Lower 32-bit at Reg[rd] and upper 32-bit at Reg[rd+1]
            unsigned int shamt  = Getbit(MEM_WB.IR, 6, 5);
            unsigned int funct  = Getbit(MEM_WB.IR, 0, 6);
            unsigned int rd = Getbit(MEM_WB.IR, 11, 5);
            if (funct == 0b100000 || funct == 0b100010 || funct == 0b100100 || funct == 0b100101 || funct == 0b000000 || funct == 0b000010 ){
                //add, sub, and, or, sll, srl
                MEM_WB.Reg[rd] = MEM_WB.ALUoutput; 
                //$zero cannot be write
                MEM_WB.Reg[0] = 0;
                return 1;

            }
            else if (funct == 0b011000){
                //mul
                //MULoutput is unsigned long
                MEM_WB.Reg[rd] = (unsigned int) (MEM_WB.MULoutput); 
                MEM_WB.Reg[rd+1] = (unsigned int) (MEM_WB.MULoutput >> 32); 
                //$zero cannot be write
                MEM_WB.Reg[0] = 0;
                return 1;
            }
        }
        break;
        //I-format (Immediate value) stores ALUoutput at Reg[rt]
        //addi, andi, ori, slti, sltiu
        //I-format (lw, lui) stores LMD at Reg[rt]
        //sw will not be used in this stage

        case 0b001000: case 0b001100: case 0b001101: case 0b001010: case 0b001011:
        //addi, andi, ori, slit, sltiu 
        {
            unsigned int rt = Getbit(MEM_WB.IR, 16, 5);
            MEM_WB.Reg[rt] = MEM_WB.ALUoutput;
            //$zero cannot be write
            MEM_WB.Reg[0] = 0;
            return 1;
        }
        break;

        case 0b100011: case 0b001111: //lw and lui
        {
            unsigned int rt = Getbit(MEM_WB.IR, 16, 5);
            MEM_WB.Reg[rt] = MEM_WB.LMD;
            return 1;
        }
        break;
    }
    
    return 0;
}

//initialize static member for latchReg
unsigned int latchReg::PC = 0;
unsigned int latchReg::Reg[32] = {0};

