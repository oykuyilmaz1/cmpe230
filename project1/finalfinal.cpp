//
// Created by Oyku on 4/28/2021.
//
#include <iostream>
#include <string>
#include <regex>
#include <queue>
#include <bits/stdc++.h>

using namespace std;
#define ERROR_STRING  "ERROR_STRING"
/// expression variables
queue<string> expresionQueue;
int nameNum = 1;
string globalVarName = "%t1";

///while variables
int whileNum = 1;
string globalWhileCondName = "whcond1";
string globalWhileBodyName = "whbody1";
string globalWhileEndName = "whend1";

///llCodeVariables
vector<string> allocateCodeStringsVector;
vector<string> initializeCodeStringsVector;
vector<string> codeStringsVector;

unordered_set<string> variableSet;

string returnTabsString(int numOfTabs){
    string result = "";
    for(int i = 0; i<numOfTabs; i++){
        result += "\t";
    }
    return result;
}

///boolean functions
/**
 *
 * @param ch char to checked
 * @return 1 if ch is '+','-','*', or '/'; 0 otherwise
 */
int isCharOperator(string ch){
    return ch == "+" || ch == "-" || ch == "*" || ch == "/";
}
/**
 *
 * @param originalVarName var name to be checked.
 * @return 1 if it is initialized before, 0 otherwise
 */
int isVariableInitialized(string originalVarName){
    return variableSet.find(originalVarName) != variableSet.end();
}
/**
 *
 * @param var variable to be checked
 * @return 1 if it is not an integer or temporary variable, 0 otherwise
 */
int isOriginalVariable(string var){
    return !isdigit(var[0]) && var.at(0) != '%';
}


///expression functions//////

/**returns and updates the temporary variable name  *
 * @return current temporary variable name
 */
string getUpdateGlobalVarName(){
    string temp = globalVarName;
    nameNum++;
    globalVarName = "%t" + to_string(nameNum);
    return temp;
}


/** turns infix string to postfix, adds the variables and operators
 * to expression queue,
 * checks for syntax errors and returns 0 if finds one.
 * @param str string to be turned into postfix
 * @return 0 if syntax error, 1 otherwise
 */
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
                expresionQueue.push(res);
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
                expresionQueue.push(res);
                stk.pop();
            }
            stk.push(c);

            isOpen = 0; isClosed = 0; isOperator = 1; isVar =0; isNumber = 0;
        }
        else if(c == '+' || c == '-'){
            while(!stk.empty() && stk.top() != '(' && stk.top() != ')'){
                string res = "";
                res += stk.top();
                expresionQueue.push(res);
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
            expresionQueue.push(res);

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
            expresionQueue.push(res);
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
        expresionQueue.push(res);
        stk.pop();
    }
    return 1;
}





/// ll code functions
/** creates a load ll code and adds it to the code strings vector
 * example: %t3 = load i32* %f1
 *
 * @param varTarget temporary variable to load the value or the original variable
 * @param varName value  of variable to load into other variable
 * @param numOfTabs number of tabs to put before the line
 */
void addLoadCodeLine(string varTarget, string varName, int numOfTabs){
    codeStringsVector.push_back(returnTabsString(numOfTabs) + varTarget + " = load i32* " + varName);
}

/** creates a operation ll code and adds it to the code strings vector
 * in the form of:
 * varTarget = op i32 var1, var2
 * @param varTarget temporary target variable to put the result in
 * @param var1 first operand or value
 * @param var2 second operand or value
 * @param op operator. can be '+', '-', '*', or '/'
 * @param numOfTabs number of tabs to put before the line
 */
void addOperationCodeLine(string varTarget, string var1, string var2, string op, int numOfTabs){
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
    codeStringsVector.push_back(returnTabsString(numOfTabs) + varTarget + " = "+ operate + " i32 " + var1 + ", " + var2);
}
/** checks if the given char is an operator,
 * i.e. if it is '+', '-', '*', or '/'
 * @param ch char to be checked
 * @return 0 if it is not an operator, 1 otherwise
 */


/** adds the allocation code to allocateCodeStringsVector in the form
 * %<originalVarName> = alloca i32
 * @param originalVarName variable to be allocated
 */
void addAllocationCodeLine(string originalVarName, int numOfTabs){
    allocateCodeStringsVector.push_back(returnTabsString(numOfTabs)+"%" + originalVarName + " = alloca i32");
}
/** adds the initialization code to initializeCodeStringsVector in the form
 * store i32 0, i32* %<originalVarName>
 * @param originalVarName variable to be initialized
 */
void addInitializationCodeLine(string originalVarName, int numOfTabs){
    initializeCodeStringsVector.push_back(returnTabsString(numOfTabs)+"store i32 0, i32* %" + originalVarName);
}
/** adds allocation and initialization codeif the variable has not been initialized before
 * @param originalVarName variable to be allocated and initialized
 */
void initVarIfNotExist(string originalVarName){
    if(isOriginalVariable(originalVarName) && !isVariableInitialized(originalVarName)){
        addAllocationCodeLine(originalVarName, 1);
        addInitializationCodeLine(originalVarName, 1);
        variableSet.insert(originalVarName);
    }
}


/** creates the necessary code for postfix expression string with necessary number of tabs.
 * @param numOfTabs number tabs to be added to the beginning of the code
 * @return returns the name of the final variable that the expression is put in
 */
string postfixToExpressionCode(int numOfTabs){
    stack<string> expressionStack;
    while(!expresionQueue.empty()){
        string next = expresionQueue.front();
        expresionQueue.pop();
        if(isCharOperator(next)){
            string var1 = expressionStack.top();
            expressionStack.pop();
            string var2 = expressionStack.top();
            expressionStack.pop();
            if(isOriginalVariable(var2)){
                var2 = "%" + var2;
                string temp = getUpdateGlobalVarName();
                addLoadCodeLine(temp, var2, numOfTabs);
                var2 = temp;
            }
            if(isOriginalVariable(var1)){
                var1 = "%" + var1;
                string temp = getUpdateGlobalVarName();
                addLoadCodeLine(temp, var1, numOfTabs);
                var1 = temp;
            }
            string result = getUpdateGlobalVarName();
            addOperationCodeLine(result, var2, var1, next, numOfTabs);
            expressionStack.push(result);
        }
        else {
            initVarIfNotExist(next);
            expressionStack.push(next);
        }
    }
    return expressionStack.top();
}
/** creates a store code in the form:
 * store i32 <varToStore>, i32* <targetVar>
 * @param targetVar variable to store the value in
 * @param varToStore value or variable to store
 * @param numOfTabs number of tabs in the beginning
 */
void addStoreCodeLine(string targetVar, string varToStore, int numOfTabs){
    codeStringsVector.push_back(returnTabsString(numOfTabs)+"store i32 "+ varToStore +", i32* " +targetVar);
}

///while functions
void getUpdateWhileName(string *whcond,string *whbody,string *whend){
    *whcond = globalWhileCondName;
    *whbody = globalWhileBodyName;
    *whend = globalWhileEndName;
    whileNum++;
    globalWhileCondName = "whcond" + to_string(whileNum);
    globalWhileBodyName = "whbody" + to_string(whileNum);
    globalWhileEndName = "whend" + to_string(whileNum);
}
int main(){

}