#include<bits/stdc++.h>
using namespace std;

// demo.txt generated and clock cycles in treminal

string hexToBin(char hex) {
    switch (hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': case 'a': return "1010";
        case 'B': case 'b': return "1011";
        case 'C': case 'c': return "1100";
        case 'D': case 'd': return "1101";
        case 'E': case 'e': return "1110";
        case 'F': case 'f': return "1111";
        default: return "";
    }
}
// A function to convert an 8-bit hexadecimal number to a 32-bit binary string
string hexToBin32(string hex) {
    // Check if the input is valid
    if (hex.length() != 8) {
        //Invalid input: hexadecimal number must be 8 bits
        return "";
    }
    // Convert each hexadecimal digit to a binary nibble and concatenate them
    string bin = "";
    for (int i = 0; i < 8; i++) {
        bin += hexToBin(hex[i]);
    }
    // Pad the binary string with zeros to make it 32 bits
    while (bin.length() < 32) {
        bin = "0" + bin;
    }
    return bin;
}
int binaryToDec(string bin){
    int r=1, ret=0;
    for(int i=bin.length()-1;i>=0;i--){
        if(bin[i]=='1') ret+=r;
        r*=2;
    }return ret;
}

string extend26To32(string str){
    return "0000"+str+"00";
}


class RegisterFile{
    public:
    int * rf;
    int HO, LI;
    ofstream demo;
    /*unordered_map <string,int> regNum={
    {"$0",0}, {"$zero",0},
    {"$at",1},
    {"$v0",2}, {"%v1",3},
    {"$a0",4}, {"$a1",5}, {"$a2",6}, {"$a3",7},
    {"$t0",8}, {"$t1",9}, {"$t2",10}, {"$t3",11}, {"$t4",12}, {"$t5",13}, {"$t6",14}, {"$t7",15}, {"$t8",24}, {"$t9",25},
    {"$s0",16}, {"$s1",17}, {"$s2",18}, {"$s3",19}, {"$s4",20}, {"$s5",21}, {"$s6",22}, {"$s7",23},
    {"$k0",26},{"$k1",27},
    {"$gp",28},{"$sp",29},{"$fp",30},
    {"$ra",31}};*/
    RegisterFile(){
        rf= new int[32];
        demo.open("demo.txt");
    }
    int read_rs(string A1){ // rs--> A1
    //cout<<"rs: "<<A1<<" "<<binaryToDec(A1)<<endl;
    //cout<<"rs in: "<<binaryToDec(A1)<<endl;
        return rf[binaryToDec(A1)];
    }
    int read_rt(string A2){ // rt--> A2
    //cout<<"rt: "<<A2<<" "<<binaryToDec(A2)<<endl;
        return rf[binaryToDec(A2)];
    }
    void writeBack(string A3, int WE3){ // rd--> A3, data--> WE3 
        cout<<"rd: "<<A3<<" "<<binaryToDec(A3)<<" "<<WE3<<endl;
        rf[binaryToDec(A3)]=WE3;
    }
    void showRegisterFile(int iNum){

        demo<<"Instruction no. "<<iNum<<", Register File:"<<endl;
        demo<<"+"; for(int i=0;i<180;i++) demo<<"-"; demo<<"+\n";
        for(int i=0;i<32;i++) demo<<"|"<<setw(4)<<i; demo<<"|\n";
        demo<<"+"; for(int i=0;i<180;i++) demo<<"-"; demo<<"+\n";
        for(int i=0;i<32;i++){
            demo<<"|"<<setw(4)<<rf[i];
        }demo<<"|\n";
        demo<<"+"; for(int i=0;i<180;i++) demo<<"-"; demo<<"+\n";

    }
};


int main(){
    
    ifstream inputMachineCode("output.txt");
    ofstream demo("demo.txt");
    vector<string> INSTRUCTIONS; // Instruction Memory

    RegisterFile rf;

    int clockCycle=0;
    string instructionAddress, instruction, line;
    while(getline(inputMachineCode, line)){
        instruction=line.substr(13);
        INSTRUCTIONS.push_back(hexToBin32(instruction));
        }
    int i=0;
    int baseAddressIM=0x00400000;

    while(i<INSTRUCTIONS.size()){
        instruction=INSTRUCTIONS[i];
        cout<<i<<endl;
        //cout<<instruction<<" "<<instruction.length()<<endl;
        string opcode=instruction.substr(0,6),
            rs=instruction.substr(6,5),
            rt=instruction.substr(11,5),
            rd=instruction.substr(16,5),
            shamt=instruction.substr(21,26), 
            funct=instruction.substr(26,32),
            imm=instruction.substr(16,16),
            add_off=instruction.substr(6,26); // 26-bit address offset 

        //cout<<rs<<endl;
        if(opcode=="000000"){ // add, addu, mult, mflo, sub
            int rs_val=rf.read_rs(rs);
            int rt_val=rf.read_rs(rt);
            if(funct=="100000"){ // add
                cout<<"HELLO "<<rs_val<<" "<<rt_val<<"\n";
                rf.writeBack(rd, rs_val+rt_val);
            }

            else if(funct=="100001"){ //addu
                rf.writeBack(rd, rs_val+rt_val);
            }
            else if(funct=="100010"){ // sub
                rf.writeBack(rd, rs_val-rt_val);
            }
            else if(funct=="011000"){ // mult
                rf.LI=rs_val*rt_val;
            }
            else if(funct=="010010"){ // mflo
                rf.writeBack(rd, rf.LI);
            }

            
        }
        else if(opcode=="001000"){ // addi
            int rs_val=rf.read_rs(rs);
            int imm_val=binaryToDec(imm);
            rf.writeBack(rt, imm_val+rs_val);

        }
        else if(opcode=="001001"){// addui
            //cout<<rt<<"\n";
           int rs_val=rf.read_rs(rs);
            int imm_val=binaryToDec(imm);
            rf.writeBack(rt, imm_val+rs_val);
        }
        else if(opcode=="000010"){ // j
            rf.showRegisterFile(i);
            int jumpTo=(binaryToDec(extend26To32(add_off))-baseAddressIM)/4;
            i=jumpTo; // set instruction memory index to jump target address
            clockCycle+=5;
            continue;
        }
        else if(opcode=="000100"){ // beq
            int rs_val=rf.read_rs(rs);
            int rt_val=rf.read_rs(rt);
            int imm_val=binaryToDec(imm);
            cout<<"In beq: "<<rs_val<<" "<<rt_val<<endl;
            if(rs_val==rt_val){
                rf.showRegisterFile(i);
                i=i+1+imm_val;
                clockCycle+=5;
                continue;
            }
        }
        clockCycle+=5;
        rf.showRegisterFile(i);
        i++;
    }
    cout<<"Clock cycles taken: "<<clockCycle;
    

   
}