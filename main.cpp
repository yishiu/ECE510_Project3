#include <iostream>
#include <fstream>
#include <string>
#include "cpu.h"
using namespace std;

int main(){
    //Memory use "unsigned char" because some compilers default char as "signed char"
    unsigned char IMem[2048] = {0};
    unsigned char DMem[2048] = {0};
    CPU cpu; 
    //Read file to IMem
    ifstream mach_in("machinecode");
    string a;
    int idx = 0;
    while (mach_in >> a){
        IMem[idx++] = stoi(a.substr(0, 8), nullptr, 2);
        IMem[idx++] = stoi(a.substr(8, 8), nullptr, 2);
        IMem[idx++] = stoi(a.substr(16, 8), nullptr, 2);
        IMem[idx++] = stoi(a.substr(24, 8), nullptr, 2);
    }

    //Utilization
    int u1 = 0, u2 = 0, u3 = 0, u4 = 0, u5 = 0;
    int ret1, ret2, ret3, ret4, ret5;
    //Total cycle time
    int time = 0;
    int last_three = -1;

    //mode
    int mode; //0 for instruction mode, 1 for cycle mode
    char sel;
    int num;
    cout << "Instruction mode (I) or cycle mode (C): ";
    cin >> sel;
    if (sel == 'i' || sel == 'I')
        mode = 0;
    else if (sel == 'c' || sel == 'C')
        mode = 1;
    else {
        cout << "Wrong mode, END PROGRAM" << endl;
        return 0;
    }
    cout << "Number of instructions/cycles: ";
    cin >> num;

    while(1){

        if ((mode == 0 && cpu.cnt_IR >= num) || (mode == 1 && time >= num)){
            cout << "Continue execution (Y/N)? ";
            cin >> sel;
            if (sel == 'n' || sel == 'N')
                break;
            else if (sel == 'y' || sel == 'Y'){
                cpu.DumpReg();
                cpu.DumpLatch();
                cpu.DumpMem(DMem);

                printf("============================== Utilization & Time ==============================\n");
                printf("IF: #%d %f%%\n", u1, (float)u1/(float)time * 100);
                printf("ID: #%d %f%%\n", u2, (float)u2/(float)time * 100);
                printf("EX: #%d %f%%\n", u3, (float)u3/(float)time * 100);
                printf("MEM: #%d %f%%\n", u4, (float)u4/(float)time * 100);
                printf("WB: #%d %f%%\n", u5, (float)u5/(float)time * 100);
                printf("Total cycle time: %d\n", time);
                cout << "Number of instructions/cycles: ";
                int tmp;
                cin >> tmp;
                num += tmp;
            }
        }

        if (last_three >= 0)
            last_three ++;
        if (last_three > 3) break; 

        ret5 = cpu.WB(); 
        u5 += ret5;
        
        ret4 = cpu.MEM(DMem); 
        u4 += ret4;
        
        ret3 = cpu.EX(); 
        u3 += ret3;

        ret2 = cpu.ID();
        u2 += ret2;

        ret1 = cpu.IF(IMem);
        //ret1 == -1: No instructions in for IF stage, need 3 more cycles to finish the program.
        if (ret1 == -1 && last_three == -1)
            last_three = 0;
        else if (ret1 > 0)
            u1 += ret1;

        //Every loop is a cpu cycle
        time += 1;
    }
    cpu.DumpReg();
    cpu.DumpLatch();
    cpu.DumpMem(DMem);

    printf("============================== Utilization & Time ==============================\n");
    printf("IF: #%d %f%%\n", u1, (float)u1/(float)time * 100);
    printf("ID: #%d %f%%\n", u2, (float)u2/(float)time * 100);
    printf("EX: #%d %f%%\n", u3, (float)u3/(float)time * 100);
    printf("MEM: #%d %f%%\n", u4, (float)u4/(float)time * 100);
    printf("WB: #%d %f%%\n", u5, (float)u5/(float)time * 100);
    printf("Total cycle time: %d\n", time);

    return 0;
}
