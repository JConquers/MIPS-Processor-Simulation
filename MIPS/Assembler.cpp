// NOTE: every label should be suffixed with ':', and this should follow a space. Next instruction should be immediately
// after space, no line leave.

// output.txt generated

#include<bits/stdc++.h>
using namespace std;
string decimalToBinary(int n, int totalLength){ // returns binary string padded on left with 0s
    string str="";
    while(n>0){
        str=((n%2==0)?"0":"1")+str;
        n/=2;
    }
    while(str.length()<totalLength){
        str="0"+str;
    }
    return str;
}
int binaryToDecimal(string str){ // converts binary string to dceimal integer
    int t=1, i=str.length()-1, retval=0;
    while(i>=0){
        if(str[i]=='1') retval+=t;
        t*=2;
        i--;
    }
    return retval;
}
bool isNumeric(string str){ // check if str contains only digits 0-9 or not
    for(int i=0;i<str.length();i++){
        if((int)str[i]>=48 && (int)str[i]<=57) continue;
        else return false;
    }return true;
}
string trim(string str){ // removes trailing and leading whitespaces
    int i=0, j=str.length()-1;
    while(str[i]==' ' || str[i]=='\t') i++;
    while (str[j]==' ' || str[j]=='\t') j--;
    string retval="";
    //cout<<"INTRIM: "<<i<<" "<<j<<endl;
    for(int k=i;k<=j;k++) retval+=str[k];
    return retval;
}
string paddingHex(int n, int totalLength){ // converts decimal integer to hexadecimal and adds required padding
   string str="";
   while(n>0){
    int x=n%16;
    if(x<10) str=to_string(x)+str;
    else{
        x%=10;
        str=(char)(97+x)+str;
    }
    n/=16;
   }
   while(str.length()<totalLength) str="0"+str;
   return str;
}
string paddingHex(string str){ // converts 32 bit binary integer to hexadecimal of 8 characters. paddingHex() above
                               // will not behave as desired incase the signed bit in 32 representaion i set                    
   string retval="";
   int i=0;
   while(i<32){
    string temp=str.substr(i,4);
    retval+=paddingHex(binaryToDecimal(temp),1);
    i+=4;
   }
   return retval;
}
string trimAddressTo26Bits(string n){ // trims the address of 32 bits into 26, so that it can fit inside J-format
    string str="";
    for(int i=4;i<30;i++) str+=n[i];
    return str;
}


void showTextSegment(vector<string> &instructions){
    cout<<"TEXT SEGMENT:\n";
    cout<<"Address    Code\n";
    cout<<"---------------------\n";
    ofstream outputMachineCode("output.txt");
    int  address=0x00400000;
    for(int i=0;i<instructions.size();i++){
        string outputLine="0x"+paddingHex(address,8)+" 0x"+paddingHex(instructions[i])+"\n";
        cout<<outputLine;
        outputMachineCode<<outputLine;
        address+=4;
    }
    outputMachineCode.close();
}

int main(){

    unordered_map <string,int> registerFile={
    {"$0",0}, {"$zero",0},
    {"$at",1},
    {"$v0",2}, {"%v1",3},
    {"$a0",4}, {"$a1",5}, {"$a2",6}, {"$a3",7},
    {"$t0",8}, {"$t1",9}, {"$t2",10}, {"$t3",11}, {"$t4",12}, {"$t5",13}, {"$t6",14}, {"$t7",15}, {"$t8",24}, {"$t9",25},
    {"$s0",16}, {"$s1",17}, {"$s2",18}, {"$s3",19}, {"$s4",20}, {"$s5",21}, {"$s6",22}, {"$s7",23},
    {"$k0",26}, {"$k1",27},
    {"$gp",28}, {"$sp",29}, {"$fp",30},
    {"$ra",31}};

    vector<string> instructions;
    vector< pair<int, string> > pendingLabel; // pendingLabel aliases address
    vector< pair<int, string> > beqLabel; // for beq instructions
    unordered_map<string, int> dataSegment; // labels defined in the data segment
    unordered_map <string,int> labels; // holds labels and their line number  
    int instructionIndex=0, dataIndex=0;
    int segment=-1; // 0 means we are in data segment, 1 means text segment/ code space
    string line, fileName;

    cout<<"Enter the file name (if file not present in same directly as this assembler, then specify proper path): ";
    cin>>fileName;
    ifstream inputAssemblyCode(fileName); // name of file that  contains assembly code

    lineFetch: while(getline(inputAssemblyCode, line)){ // why lineFetch ---> could have used continue but used this for better clarity
        cout<<line<<"\n";
        line=trim(line);
    
        if(line==".data"){
            segment=0; 
            goto lineFetch;
        }
        else if(line==".text"){
            segment=1;
            goto lineFetch;
        }
        else if(segment==0){ // data segment can be currently read in the format as given in template.asm

            if(line=="") goto lineFetch;
            string word=line.substr(0,line.find(" ")); 
            word=word.substr(0,word.find(":"));
            
            dataSegment[word]=dataIndex;

            line=line.substr(line.find(" "));
            line=trim(line);
            
            string statement=line.substr(line.find("\"")+1);
            statement=statement.substr(0,statement.find("\""));
            //cout<<dataIndex<<": "<<word<<"-"<<statement<<endl;
            if(statement.length()==2 && statement[0]=='\\') dataIndex+=(statement.length());
            else dataIndex+=(statement.length())+1;

        }

        else if(segment==1){
          //cout<<line<<endl;
        
            exec:
            if(line=="") goto lineFetch;

            string word;
            string operation=line.substr(0,line.find(" ")); operation=trim(operation); 
            line=line.substr(line.find(" ")+1); line=trim(line); 


            if(operation[0]=='#'){ // '#' indicates a comment, so  ignore it
                goto lineFetch;
            } 
            else if(operation=="jal" || operation=="j" || operation=="jr" || operation=="mflo"){
                if(operation=="jr"){
                    line=line.substr(0,line.find(" ")); 
                    word="000000"+decimalToBinary(registerFile[line],5)+"000000000000000001000";
                }
                else if(operation=="j"){
                    word="000010";
                    pendingLabel.push_back({instructionIndex,line}); // address for this hasn't been put yet
                }
                else if(operation=="jal"){
                    word="000011";
                    pendingLabel.push_back({instructionIndex,line}); // address for this hasn't been put yet
                }
                else{
                    word="0000000000000000"+decimalToBinary(registerFile[line],5)+"00000010010"; // mflo-> move from lo register
                }
                instructions.push_back(word); //only six bit opcode
            }
            else if(operation=="add" || operation=="sub" || operation=="slt"){
                string a, b, c;
                    a=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                    b=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                    c=line.substr(0,line.find(" "));
                    word=decimalToBinary(registerFile[b],5)+
                         decimalToBinary(registerFile[c],5)+
                         decimalToBinary(registerFile[a],5);
                if(operation=="add") word="000000"+word+"00000100000";
                else if(operation=="sub") word="000000"+word+"00000100010";
                else if (operation=="slt") word="000000"+word+"00000101010";
                instructions.push_back(word);
            }
            else if(operation=="addi" || operation=="beq" || operation=="bne"){
                string a, b, c;
                    a=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                    b=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                    c=line.substr(0,line.find(" "));
                
                if(operation=="addi"){
                    word="001000"; // 6-bit opcode
                    word+=decimalToBinary(registerFile[b],5);// rs
                    word+=decimalToBinary(registerFile[a],5);// rt
                    word+=decimalToBinary(stoi(c),16);// imm
                    }

                else if(operation=="beq"){
                    word="000100"; // 6-bit opcode
                    word+=decimalToBinary(registerFile[a],5);// rs
                    word+=decimalToBinary(registerFile[b],5);// rt
                    if(!isNumeric(c)) beqLabel.push_back({instructionIndex,c}); // immediate field field in beq might be a label or an integer
                    else word+=decimalToBinary(stoi(c),16);
                    }
                else{ // bne
                    word="000101"; // 6-bit opcode
                    word+=decimalToBinary(registerFile[a],5);// rs
                    word+=decimalToBinary(registerFile[b],5);// rt
                    if(!isNumeric(c)) beqLabel.push_back({instructionIndex,c}); // immediate field field in beq might be a label or an integer
                    else word+=decimalToBinary(stoi(c),16);
                }
                instructions.push_back(word);
            }
            else if(operation=="syscall"){
                word="00000000000000000000000000001100";
                instructions.push_back(word);
            }
            else if(operation=="lw" || operation=="sw"){ // Eg: lw $s0,2($t0) ---> lw a,b(c)
                string a, b, c;
                a=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                b=line.substr(0,line.find("(")); line=line.substr(line.find("(")+1);line=trim(line);
                c=line.substr(0,line.find(")"));
           
                if(operation=="lw") word="100011";
                else word="101011";
                word+= decimalToBinary(registerFile[c],5);
                word+= decimalToBinary(registerFile[a],5);
                word+= decimalToBinary(stoi(b),16);
                instructions.push_back(word);
            }
            else if(operation=="move" || operation=="la" ||operation=="li" || operation=="mult"){
                /* Above 3 instructions are pseudo instructions. They are provided by the assembler, not by the ISA.
                   move $t2,$t4 ---> addu $t2,$0,$t4
                   li $v0,1 ---> addui $v0,$0,1 */
                string a, b;
                a=line.substr(0,line.find(",")); line=line.substr(line.find(",")+1); line=trim(line);
                b=line.substr(0,line.find(" "));
                //cout<<">>> "<<a<<" "<<b<<" <<<\n";
                if(operation=="move"){
                    word="000000";
                    word+=decimalToBinary(registerFile["$0"],5);
                    word+=decimalToBinary(registerFile[b],5);
                    word+=decimalToBinary(registerFile[a],5);
                    word+="00000100001";
                    instructions.push_back(word);
                }
                else if(operation=="la"){ // la $a0,inp_statement ---> implemented as lui $at, 4097; 
                                          // ori $a0,$at,(address of inp_statement^0x10010000 )
                    // NOTE: 0x10010000---> base address of data segment in MIPS memory map
                    word="001111";
                    word+="00000"+decimalToBinary(registerFile["$at"],5)+decimalToBinary(4097,16);
                    instructions.push_back(word);
                    instructionIndex++;
                    word="001101";
                    word+=decimalToBinary(registerFile["$at"],5)+decimalToBinary(registerFile[a],5);
                    // the label may be present in the data segment or in the text segment/ code space
                    if(labels.find(b)!=labels.end()) word+=decimalToBinary(labels[b],16);
                    else if(dataSegment.find(b)!=dataSegment.end()) word+=decimalToBinary(dataSegment[b],16);
                    else{
                        cout<<"Invalid label. Exit\n";
                        exit(0);
                    }
                    instructions.push_back(word);

                }
                else if(operation=="li"){ // li
                    word="001001";
                    word+=decimalToBinary(registerFile["$0"],5)+
                        decimalToBinary(registerFile[a],5)+
                        decimalToBinary(stoi(b),16);
                    instructions.push_back(word);
                }   
                else{ // mult rs,rt -> 32+32=64 bit output. 32 lower bits are put into LI nad rest in HI.
                    word="000000"+decimalToBinary(registerFile[a],5)+decimalToBinary(registerFile[b],5)+"0000000000011000";
                    instructions.push_back(word);
                }

            }
            else{
                if(operation[operation.length()-1]==':'){ // encountered a label
                    labels[operation.substr(0,operation.find(":"))]=instructionIndex;
                    cout<<">"<<line<<"<\n";
                    for(int i=0;i<line.length();i++){
                        if(line[i]!=' ' && line[i]!='\t') break;
                        else if(i==(line.length()-1)) goto lineFetch;
                    }
                    line=trim(line);
                    goto exec;
                }
                cout<<"Invalid instructions encountered. Exit.\n";
                exit(0);
            }
                    cout<<paddingHex(binaryToDecimal(word),8)<<"\n";
                

            instructionIndex++;
            }
    }

    int baseAddress=0x00400000;
    // Now entire code has been read, so now filling the addresses in label's place
    for(int i=0;i<pendingLabel.size();i++){ 
        string jumpAddress=trimAddressTo26Bits(decimalToBinary(baseAddress+ 4*labels[pendingLabel[i].second],32));
        instructions[pendingLabel[i].first]=instructions[pendingLabel[i].first]+jumpAddress;
    }
    for(int i=0;i<beqLabel.size();i++){
        string jumpAddress=decimalToBinary(labels[beqLabel[i].second]-beqLabel[i].first-1, 16);
        instructions[beqLabel[i].first]=instructions[beqLabel[i].first]+jumpAddress;
    }

    inputAssemblyCode.close();
    showTextSegment(instructions);
    return 0;
}