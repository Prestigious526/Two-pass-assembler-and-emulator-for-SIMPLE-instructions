//Name: Pragya Mahajan
//Roll No.: 2302CS05

#include <bits/stdc++.h>
using namespace std;

map<int,string> Errors; 
//location, message and <operator
map<int,string> Warnings; 
//location, message and <operator
struct lineInfo{
	// Storing line with its program counter
	int program_counter;
	string label, mnemonic, operand, prev_operand;
};
vector<lineInfo> Lines;	

map<string,pair<string,string>> List;
//address,machine_code,statement
vector<string> MachineCodes;					
// Machines codes 

void storing_error(int locctr, string err) {
	Errors[locctr]=err;
}

void pushWarnings(int locctr, string err) {
	Warnings[locctr]=err;
}

map<string, pair<int, int> > symbol_table;		//label, address and line_number
vector<pair<string, pair<string, int>>> opcode_table;		//mnemonic, opcode, type of operand
map<string, vector<int> > label_location; // labels and their all locations
vector<pair<string, string>> set_variables; 		//labels and associated values
void make_opcode_table() {
opcode_table = 
	{{"data",{"",1}}, 
	{"ldc",{"00",1}}, 
	{"adc",{"01",1}}, 
	{"ldl",{"02",2}},                           	// 	0 : nothing required
	{"stl", {"03", 2}},                             // 	1 : value required
	{"ldnl", {"04", 2}},                            	// 	2 : offset required
	{"stnl", {"05", 2}}, 
	{"add", {"06", 0}},
	{"sub", {"07", 0}}, 
	{"shl", {"08", 0}}, 
	{"shr", {"09", 0}}, 
	{"adj", {"0A", 1}},
	{"a2sp", {"0B", 0}}, 
	{"sp2a", {"0C", 0}}, 
	{"call", {"0D", 2}}, 
	{"return", {"0E", 0}},
	{"brz", {"0F", 2}}, 
	{"brlz", {"10", 2}}, 
	{"br", {"11", 2}}, 
	{"HALT", {"12", 0}},
	{"SET", {"", 1}}};
}

bool is_label(string label){
    return label.back()==':';
}

vector<string> temp; //tomporary storage for tokens

/*void extractLabel(string token) {
	if(is_label(token)==1){
		token.pop_back();
		temp.push_back(token);
	}
}*/

// Function to extract and store the label 
void extractLabel(string token){
		auto i =token.find(':');
		if (i != string::npos and token.back() != ':') {		// case when there is no separation between ':' and the statement
			temp.push_back(token.substr(0, i + 1));
			token=token.substr(i + 1);
		}
}
// Function to extract mnemonics 
void extractMnemonic(string token) {
    string mnemonic = token;
    if (mnemonic.back() == ';') {  // Handle cases where mnemonic is directly followed by `;`
        mnemonic.pop_back();        
    }
    temp.push_back(mnemonic);
}

// Function to remove comments
string remove_comment(string current_line, int line_number) {
    string cleaned_line;
    for (int i=0; i<current_line.length(); i++) {
        if (current_line[i]==';') {
            break;  // Stop at the comment symbol ';'
        }
        cleaned_line+=current_line[i];
    }
    return cleaned_line;
}

vector<string> extract_tokens(string current_line, int line_number) {
    temp.clear(); // Clear `temp` for each new line
	remove_comment(current_line, line_number);  
    if(current_line.empty()){
        return temp;
    }
	stringstream current(current_line);
    string token;

    while (current >> token) {
        if (token.empty()) {
            continue;
        }
        if (token[0]==';') {
            break;  // Stop processing tokens if comment starts
        }

        extractLabel(token);       
        extractMnemonic(token);   
    }
	return temp;
}



bool is_digit(char c) {                 
	//check if character is a digit
	return (c>='0' && c<='9');
}

bool is_letter(char ch){
    //check if it is a letter
    return (ch>='A' && ch<='Z') || (ch>='a' && ch<='z');
}

bool is_alnum(char ch){
	return is_letter(ch) || is_digit(ch);
}

bool valid_label_name(string label) {
    if(label.empty() || !is_letter(label[0])){   
        //if first letter is not alphabet, invalid label name
        return false;
    }
    for(int i=1; i<label.length()-1; i++){
        //all letters need to be alphanumeric
        if(!is_alnum(label[i])) {
            return false;
        }
    }
    return true;
}

void process_label(string label, int line_number, int pc) {
	if (label.empty()){
		return;
	}
	bool test=valid_label_name(label);
	if(!test){
		Errors[line_number]= "Bogus Label name";
	} 
	else{
		if(symbol_table.count(label) && symbol_table[label].first!=-1){
			Errors[line_number]="Duplicate label definition";
		} 
		else{
			symbol_table[label]={pc,line_number};
		}
	}
}

bool containsInstruction(string instruction) { //to check if opcode table conntains instruction
    for (auto opcode:opcode_table) {
        if (opcode.first==instruction) {
            return true;
        }
    }
    return false;
}
pair<string, int> getOpcodeInfo(string instName) {
    for (const auto& opcode : opcode_table) {
        if (opcode.first == instName) {
            return opcode.second;
        }
    }
    return {"", -1}; 
}

string remove_sign(string str){
    if(str[0]=='+' || str[0]=='-'){
		str.erase(0);
    }
	return str;
}

bool is_num(string str) {
    if(str.empty()){
        return false;
    }
    int start=0;
    //Check + or - sign
    if(str[0]=='+' || str[0]=='-'){
		str.erase(0);
    }
    // invalid if only contains sign
    //if(start==str.size()){
    //    return false;
    //}
    // Check each remaining character to ensure it's a digit
    for (int i=start; i<str.size(); i++){
        if(!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

bool is_octal(string oct_number) {
    if(oct_number.empty()){
        return false;
    }
    // Check that each character is between '0' and '7'
    for(int i=0; i<oct_number.size(); i++) {
        if (oct_number[i]<'0' || oct_number[i]>'7') {
            return false;
        }
    }
    return true;
}

bool is_hex(string hex_number) {
    if(hex_number.empty()){
        return false;
    }
	if (!isxdigit(hex_number[0])) {
        return false;
    }
	if (!isxdigit(hex_number[1]) && hex_number[1]!='x') {
        return false;
    }
    // Check each character is hexa
    for(int i=2; i<hex_number.size(); i++) {
        if (!isxdigit(hex_number[i])) {
            return false;
        }
    }
    return true;
}

string octal_dec(string num){
	//convert octal string to decimal string
    int decimalValue=0;
    int power = 0;
	int i = num.length() - 1;
    while(i--){
        // Convert each character (digit) to its integer value and multiply by the appropriate power of 8
        if (num[i]>='0' && num[i]<='7') {
            decimalValue+=(num[i]-'0') * pow(8, power);
            power++;
        } 
		else {
            // Handle invalid octal digits
            cerr << "Invalid octal number!" << endl;
            return "";
        }
    }
	// Convert the decimal value to string
    return to_string(decimalValue);
}

int hex_to_dec(char hexChar) {
    if(hexChar>='0' && hexChar<='9'){
        return hexChar-'0';
    }
    if(hexChar>='A'&& hexChar<='F'){
        return hexChar-'A'+10;
    }
    if (hexChar>='a'&& hexChar<='f'){
        return hexChar-'a'+10;
    }
    return -1; 
}

string hex_dec(string num){
    long long decimal_value=0;
    int power=0;
    for (int i=num.size()-1; i>=0; i--) {
        int digitValue=hex_to_dec(num[i]);
        if (digitValue == -1) {
            return "Invalid hexadecimal number";
        }
        decimal_value += digitValue * pow(16, power);
        power++;
    }
	return to_string(decimal_value);
}

string processOperand(string operand, int locctr) {
	string ret="";
	string now=operand, sign="";
	if (now[0] == '-' or now[0] == '+') {
		sign = now[0];
		now = now.substr(1);
	}
	ret += sign;
	if (valid_label_name(operand)) {
		if (!symbol_table.count(operand)) {
			symbol_table[operand]={-1, locctr};		// label used but wasn't declared so far...
		}
		label_location[operand].push_back(locctr);
		return operand;
	}
	if(is_num(now)){
		ret+=now;
	} 
	else if(is_octal(now)){
		ret+=octal_dec(now.substr(1));

	} 
	else if(is_hex(now)){
		ret+=hex_dec(now.substr(2));
	} 
	else{
		ret="";
	}
	return ret;
}

void processMnemonic(string instruction, string operand, int line_number, int pc, int rem, bool flag) {
	if (instruction.empty()){
		return;
	}
	if (containsInstruction(instruction)==1){
		int type=getOpcodeInfo(instruction).second;
		int isOp=!operand.empty();
		if (type > 0) {
			if(!isOp){
				Errors[line_number]= "Missing operand";
			} 
			else if(rem>0){
				Errors[line_number]="extra on end of line";
			} 
			else{
				string replaceOP = processOperand(operand,line_number);
				if(replaceOP.empty()) {
					Errors[line_number]="Invalid format: not a valid label or a number";
				} 
				else{
					operand=replaceOP;
					flag=true;
				}
			}
		}
		else if (type==0 and isOp) {
			Errors[line_number]="unexpected operand";
		} 
		else {
			flag=true;
		}
	} 
	else {
		Errors[line_number]="Bogus Mnemonic";
	}
}

//label operand errors and warnings
void lo_errors(){

	for (auto label : symbol_table) {
		if (label.second.first == -1) {
			for (auto lineNum :label_location[label.first]) {
				Errors[lineNum]="no such label";
			}
		} else if (!label_location.count(label.first)) {
			pushWarnings(label.second.second, "label declared but not used");
		}
	}
}

void pass1(const vector<string> &readLines) {
	int line_number=0, program_counter= 0;
	for(string current_line: readLines){
		auto cur=extract_tokens(current_line,line_number);
		line_number++;
		if (cur.empty()){
			continue;
		}
		string label= "", instruction= "", operand = "";
		int size=cur.size();
		int position=0;
		if (cur[position].back() == ':') {
			label=cur[position];
			label.pop_back();
			position++;
		}
		if (position<size) {
			instruction=cur[position];
			position++;
		}
		if (position<size) {
			operand=cur[position];
			position++;
		}
		process_label(label,line_number,program_counter);
		bool flag=false;
		int rem=size-position;

		processMnemonic(instruction, operand, line_number, program_counter, rem, flag);

		if (containsInstruction(instruction)==1){
			int type=getOpcodeInfo(instruction).second;
			int isOp=!operand.empty();
			if (type > 0) {
				if(isOp && rem<=0){
					string replaceOP = processOperand(operand,line_number);
					if(!replaceOP.empty()) {
						flag=true;
					} 
				}
			}
			else if (type==0 and isOp) {
			} 
			else {
				flag=true;
			}
		} 
		string prevOperand=operand;	
		Lines.push_back({program_counter, label, instruction, operand, prevOperand});
		program_counter+=flag;
		if (flag and instruction== "SET") {		
			if (label.empty()) {
				Errors[line_number]="label(or variable) name missing";
			} 
			else {
				// Storing SET instruction information
				set_variables.push_back(make_pair(label,operand));
			}
		}
	}
	lo_errors();

}




void write_error_log(const string& filename) {
    ofstream logfile(filename);  // Open the log file for writing
    if(!logfile){
        cerr<<"Unable to create log file: " <<filename << endl;
        return;
    }
	if(Errors.empty() && Warnings.empty()){
        logfile << "No errors or warnings found." << endl;
    } 
    else{
        if(!Errors.empty()){
            logfile << "Errors:" << endl;
            for (auto err:Errors) {
                logfile << err.second << " at Line " << err.first << endl;
            }
        } 
        else{
            logfile<<"No errors found."<<endl;
        }

        if(!Warnings.empty()){
            logfile << "Warnings:" << endl;
            for (auto war:Warnings) {
                logfile << war.second << " at Line " << war.first << endl;
            }
        } 
        else {
            logfile << "No warnings found." << endl;
        }
    }
    logfile.close();
    cout << "Error log has been written to " << filename << endl;
}

string dec_hex(int num) {
    string hexString;
    unsigned int value=static_cast<unsigned int>(num); // Handle potential negative values

    for (int i=0; i<8; ++i) {
        int remainder=value % 16;
        char hexChar=(remainder<10) ? ('0'+remainder) : ('A'+remainder-10);
        hexString=hexChar + hexString;
        value/=16;
    }
	return hexString;
}

int stringToInt(string numStr, bool isHex = false) {
    int value = 0;
    int base = isHex ? 16 : 10;
    int start = 0;
    bool isNegative = false;

    // Check for sign
    if (numStr[start] == '-') {
        isNegative = true;
        start++;
    } else if (numStr[start] == '+') {
        start++;
    }

    // Check for hexadecimal prefix if isHex is true
    if (isHex && numStr.substr(start, 2) == "0x") {
        start += 2;
    }

    // Convert remaining characters to integer
    for (int i = start; i < numStr.size(); ++i) {
        char ch = numStr[i];
        int digitValue;

        if (isHex && ch >= 'A' && ch <= 'F') {
            digitValue = ch - 'A' + 10;
        } else if (isHex && ch >= 'a' && ch <= 'f') {
            digitValue = ch - 'a' + 10;
        } else if (ch >= '0' && ch <= '9') {
            digitValue = ch - '0';
        } else {
            cerr << "Invalid character in numeric string: " << ch << endl;
            return -1;  // Handle invalid characters gracefully
        }

        value = value * base + digitValue;
    }

    return isNegative ? -value : value;
}




bool set_variables_count(string operand) {
    for (const auto& var : set_variables) {
        if (var.first == operand) {
            return true;
        }
    }
    return false;
}
// Generating machine codes and building list vector

string set_variable_value(string operand){
    for (const auto& var : set_variables) {
        if (var.first == operand) {  
            return var.second;      
        }
    }
    return "";  
}

// Inserting into List vector
void pushInList(int program_counter, string macCode, string label_name, string mnem, string operand){
	if (!label_name.empty()){
		label_name+= ": ";
	}
	if(!mnem.empty()){
		mnem+= " ";
	}
	string sentence=label_name + mnem + operand;
	List[dec_hex(program_counter)]={macCode, sentence};
}



void pass2() {
	for (auto curLine : Lines) {
		string label = curLine.label;
		string mnemonic = curLine.mnemonic;
		string  operand = curLine.operand;
		string prevOperand = curLine.prev_operand;
		int pctr = curLine.program_counter, type = -1;
		if (!mnemonic.empty()) {
			type =getOpcodeInfo(mnemonic).second;
		}
		string Mcode = "        ";
		if (type == 2) { 	
			// offset is required
			int offset = (symbol_table.count(operand) ? symbol_table[operand].first - (pctr + 1) : stringToInt(operand));
			Mcode = dec_hex(offset).substr(2) + getOpcodeInfo(mnemonic).first;    
		} 
		else if (type == 1 and mnemonic != "data" and mnemonic != "SET") {		
			// value is required
			int value = (symbol_table.count(operand) ? symbol_table[operand].first : stringToInt(operand));
			Mcode = dec_hex(value).substr(2) + getOpcodeInfo(mnemonic).first;    
			if (set_variables_count(operand)) {
				// if in case the operand is a variable used in SET operation
				Mcode = dec_hex(stringToInt(set_variable_value(operand))).substr(2) +getOpcodeInfo(mnemonic).first;  
			}
		} else if (type == 1 and (mnemonic == "data" || mnemonic == "SET")) {	
			// specical case for data  and SET mnemonic
			Mcode = dec_hex(stringToInt(operand));
		} else if (type == 0) {		
			// nothing is required
			Mcode = "000000" + getOpcodeInfo(mnemonic).first;
		} else {		
			// do nothing...
		}
		MachineCodes.push_back(Mcode);
		pushInList(pctr, Mcode, label, mnemonic, prevOperand);
	}
}


// Writing into listing file(.lst) and machine codes object file(.o)

void write() {
	string list_file="test4.lst";
	ofstream listoutput(list_file);
	for (auto cur : List) {
		listoutput<< cur.first << " " << cur.second.first << " " << cur.second.second << endl;
	}
	listoutput.close();
	cout << "Listing(.lst) file generated" << endl;
	string machine_file="test4.o";
	ofstream coutMCode;
	coutMCode.open(machine_file, ios::binary | ios::out);
	for (auto code : MachineCodes) {
		if (code.empty() or code == "        ") continue;
		unsigned int cur = (unsigned int)stringToInt(hex_dec(code));
		static_cast<int>(cur);
		coutMCode.write((const char*)&cur, sizeof(unsigned int));
	}
	coutMCode.close();
	cout << "Machine code object(.o) file generated" << endl;
}

int main() {
	vector<string> readLines; 		// stores each line 
	// Reading from the input file
	string filename;
	cout<<"Input file name:";
	cin>>filename;
	ifstream cinfile;
	cinfile.open(filename);
	if (cinfile.fail()) {
		cout << "Input file doesn't exit" << endl;
		exit(0);
	}
	string curLine;
	while (getline(cinfile, curLine)) {
		readLines.push_back(curLine);
	}
	cinfile.close();
	make_opcode_table();						// intialise Opcode table
	pass1(readLines);							// first pass
	write_error_log("test4.log");				// if found errors, write and terminate 
	if (Errors.empty()) {						// if no errors
		pass2();								// go for second pass
		write();								// write machine code and listing file	
	}
	//for (auto curLine : Lines) {
	//	string operand = curLine.operand;
	//	cout<<symbol_table[operand].first<<" "<<symbol_table[operand].second<<endl;
	//}
	return 0;
}