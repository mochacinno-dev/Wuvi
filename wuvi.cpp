#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>
#include <cmath>

using namespace std;

enum DataType { STRING, INTEGER, FLOAT, BOOL, NULLTYPE, CHAR, DOUBLE };

struct Variable {
    DataType type;
    variant<string, int, float, bool, char, double> value;
};

class WuviInterpreter {
private:
    map<string, Variable> variables;
    map<string, vector<string>> functions;

    DataType parseDataType(const string& token) {
        if (token == ">>+>") return STRING;
        if (token == "<<-") return INTEGER;
        if (token == ">>+>>") return FLOAT;
        if (token == "<<>>") return BOOL; // true
        if (token == ">>") return BOOL; // false (also used for type)
        if (token == "_-_") return NULLTYPE;
        if (token == "_+_") return CHAR;
        if (token == ">><<++--__") return DOUBLE;
        return NULLTYPE;
    }

    int parseBuiltin(const string& token) {
        try {
            return stoi(token);
        } catch (...) {
            return -1;
        }
    }

    void executeBuiltin(int builtin, const string& arg) {
        switch(builtin) {
            case 58: // Print
                if (variables.count(arg)) {
                    printVariable(variables[arg]);
                } else {
                    cout << arg << endl;
                }
                break;
            case 67: { // Input
                string input;
                getline(cin, input);
                Variable var;
                var.type = STRING;
                var.value = input;
                variables[arg] = var;
                break;
            }
            case 24: // +
            case 32: // -
            case 15: // /
            case 45: // *
            case 40: // ^
            case 0:  // %
                cout << "Math operations need implementation (they won't be implemented, they are just added in here for coziness)" << endl;
                break;
        }
    }

    void printVariable(const Variable& var) {
        switch(var.type) {
            case STRING:
                cout << get<string>(var.value) << endl;
                break;
            case INTEGER:
                cout << get<int>(var.value) << endl;
                break;
            case FLOAT:
                cout << get<float>(var.value) << endl;
                break;
            case DOUBLE:
                cout << get<double>(var.value) << endl;
                break;
            case BOOL:
                cout << (get<bool>(var.value) ? "<<>>" : ">>") << endl;
                break;
            case CHAR:
                cout << get<char>(var.value) << endl;
                break;
            case NULLTYPE:
                cout << "_-_" << endl;
                break;
        }
    }

    vector<string> tokenize(const string& line) {
        vector<string> tokens;
        istringstream iss(line);
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

public:
    void execute(const string& code) {
        istringstream stream(code);
        string line;
        
        while (getline(stream, line)) {
            if (line.empty()) continue;
            
            vector<string> tokens = tokenize(line);
            if (tokens.empty()) continue;

            // Check for function definition
            if (tokens[0] == "init") {
                parseFunctionDefinition(stream, tokens);
            }
            // Check for variable assignment
            else if (tokens.size() >= 3 && tokens[1] == "=") {
                string varName = tokens[0];
                DataType type = parseDataType(tokens[2]);
                
                Variable var;
                var.type = type;
                
                if (tokens.size() > 3) {
                    string valueStr = tokens[3];
                    assignValue(var, valueStr);
                }
                
                variables[varName] = var;
            }
            // Execute builtin
            else if (tokens.size() >= 2) {
                int builtin = parseBuiltin(tokens[0]);
                if (builtin != -1) {
                    string arg = tokens.size() > 1 ? tokens[1] : "";
                    executeBuiltin(builtin, arg);
                }
            }
        }
    }

    void parseFunctionDefinition(istringstream& stream, vector<string>& tokens) {
        if (tokens.size() < 5) return;
        
        string funcName = tokens[1];
        vector<string> funcBody;
        string line;
        
        while (getline(stream, line)) {
            if (line.find("end") != string::npos) break;
            funcBody.push_back(line);
        }
        
        functions[funcName] = funcBody;
    }

    void assignValue(Variable& var, const string& valueStr) {
        switch(var.type) {
            case STRING:
                var.value = valueStr;
                break;
            case INTEGER:
                var.value = stoi(valueStr);
                break;
            case FLOAT:
                var.value = stof(valueStr);
                break;
            case DOUBLE:
                var.value = stod(valueStr);
                break;
            case BOOL:
                var.value = (valueStr == "<<>>");
                break;
            case CHAR:
                var.value = valueStr[0];
                break;
            case NULLTYPE:
                break;
        }
    }
};

int main() {
    WuviInterpreter interpreter;
    
    string program = R"(
msg = >>+> Hello_World!
58 msg
x = <<- 42
58 x
flag = <<>> <<>>
58 flag
)";

    cout << "Wuvi Interpreter" << endl;
    cout << "Executing program..." << endl << endl;
    
    interpreter.execute(program);
    
    cout << endl << "Wuvin' done" << endl;
    
    return 0;
}