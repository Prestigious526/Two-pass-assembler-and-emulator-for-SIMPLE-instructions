//Name: Pragya Mahajan
//Roll No.: 2302CS05

#include <bits/stdc++.h>
using namespace std;

// Converts a decimal integer to hexadecimal as a string.
string dec_hex(int number) {
    if (number == 0){
        return "0";
    }
    unsigned int num = static_cast<unsigned int>(number); // Convert to unsigned for bit manipulation
    string result = "";
    while (num > 0) {
        int remainder = num % 16;
        // Convert remainder to corresponding hex character
        if (remainder < 10) {
            result += (remainder + '0'); // '0' - '9'
        } 
        else {
            result += (remainder - 10 + 'a'); // 'a' - 'f'
        }
        num /= 16;
    }
    
    // The result is in reverse order, so reverse it
    reverse(result.begin(), result.end());
    return result;
}
vector<pair<int,pair<int,int>>> memoryWrites;  //address,previous_value, current_value
vector<pair<int,int>> memoryReads;  //address,value
// Memory array and register variables
int memory[20000005];
int programCounter, stackPointer, regA, regB;
vector<int> machineCodes;

// Executes instructions stored in machineCodes and traces operations.
void executeInstructions(int size) {
    programCounter = 0;
    int instructionCount = 0;
    bool halted = false;
    bool errorFlag = false;

    while (programCounter < size) {
        int instruction = machineCodes[programCounter];
        int operation = 0;
        int offset = 0;

        // Decode operation code
        for (int i = 0; i < 8; i++) {
            if (instruction & (1 << i))
                operation += (1 << i);
        }

        // Decode offset
        for (int i = 8; i < 31; i++) {
            if (instruction & (1 << i))
                offset += (1 << (i - 8));
        }
        if (instruction & (1 << 31))
            offset -= (1 << 23);

        int previous = -1;

        if(operation==0){
            regB = regA;
            regA = offset;
            break;
        }
        if(operation==1){
            regA += offset;
            break;
        }
        if(operation==2){
            regB = regA;
            regA = memory[stackPointer + offset];
            memoryReads.push_back({stackPointer + offset, regA});
            break;
        }
        if(operation==3){
            previous = memory[stackPointer + offset];
            memory[stackPointer + offset] = regA;
            regA = regB;
            memoryWrites.push_back(make_pair(stackPointer + offset, make_pair(previous, memory[stackPointer + offset])));
            break;
        }
        if(operation==4){
            previous = regA;
            regA = memory[regA + offset];
            memoryReads.push_back({previous + offset, regA});
            break;
        }
        if(operation==5){
            previous = memory[regA + offset];
            memory[regA + offset] = regB;
            memoryWrites.push_back(make_pair(regA + offset,make_pair(previous, regB)));
            break;
        }
        if(operation==6){
            regA += regB;
            break;
        }
        if(operation==7){
            regA = regB - regA;
            break;
        }
        if(operation==12){
            regA = (regB << regA);
            break;
        }
        if(operation==9){
            regA = (regB >> regA);
            break;
        }
        if(operation==10){
            stackPointer += offset;
            break;
        }
        if(operation==11){
            stackPointer = regA;
            regA = regB;
            break;
        }
        if(operation==12){
            regB = regA;
            regA = stackPointer;
            break;
        }
        if(operation==13){
            regB = regA;
            regA = programCounter;
            programCounter += offset;
            break;
        }
        if(operation==14){
            programCounter = regA;
            regA = regB;
            break;
        }
        if(operation==15){
            if (regA == 0) {
                programCounter += offset;
            }
            break;
        }
        if(operation==16){
            if (regA < 0) {
                programCounter += offset;
            }
            break;
        }
        if(operation==17){
            programCounter += offset;
            break;
        }
        if(operation==18){
            halted = true;
            break;
        }

        programCounter++;
        cout << "PC= " <<dec_hex(programCounter) << " SP= " <<dec_hex(stackPointer) 
             << " A= " <<dec_hex(regA) << " B= " <<dec_hex(regB) << "\n";

        if (programCounter < 0 || instructionCount > (1 << 24)) {
            errorFlag = true;
            break;
        }
        instructionCount++;
        if (halted)
            break;
    }

    if (errorFlag) {
        cout << "Segmentation fault or other error\n";
        exit(0);
    }
    cout << "Total instructions executed: " << instructionCount << "\n";
}

// Displays available instructions.
void displayInstructions() {
    cout << "Opcode  Mnemonic   Operand\n";
    cout << "0       ldc        value\n";
    cout << "1       adc        value\n";
    cout << "2       ldl        value\n";
    cout << "3       stl        value\n";
    cout << "4       ldnl       value\n";
    cout << "5       stnl       value\n";
    cout << "6       add\n";
    cout << "7       sub\n";
    cout << "9       shr\n";
    cout << "10      adj        value\n";
    cout << "11      a2sp\n";
    cout << "12      sp2a\n";
    cout << "13      call       offset\n";
    cout << "14      return\n";
    cout << "15      brz        offset\n";
    cout << "16      brlz       offset\n";
    cout << "17      br         offset\n";
    cout << "18      HALT\n";
    cout << "        SET        value\n";
}

int main() {
    string fileName;
    cout << "Enter the file name to run (include extension): ";
    cin >> fileName;
    ifstream file(fileName, ios::in | ios::binary);

    unsigned int current;
    int counter = 0;

    while (file.read((char*)&current, sizeof(int))) {
        machineCodes.push_back(current);
        memory[counter++] = current;
    }

    int size = machineCodes.size();

    while (true) {
        cout << "MENU\n";
        cout << "1: Run Trace\n";
        cout << "2: Display Instructions\n";
        cout << "3: Reset Flags\n";
        cout << "4: Show Memory Before Execution\n";
        cout << "5: Show Memory After Execution\n";
        cout << "6: Show Memory Reads\n";
        cout << "7: Show Memory Writes\n";
        cout << "8: Exit\n";
        cout << "Choose an option: ";
        
        int option;
        cin >> option;
        switch(option){
        case 1:
            executeInstructions(size);
        
        case 2:
            displayInstructions();
        
        case 3:
            regA = regB = programCounter = stackPointer = 0;
        case 4:
            cout << "Memory before execution:\n";
            for (int i = 0; i < size; i += 4) {
                cout << dec_hex(i) << " ";
                for (int j = i; j < min(size, i + 4); ++j) {
                    cout << dec_hex(machineCodes[j]) << " ";
                }
                cout << "\n";
            }
        
        case 5:
            cout << "Memory after execution:\n";
            for (int i = 0; i < size; i += 4) {
                cout << dec_hex(i) << " ";
                for (int j = i; j < min(size, i + 4); ++j) {
                    cout << dec_hex(memory[j]) << " ";
                }
                cout << "\n";
            }
        
        case 6:
            for (auto& read : memoryReads) {
                cout << "Memory read at [" << dec_hex(read.first) << "] value: " << dec_hex(read.second) << "\n";
            }
        case 7:
            for (auto& write : memoryWrites) {
                cout << "Memory write at [" << dec_hex(write.first) << "] previous: " 
                     << dec_hex(write.second.first) << " current: " << dec_hex(write.second.second) << "\n";
            }
        
        case 8:
            break;
        }
    }
    
    return 0;
}
