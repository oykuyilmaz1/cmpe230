//
// Created by Oyku on 4/26/2021.
//
#include <iostream>
#include <string>
#include <algorithm>
#include <regex>
#include <queue>

using namespace std;
queue<string> expresion;
string errorString = "ERROR";
string removeWhiteSpaces(string line){
    return std::regex_replace( line, std::regex("\\s+"), "" );
}
int nameNum = 1;
string globalVarName = "%t1";
int whileNum = 1;
string globalWhileCondName = "whcond1";
string globalWhileBodyName = "whbody1";
string globalWhileEndName = "whend1";
vector<string> codeStrings;
string createTabs(int numOfTabs){
    string result = "";
    for(int i = 0; i<numOfTabs; i++){
        result += "\t";
    }
    return result;
}
int infixToPostFix(string str){
    stack<char> stk;
    stack<char> pStack;
    int i = 0;
    int isOpen = 0,isClosed = 0, isOperator = 0, isVar =0, isNumber = 0;
    int n = str.length();
    while(i < n){
        char c = str[i];
        if(c == '('){
            if(isClosed){
                cout << "Syntax Error 1"<<endl;
                return 0;
            }
            stk.push(c);
            pStack.push(c);
            isOpen = 1; isClosed = 0; isOperator = 0; isVar =0; isNumber = 0;
        }
        else if(c == ')'){
            if(isOperator || isOpen){
                cout << "Syntax Error 2"<<endl;
                return 0;
            }
            while(!stk.empty() && stk.top() != '('){
                string res = "";
                res += stk.top();
                expresion.push(res);
                stk.pop();
            }
            stk.pop();
            pStack.pop();
            isOpen = 0; isClosed = 1; isOperator = 0; isVar =0; isNumber = 0;
        }
        else if(c == '*' || c == '/'){
            if(isOperator || isOpen){
                cout << "Syntax Error3"<<endl;
                return 0;
            }
            while (!stk.empty() && (stk.top() == '*' || stk.top() == '/')){
                string res = "";
                res += stk.top();
                expresion.push(res);
                stk.pop();
            }
            stk.push(c);

            isOpen = 0; isClosed = 0; isOperator = 1; isVar =0; isNumber = 0;
        }
        else if(c == '+' || c == '-'){
            while(!stk.empty() && stk.top() != '(' && stk.top() != ')'){
                string res = "";
                res += stk.top();
                expresion.push(res);
                stk.pop();
            }
            stk.push(c);
            if(isOperator || isOpen){
                cout << "Syntax Error4"<<endl;
                return 0;
            }
            isOpen = 0; isClosed = 0; isOperator = 1; isVar =0; isNumber = 0;
        }
        else if(isdigit(c)){
            if(isClosed || isVar || isNumber){
                cout << "Syntax Error5"<<endl;
                return 0;
            }
            string res = "";
            while(isdigit(c)){
                res += c;
                i++;
                c = str[i];
            }
            i--;
            expresion.push(res);

            isOpen = 0; isClosed = 0; isOperator = 0; isVar =0; isNumber = 1;
        }
        else if(isalpha(c)){
            if(isClosed || isNumber || isVar){
                cout << "Syntax Error6"<<endl;
                return 0;
            }
            string res = "";
            while(isalnum(c)){
                res += c;
                i++;
                c = str[i];
            }
            i--;
            expresion.push(res);
            isOpen = 0; isClosed = 0; isOperator = 0; isVar =1; isNumber = 0;
        }
        else if(c == ' '){

        }
        else {
            cout << "Syntax Error7"<<endl;
            return 0;
        }
        i++;
    }
    if(!pStack.empty()){
        cout << "Syntax Error8"<<endl;
        return 0;
    }
    while(!stk.empty()){
        string res = "";
        res += stk.top();
        expresion.push(res);
        stk.pop();
    }
    return 1;
}
string loadCodeLine(string varTarget, string varName, int numOfTabs){
    return createTabs(numOfTabs) + varTarget + " = load i32* " + varName;
}
string operationCodeLine(string varTarget, string var1, string var2, string op, int numOfTabs){
    string operate;
    if(op == "+"){
        operate = "add";
    }
    else if(op == "-"){
        operate = "sub";
    }
    else if(op == "/"){
        operate = "div";
    }
    else if(op == "*"){
        operate = "mul";
    }
    else {
        return errorString;
    }
    return createTabs(numOfTabs) + varTarget + " = "+ operate + " i32 " + var1 + ", " + var2;
}
void updateGlobalVarName(){
    nameNum++;
    globalVarName = "%t" + to_string(nameNum);
}
void updateGlobalWhileName(){
    whileNum++;
    globalWhileCondName = "whcond" + to_string(whileNum);
    globalWhileBodyName = "whbody" + to_string(whileNum);;
    globalWhileEndName = "whend" + to_string(whileNum);;
}
int isCharOperation(string ch){
    return ch == "+" || ch == "-" || ch == "*" || ch == "/";
}
string createExpressionCode(string exp, int numOfTabs){
    if(infixToPostFix(exp)){
        stack<string> expressionStack;
        while(!expresion.empty()){
            string temp = expresion.front();
            expresion.pop();
            if(isCharOperation(temp)){
                string var1 = expressionStack.top();
                expressionStack.pop();
                string var2 = expressionStack.top();
                expressionStack.pop();
                if(!isdigit(var2[0]) && var2.at(0) != '%'){
                    var2 = "%" + var2;
                    codeStrings.push_back(loadCodeLine(globalVarName, var2, numOfTabs));
                    var2 = globalVarName;
                    updateGlobalVarName();
                }
                if(!isdigit(var1[0]) && var1.at(0) != '%'){
                    var1 = "%" + var1;
                    codeStrings.push_back(loadCodeLine(globalVarName, var1, numOfTabs));
                    var1 = globalVarName;
                    updateGlobalVarName();
                }
                string result = globalVarName;
                updateGlobalVarName();
                codeStrings.push_back(operationCodeLine(result, var2, var1, temp, numOfTabs));
                expressionStack.push(result);
            }
            else {
                expressionStack.push(temp);
            }
        }
        return expressionStack.top();
    }
    else{
        return errorString;
    }
}

int createWhileConditionCode(string cond){
    string line = globalWhileCondName + ":";
    codeStrings.push_back(line);
    string result = createExpressionCode(cond, 1);
    if(result != errorString){
        line = "\t" + globalVarName + " = icmp ne i32 " + result+", 0";
        codeStrings.push_back(line);
        line = "\tbr i1 " + globalVarName + ", label " + globalWhileBodyName + ", label " + globalWhileEndName;
        codeStrings.push_back(line);
        updateGlobalVarName();
        return 1;
    }
    else {
        return 0;
    }

}
void addAssignmentCode(string targetVar, string var, int numOfTabs){
    string result = createTabs(numOfTabs);
    if(var.at(0) != '%' && !isdigit(var.at(0))){
        var = "%" + var;
        result +=  globalVarName + " load i32* " + var;
        var = globalVarName;
        codeStrings.push_back(result);
        updateGlobalVarName();
    }
    result = createTabs(numOfTabs) + "store i32 " + var + ", i32* %" + targetVar;
    codeStrings.push_back(result);
}
void createAssignmentCode(string targetVar, string exp, int numOfTabs){
    string var = createExpressionCode(exp,  numOfTabs);
    addAssignmentCode(targetVar, var, numOfTabs);
}
int createPrintCode(string exp, int numOfTabs){
    string result = createExpressionCode(exp, numOfTabs);
    if(result != errorString){
        if(result.at(0) != '%'){
            result = "%" + result;
        }
        codeStrings.push_back(loadCodeLine(globalVarName, result, numOfTabs));
        result = createTabs(numOfTabs)+"call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 "+ globalVarName +  ")";
        codeStrings.push_back(result);
        updateGlobalVarName();
        return 1;
    }
    else {
        return 0;
    }
}

int main() {
//    string exp = "aaa+bbb*(12*3-e1)*(f+g*h)-i";
//    string exp = "aa";
    createAssignmentCode("x", "15", 1);
//    createPrintCode(exp, 1);
//    infixToPostFix(exp);
//    cout << createExpressionCode(exp, 0);
//    createWhileConditionCode(exp);
//    string resString = "";
//    while(!expresion.empty()){
//        resString  = resString + expresion.front() + " ";
//        expresion.pop();
//    }
//    cout << resString << endl;
    for(auto line : codeStrings){
        cout << line << endl;
    }
    return 0;
}
