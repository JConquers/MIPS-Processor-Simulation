#include<bits/stdc++.h>
using namespace std;

// demoPipeline.txt generated

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
    //cout<<"rd: "<<A3<<" "<<binaryToDec(A3)<<endl;
        rf[binaryToDec(A3)]=WE3;
    }
    void showRegisterFile(int iNum){

        //demo<<"Ghoda\n";
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


class instructionModule{
    public:
    int RegWrite=0,
        MemWrite=0,
        Branch=0,
        Result=0,
        iNum;
    string RegDst="";
    string instruction, // tell EX stage what to do 
           instructionTimeline,
           opcode, rs, rt, rd, shamt, funct, imm, add_off;
    instructionModule(string instruction, int iNum){
            opcode=instruction.substr(0,6),
            rs=instruction.substr(6,5),
            rt=instruction.substr(11,5),
            rd=instruction.substr(16,5),
            shamt=instruction.substr(21,26), 
            funct=instruction.substr(26,32),
            imm=instruction.substr(16,16),
            add_off=instruction.substr(6,26); // 26-bit address offset
            this->iNum=iNum; 
            instructionTimeline="";
    }

    string IF(){
        return this->instruction;
    }

    string ID(RegisterFile * rf){ // return the name of instruction to be executed

        if(opcode=="000000"){ // add, addu, mult, mflo, sub
            int rs_val=rf->read_rs(rs);
            int rt_val=rf->read_rs(rt);
            if(funct=="100000"){ // add
                RegWrite=1;
               return instruction="add";
            }

            else if(funct=="100001"){ //addu
                RegWrite=1;
                return instruction="addu";
            }
            else if(funct=="100010"){ // sub
                RegWrite=1;
                return instruction="sub";
            }
            else if(funct=="011000"){ // mult
                return instruction="mult";
                // Result of mult is automatically pushed into HO and LI after splitting higher and lower bits.
            }
            else if(funct=="010010"){ // mflo
                RegWrite=1;
                return instruction="mflo";
            }
        }
        else if(opcode=="001000"){ // addi
            RegWrite=1;
            return instruction="addi";
        }
        else if(opcode=="001001"){// addui
            RegWrite=1;
            return instruction="addiu";
        }
        else if(opcode=="000010"){ // j
            return instruction="j";
        }
        else if(opcode=="000100"){ // beq
            return instruction="beq";
        }
    }
    void EX(RegisterFile * rf){
        if(instruction=="add"){
            //rf->writeBack(rd, rf->read_rs(rs)+rf->read_rt(rt));
            Result=rf->read_rs(rs)+rf->read_rt(rt);
            RegDst=rd;
        }
        else if(instruction=="addu"){
            //rf->writeBack(rd, rf->read_rs(rs)+rf->read_rt(rt));
            Result=rf->read_rs(rs)+rf->read_rt(rt);
            RegDst=rd;
        }
        else if(instruction=="sub"){
            //rf->writeBack(rd, rf->read_rs(rs)-rf->read_rt(rt));
            Result=(rf->read_rs(rs)) - (rf->read_rt(rt));
            RegDst=rd;
        }
        else if(instruction=="mult"){
            rf->LI=rf->read_rs(rs)*rf->read_rt(rt);
        }
        else if(instruction=="mflo"){
            Result=rf->LI;
            RegDst=rd;
        }
        else if(instruction=="addi"){
            Result=rf->read_rs(rs)+binaryToDec(imm);
            RegDst=rt;
        }
        else if(instruction=="addiu"){
            Result=rf->read_rs(rs)+binaryToDec(imm);
            RegDst=rt;
        }
        else if(instruction=="j"){

        }
        else if(instruction=="beq"){
            cout<<"Checking for beq: "<<rf->read_rs(rs)<<" "<<rf->read_rt(rt)<<endl;
            if(rf->read_rs(rs)==rf->read_rt(rt)){
                Branch=1;
            }
        }
    }
    void DM(RegisterFile * rf){
        // no instruction in this code uses lw / sw instructions.
    }
    void WB(RegisterFile *rf){
        if(RegWrite==1){
            rf->writeBack(RegDst, Result);
        }
    }

};

    

int main(){
    ifstream inputMachineCode("output.txt");
    ofstream demoPipeline("demoPipeline.txt");
    
    RegisterFile * rf=new RegisterFile(); // Universal register file

    int clockCycles=0;
    string spentClockCycles="";

    vector< instructionModule * > INSTRUCTIONS;
    vector< instructionModule * > PIPELINE(5, NULL); // 5 instruction throughput pipeline
    int occupancy=0; // total instructions in pipeline
    vector< string > PIPELINE_DIAGRAM_ROWS; // to display the pipeline diagram

        rf->showRegisterFile(00);
    string instructionAddress, instruction, line;
    int n=0; // no. of PIPELINE (0 based counting)

    while(getline(inputMachineCode, line)){
        instruction=line.substr(13);
        instructionModule * obj= new instructionModule(hexToBin32(instruction), n);
        INSTRUCTIONS.push_back(obj);
        n++;
        }

    //cout<<n<<"--\n";

    {
    PIPELINE[0]=INSTRUCTIONS[0]; // shift the next instruction in pipeline to this stage
    PIPELINE[0]->instructionTimeline=spentClockCycles + "IF  ";
    occupancy++;
    clockCycles++;
    spentClockCycles+="    ";
    }
    
    for(int i=1; occupancy!=0 ; ){

        concurrent_execution_start:
        
        cout<<i<<endl;

        clockCycles++;

        string RegDst_WB, RegDst_DM; // destination register for instruction in WB and DM phase
        int RegWrite_WB, RegWrite_DM; // respective signals for instructions in these 2 phases

        { // WB
            // operation
            PIPELINE[4]=PIPELINE[3]; // shift the next instruction in pipeline to this stage,has to be done always
            if(PIPELINE[4]!=NULL){
                RegDst_WB=PIPELINE[4]->RegDst;
                RegWrite_WB=PIPELINE[4]->RegWrite;
                PIPELINE[4]->WB(rf);
                PIPELINE[4]->instructionTimeline+="WB  ";
                PIPELINE_DIAGRAM_ROWS.push_back( PIPELINE[4]->instructionTimeline );
                rf->showRegisterFile(PIPELINE[4]->iNum);
                //cout<<PIPELINE[4]->instructionTimeline<<endl;
                occupancy--;
                cout<<"w:"<<PIPELINE[4]->instruction<<" ";
            }


        }
        { // DM
            // operation
            PIPELINE[3]=PIPELINE[2]; // shift the next instruction in pipeline to this stage,has to be done always
            if(PIPELINE[3]!=NULL){
                RegDst_DM=PIPELINE[3]->RegDst;
                RegWrite_DM=PIPELINE[3]->RegWrite;
                PIPELINE[3]->DM(rf);
                PIPELINE[3]->instructionTimeline+="DM  ";
                cout<<"m:"<<PIPELINE[3]->instruction<<" w?:"<<RegWrite_DM<<" ";
                
            }

        }
        { // EX
            // operation
            PIPELINE[2]=PIPELINE[1]; // shift the next instruction in pipeline to this stage, needs to happen regardless of anything else
            if(PIPELINE[2]!=NULL){
                PIPELINE[2]->instructionTimeline+="EX  ";
                cout<<"e:"<<PIPELINE[2]->instruction<<" ";
                // handling the need of forwarding i.e resolving data hazard
                if(RegWrite_WB==1){
                    if(RegDst_WB==PIPELINE[2]->rs) rf->writeBack(PIPELINE[2]->rs, PIPELINE[4]->Result);
                    else if(RegDst_WB==PIPELINE[2]->rt) rf->writeBack(PIPELINE[2]->rt, PIPELINE[4]->Result); 
                    //cout<<"wsd \n";                   
                }
                if(RegWrite_DM==1){
                    if(RegDst_DM==PIPELINE[2]->rs) rf->writeBack(PIPELINE[2]->rs, PIPELINE[3]->Result);
                    else if(RegDst_DM==PIPELINE[2]->rt) rf->writeBack(PIPELINE[2]->rt, PIPELINE[3]->Result);
                    cout<<"m_dep   ";                    
                }

                PIPELINE[2]->EX(rf);
                // handling the need of pipeline flush i.e. resolving control hazards
                string nameOfOperation=PIPELINE[2]->instruction;
                if(nameOfOperation=="beq"){
                    if(PIPELINE[2]->Branch==1){
                        cout<<"BRANCH\n";
                        i=PIPELINE[2]->iNum + (1+binaryToDec(PIPELINE[2]->imm)); // updating which next instruction to be fetched
                        PIPELINE[1]=NULL; // flushing the instruction in ID stage
                        PIPELINE[0]=NULL; // flushing the instruction in IF stage
                        occupancy-=1;
                        cout<<"Occupancy: "<<i<<" ";
                        spentClockCycles+="    ";
                        
                        goto concurrent_execution_start;
                    }
                }
                else if(nameOfOperation=="j"){
                        cout<<"jump ";
                        i=(binaryToDec(extend26To32(PIPELINE[2]->add_off))-0x00400000)/4; // updating which next instruction to be fetched
                        cout<<binaryToDec(extend26To32(PIPELINE[2]->add_off))<<" "<<0x00400000<<" "<<i<<endl;
                        
                        //PIPELINE[2]->instructionTimeline+="EX  ";
                        PIPELINE[1]=NULL; // flushing the instruction in ID stage
                        PIPELINE[0]=NULL; // flushing the instruction in IF stage
                        occupancy-=1;
                        spentClockCycles+="    ";
                        goto concurrent_execution_start;
                }
                // no dependency, no control hazard
                //PIPELINE[2]->EX(rf);
            }

        }

        //cout<<"farewell "<<i<<endl;
        { // ID
            // operation
            PIPELINE[1]=PIPELINE[0]; // shift the next instruction in pipeline to this stage
            if(PIPELINE[1]!=NULL){
                PIPELINE[1]->ID(rf);
                cout<<"d:"<<PIPELINE[1]->instruction<<" ";
                PIPELINE[1]->instructionTimeline+="ID  ";
            }

        }

        { // IF
            // operation
            if(i<n){
            PIPELINE[0]=INSTRUCTIONS[i]; // shift the next instruction in pipeline to this stage
            PIPELINE[0]->instructionTimeline=spentClockCycles + "IF  ";
            occupancy++;
            cout<<"i:"<<PIPELINE[0]->instruction<<"\n";
            }
            else{
                PIPELINE[0]=NULL;
            }
        }

        spentClockCycles+="    ";
        i++;
        }
        cout<<"clk cycles: "<<clockCycles;

        for(int i=0;i<PIPELINE_DIAGRAM_ROWS.size();i++){
            demoPipeline<<PIPELINE_DIAGRAM_ROWS[i]<<endl;
        }

    

}