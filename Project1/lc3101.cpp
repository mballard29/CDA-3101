/*
 * Mason Ballard
 * written in C++
 * CDA 3101 Project 1
 * Fall 2020, Tyson
 */

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <bitset>
#include <cmath>
using namespace std;

bool isNumber(string str);
string toMC(string as, unordered_map<string, int> st, char mode, bool branch, int pc);
string toDec(string bin);

int main(int argc, char *argv[]){
    if (argc != 3){
        cout << "error: usage: "<< argv[0] <<" <assembly-code-file> <machine-code-file>" << endl;
        exit(1);
    }

    char *inFile = argv[1], *outFile = argv[2];
    ifstream in;
    ofstream out;
    in.open(inFile);
    if (!in){
        cout << "error in opening " << inFile << endl;
        exit(1);
    }
    out.open(outFile);
    if (!out){
        cout << "error in opening " << outFile << endl;
        exit(1);
    }

    unordered_map<string, int> symTable;
    int addr = 0;
    string temp;
    while (!in.eof()){
        if (in.peek() == '\t' || in.peek() == ' '){
            getline(in, temp, '\n');
            addr++;
        }
        else{
            string label;
            in >> label;
            if (symTable.find(label) == symTable.end())
                symTable.insert(pair<string, int>(label, addr));
            else
                exit(1);
            getline(in, temp, '\n');
            addr++;
        }
    }

    in.clear();
    in.close();
    in.open(inFile);

    string command, regA, regB, dest_off, mc;
    int pc = 0;
    while (!in.eof()){
        if (in.peek() != '\t' && in.peek() != ' '){
            in >> command;
        }
        in >> command;
        if (command == ".fill"){
            in >> regA;
            if (!isNumber(regA)){
                if (symTable.find(regA) != symTable.end()){
                    out << symTable.find(regA)->second << endl;
                    pc++;
                }
                else
                    exit(1);
            }
            else
                out << regA << endl;
        }

        else if (command == "halt" || command == "noop"){
            if (command == "halt"){
                out << 25165824 << endl;
                pc++;
            }
            else{
                out << 29360128 << endl;
                pc++;
            }
        }
        else if (command == "lw" || command == "sw" || command == "beq" || command == "jalr" || command == "add" || command == "nand"){
            in >> regA >> regB >> dest_off;
            if (!(isNumber(regA)) | !(isNumber(regB))){
                exit(1);
            }
            if (command == "jalr"){
                mc = "101";
                mc.append(toMC(regA, symTable, 'r', false, pc));
                mc.append(toMC(regB, symTable, 'r', false, pc));
                mc.append("0000000000000000");
                out << toDec(mc) << endl;
                pc++;
            }
            else if (command == "lw" || command == "sw" || command == "beq"){
                if (command == "lw")
                    mc = "10";
                else if (command == "sw")
                    mc = "11";
                else if (command == "beq")
                    mc = "100";
                mc.append(toMC(regA, symTable, 'r', false, pc));
                mc.append(toMC(regB, symTable, 'r', false, pc));
                if (command == "beq"){
                    mc.append(toMC(dest_off, symTable, 'f', true, pc));
                }
                else{
                    mc.append(toMC(dest_off, symTable, 'f', false, pc));
                }
                out << toDec(mc) << endl;
                pc++;
            }
            else if (command == "add" || command == "nand"){
                if (command == "nand")
                    mc = "1";
                mc.append(toMC(regA, symTable, 'r', false, pc));
                mc.append(toMC(regB, symTable, 'r', false, pc));
                mc.append("0000000000000");
                mc.append(toMC(dest_off, symTable, 'r', false, pc));
                out << toDec(mc) << endl;
                pc++;
            }
            else    // will catch stray trailing characters and non-instructions
                exit(1);
        }
        mc.clear();
        if (in.peek() == '\n') in.get();
    }

    out.close();
}

bool isNumber(string str){
    for (int i = 0; i < str.length(); i++){
        if (str[0] == '-'){
            i++;
        }
        if (!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

string toMC(string as, unordered_map<string, int> st, char mode, bool branch, int pc){
    int bin;
    if (isNumber(as)){
        if (mode == 'r'){
            bin = stoi(as);
            return bitset<3>(bin).to_string();
        }
        if (mode == 'f'){
            bin = stoi(as);
            if (bin < -32768 || bin > 32767)
                exit(1);
            return bitset<16>(bin).to_string();
        }
    }
    if (st.find(as) != st.end()) {
        bin = st.find(as)->second;
        if (branch == true){
            bin = bin - (pc + 1);
        }
    }
    else
        exit(1);
    if (mode == 'r'){
        return bitset<3>(bin).to_string();
    }
    if (mode == 'f'){
        if (bin < -32768 || bin > 32767)
            exit(1);
        return bitset<16>(bin).to_string();
    }

    exit(1);
}

string toDec(string bin){
    int dec = 0, place = 0;
    for (string::iterator it = bin.end()-1; it >= bin.begin(); it--){
        int bit;
        if (*it == '0')
            bit = 0;
        else
            bit = 1;
        dec += bit * (pow(2, place));
        place++;
    }
    return to_string(dec);
}