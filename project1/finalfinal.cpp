//
// Created by Oyku on 4/28/2021.
//
#include <iostream>
#include <string>
#include <regex>
#include <queue>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;
#define ERROR_STRING  "ERROR_STRING"
#define IF_START_STRING "IF_START_STRING"
#define CHOOSE_STRING "CHOOSE_STRING"
#define WHILE_COND_STRING "WHILE_COND_STRING"
#define PRINT_STRING "PRINT_STRING"
#define REGULAR_STRING "REGULAR_STRING"
#define EXPRESSION_STRING "EXPRESSION_STRING"

/// expression variables
queue<string> expresionQueue;
int nameNum = 1;
string globalVarName = "%t1";

///while variables
int whileNum = 1;
string globalWhileCondName = "whcond1";
string globalWhileBodyName = "whbody1";
string globalWhileEndName = "whend1";

///if variables
int ifNum = 1;
string globalIfCondName = "ifcond1";
string globalIfBodyName = "ifbody1";
string globalIfEndName = "ifend1";
///choose variables
int chooseNum = 1;
string chooseName = "%choose1";
string getUpdateChooseName(){
    string temp = chooseName;
    chooseNum++;
    chooseName = "%choose" + to_string(chooseNum);
    return temp;
}

///llCodeVariables
vector<string> allocateCodeStringsVector;
vector<string> initializeCodeStringsVector;
vector<string> codeStringsVector;
vector<string> syntaxErrorVector;

unordered_set<string> variableSet;

int isInWhile = 0;
int isInIf = 0;


string returnTabsString(int numOfTabs){
    string result = "";
    for(int i = 0; i<numOfTabs; i++){
        result += "\t";
    }
    return result;
}
string removeWhiteSpaces(const string line){
    return std::regex_replace( line, std::regex("\\s+"), "" );
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

int isValidVariable(string var){
    int started = 0, ended =0;
    for(char c:var){
        if(started){
            if(c == ' '){
                ended = 1;
                started = 0;
            }
            else if(!isalnum(c)){
                return 0;
            }
        }
        else if(ended){
            if(c != ' '){
                return 0;
            }
        }
        else{
            if(isalpha(c)){
                started = 1;
            }
            else if(c == ' '){
                continue;
            }
            else {
                return 0;
            }
        }
    }
    return 1;
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
    if(removeWhiteSpaces(str) == "$"){
        return 0;
    }
    stack<char> stk;
    stack<char> pStack;
    int i = 0;
    int isOpen = 0,isClosed = 0, isOperator = 0, isVar =0, isNumber = 0;
    int n = str.length()-1;
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
    if(isOpen || isOperator){
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


void createSyntaxErrorLines(int line){
    syntaxErrorVector.emplace_back("; ModuleID = \'mylang2ir\'");
    syntaxErrorVector.emplace_back("declare i32 @printf(i8*, ...)");
    syntaxErrorVector.emplace_back(R"(@print.str = constant [4 x i8] c"%d\0A\00")");
    syntaxErrorVector.emplace_back("define i32 @main() {");
    syntaxErrorVector.emplace_back(returnTabsString(1) + "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str, i32 0, i32 0), i32 " +
                                           to_string(line) + " )");
    syntaxErrorVector.emplace_back(returnTabsString(1)+"ret i32 0");
    syntaxErrorVector.emplace_back("}");
//    ofstream llFile("file.ll");
//    for(const string& str : syntaxErrorVector){
//        llFile << str << endl;
//    }
//    llFile.close();
//    exit(0);
}



/** creates the expression code, checks for the syntax errors. If any, program exits.
 *
 * @param expr expression to be converted
 * @param line line of code to be converted
 */
string createExpressionCode(string expr, int line){
    int isSyntaxError = !infixToPostFix(expr);
    if(isSyntaxError){
        createSyntaxErrorLines(line);
        return ERROR_STRING;
    }
    else {
        return postfixToExpressionCode(1);
    }
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
void createAssignmentCode(string targetVarOriginal, string expr, int line){
    if(isValidVariable(targetVarOriginal)){
        targetVarOriginal = removeWhiteSpaces(targetVarOriginal);
    }
    else {
        createSyntaxErrorLines(line);
    }
    string varToStore = createExpressionCode(expr, line);
    initVarIfNotExist(targetVarOriginal);
    if(isOriginalVariable(varToStore)){
        string temp = getUpdateGlobalVarName();
        addLoadCodeLine(temp, "%" + varToStore, 1);
        varToStore = temp;
    }
    addStoreCodeLine("%" + targetVarOriginal, varToStore,1);
}
///while functions
void updateWhileName(){
    whileNum++;
    globalWhileCondName = "whcond" + to_string(whileNum);
    globalWhileBodyName = "whbody" + to_string(whileNum);
    globalWhileEndName = "whend" + to_string(whileNum);
}

void createWhileConditionCode(string expr, int line){
    codeStringsVector.emplace_back(globalWhileCondName + ":");
    string result = createExpressionCode(expr, line);
    if(isOriginalVariable(result)){
        string temp = getUpdateGlobalVarName();
        addLoadCodeLine(temp, "%" + result, 1);
        result = temp;
    }
    string temp = getUpdateGlobalVarName();
    codeStringsVector.emplace_back(returnTabsString(1) + temp + " = icmp ne i32 "+ result+", 0");
    codeStringsVector.emplace_back(returnTabsString(1) + "br i1 " + temp + ", label %" + globalWhileBodyName + ", label %" + globalWhileEndName);
    codeStringsVector.emplace_back(globalWhileBodyName + ":");
}

///if functions
void updateIfName(){
    ifNum++;
    globalWhileCondName = "ifcond" + to_string(ifNum);
    globalWhileBodyName = "ifbody" + to_string(ifNum);
    globalWhileEndName = "ifend" + to_string(ifNum);
}
void createIFConditionCode(string expr, int line){
    codeStringsVector.emplace_back(globalIfCondName + ":");
    string result = createExpressionCode(expr, line);
    if(isOriginalVariable(result)){
        string temp = getUpdateGlobalVarName();
        addLoadCodeLine(temp, "%" + result, 1);
        result = temp;
    }
    string temp = getUpdateGlobalVarName();
    codeStringsVector.emplace_back(returnTabsString(1) + temp + " = icmp ne i32 "+ result+", 0");
    codeStringsVector.emplace_back(returnTabsString(1) + "br i1 " + temp + ", label %" + globalIfBodyName + ", label %" + globalIfEndName);
    codeStringsVector.emplace_back(globalIfBodyName + ":");
}

///print function
void createPrintCode(string expr, int line){
    string result = createExpressionCode(expr, line);
    if(isOriginalVariable(result)){
        string temp = getUpdateGlobalVarName();
        addLoadCodeLine(temp, "%" + result, 1);
        result = temp;
    }
    codeStringsVector.emplace_back(returnTabsString(1) + "call i32 (i8*, ...)* "
                                                         "@printf(i8* getelementptr ([4 x i8]* "
                                                         "@print.str, i32 0, i32 0), i32 "+ result +  ")");
}

///parse line function
string checkExpressionOrCondition(string str){
    if(isValidVariable(str)){
        str = removeWhiteSpaces(str);
        if(str == "if"){
            return IF_START_STRING;
        }
        else if(str == "choose"){
            return CHOOSE_STRING;
        }
        else if(str == "while"){
            return WHILE_COND_STRING;
        }
        else if(str == "print"){
            return PRINT_STRING;
        }
        else {
            return EXPRESSION_STRING;
        }
    }
    return ERROR_STRING;
}
int checkParentheses(string str){
    stack<char> stk;
    for(char c:str){
        if(c == '('){
            stk.push(c);
        }
        else if(c == ')'){
            stk.pop();
        }
    }
    return stk.empty();
}

int extractCondition(string str, string *expr){
    if(!checkParentheses(str)){
        return 0;
    }
    if(str[0] != '('){
        return 0;
    }
    int i = 1;
    int n = str.length();
    int isValid = 0;
    string temp;
    while(i<n){
        if(str[i] == '{'){
            if(removeWhiteSpaces(str.substr(i)) == "{$"){
                isValid = 1;
                break;
            }
            else{
                break;
            }
        }
        temp += str[i];
        i++;
    }
    if(!isValid){
        return isValid;
    }
    n = temp.length();
    i = n-1;
    while(i >= 0){
        if(temp[i] == ')'){
            temp.erase(i);
            break;
        }
        i--;
    }
    *expr = temp;
    return isValid;
}
int extractPrint(string str, string *expr){
    if(!checkParentheses(str)){
        return 0;
    }
    int n = str.length() -1;
    int i = n-1;
    while(i >= 0){
        if(str[i] == ')') {
            str.erase(i);
            break;
        }
        i--;
    }
    *expr = str;
    return 1;
}
//int checkLineOrder(string line, string lineBefore){
//    if(line == WHILE_COND_STRING){
//        return lineBefore != WHILE_COND_STRING || lineBefore != WHILE_BODY_STRING
//        || lineBefore != IF_START_STRING || lineBefore != IF_BODY_STRING;
//    }
//    if(line == WHILE_BODY_STRING){
//        return lineBefore == WHILE_COND_STRING || lineBefore != WHILE_BODY_STRING;
//    }
//    if(line == WHILE_END_STRING){
//        return lineBefore == WHILE_BODY_STRING;
//    }
//    if(line == IF_START_STRING){
//        return lineBefore != WHILE_COND_STRING || lineBefore != WHILE_BODY_STRING
//        || lineBefore != IF_START_STRING || lineBefore != IF_BODY_STRING;
//    }
//    if(line == IF_BODY_STRING){
//        return lineBefore == IF_START_STRING || lineBefore != IF_BODY_STRING;
//    }
//    if(line == IF_END_STRING){
//        return lineBefore == IF_BODY_STRING;
//    }
//    return 1;
//}

string returnChooseOrExpression(string cond){
    string res="";
    for(char c:cond){
        if(c == '('){
            res = checkExpressionOrCondition(res);
            if(res == CHOOSE_STRING || res == EXPRESSION_STRING){
                return  res;
            }
            else{
                return ERROR_STRING;
            }
        }
        else if(c == '$'){
            return EXPRESSION_STRING;
        }
        else {
            res += c;
        }
    }
    return ERROR_STRING;
}
string createChooseCode(string a,string b, string c, string d, int line){
    string resultVar = getUpdateGlobalVarName();
    // if all expression
    a = createExpressionCode(a, line);
    b = createExpressionCode(b, line);
    c = createExpressionCode(c, line);
    d = createExpressionCode(d, line);
    initVarIfNotExist(a);
    initVarIfNotExist(b);
    initVarIfNotExist(c);
    initVarIfNotExist(d);
    string tempA, tempB,tempC, tempD;
    tempB = getUpdateGlobalVarName();
    tempC = getUpdateGlobalVarName();
    tempD = getUpdateGlobalVarName();
    if(isOriginalVariable(a)){
        a = "%"+a;
        tempA = getUpdateGlobalVarName();
        addLoadCodeLine(tempA, a, 1);
        a = tempA;
    }

    string c1,c2,c3,c4, temp;
    c1 = getUpdateChooseName();
    c2 = getUpdateChooseName();
    c3 = getUpdateChooseName();
    c4 = getUpdateChooseName();
    //if (a == 0)
    temp = getUpdateGlobalVarName();
    codeStringsVector.emplace_back(returnTabsString(1) + temp + " = icmp eq i32 "+ a +", 0");
    codeStringsVector.emplace_back(returnTabsString(1) + "br i1 "+temp + ", label "+ c1 +" label " + c2);
    // do
    codeStringsVector.emplace_back(c1+":");
    if(isOriginalVariable(b)){
        b = "%"+b;
    }
    addLoadCodeLine(resultVar, b, 1);
    codeStringsVector.emplace_back("br label " + c4);

    //if(a>0)
    codeStringsVector.emplace_back(c2+":");
    if(isOriginalVariable(c)){
        c = "%"+c;
    }
    addLoadCodeLine(resultVar, c, 1);
    codeStringsVector.emplace_back(returnTabsString(1) + "br i1 "+temp + ", label "+ c4 +" label " + c3);
    //else
    codeStringsVector.emplace_back(c3+":");
    if(isOriginalVariable(d)){
        d = "%"+d;
    }
    addLoadCodeLine(resultVar, d, 1);
    codeStringsVector.emplace_back(c4+":");
    return resultVar;



}

//int parseAndTurnToLLCode(string line, int lineNum){
//    line+="$";
//    if(removeWhiteSpaces(line) == "$"){
//        return 1;
//    }
//    int i = 0;
//    int n = line.length();
//    string str1="",expr="", str2="",str3="";
//    int isRightP = 0, isCurly = 0;
//    while(i < n){
//        char c = line[i];
//        if(c == '$'){
//            return 0;
//        }
//        if(c == '='){
//            expr = line.substr(i+1);
//            createAssignmentCode(str1, expr,lineNum);
//            if(lineBefore == WHILE_BODY_STRING || lineBefore == WHILE_COND_STRING){
//                return WHILE_BODY_STRING;
//            }
//            if(lineBefore == IF_START_STRING || lineBefore == IF_BODY_STRING){
//                return IF_BODY_STRING;
//            }
//            return ASSIGNMENT_STRING;
//        }
//        else if(c == '('){
//            string condType = checkExpressionOrCondition(str1);
//            if(condType == IF_START_STRING){
//                str1 = line.substr(i);
//                extractCondition(str1, &expr);
//                createIFConditionCode(expr+"$", lineNum);
//                return IF_START_STRING;
//            }
//            else if(condType == CHOOSE_STRING){
//                return CHOOSE_STRING;
//            }
//            else if(condType == WHILE_COND_STRING){
//                str1 = line.substr(i);
//                extractCondition(str1, &expr);
//                createWhileConditionCode(expr+"$", lineNum);
//                return WHILE_COND_STRING;
//            }
//            else if(condType == PRINT_STRING){
//                return lineBefore;
//            }
//            else {
//                createSyntaxErrorLines(lineNum);
//            }
//        }
//        else if(c == '}'){
//            if(lineBefore == WHILE_BODY_STRING){
//                codeStringsVector.emplace_back(returnTabsString(1) + "br label %" + globalWhileCondName);
//                codeStringsVector.emplace_back(globalWhileEndName + ":");
//                updateWhileName();
//                return WHILE_END_STRING;
//            }
//            else if(lineBefore == IF_BODY_STRING){
//                codeStringsVector.emplace_back(globalIfEndName + ":");
//                updateIfName();
//                return IF_END_STRING;
//            }
//        }
//        else {
//            str1 += c;
//        }
//        i++;
//    }
//    return EMPTY_lINE_STRING;
//}


int main(){
//    createSyntaxErrorLines(100);
//    createAssignmentCode("oyku","10 + a * (b+c)$",9);
//    createPrintCode("aa + bb *c$",8);
//    createWhileConditionCode("n$",9);

//    cout<<isValidVariable("oyk*u")<<endl;
//    parseAndTurnToLLCode("a89 = ", 1);
//    string expr;
//    int v = extractCondition("((oykujykjutky)) { $", &expr);
//    cout<<expr<< " "<<v<<endl;
//    string x = parseAndTurnToLLCode("if(n)    {  ", 5, EMPTY_lINE_STRING);
//    x = parseAndTurnToLLCode(" a10 = 1 0 + o * uh  ", 6, x);
//    x = parseAndTurnToLLCode("f1 = f0 +f1  ", 7, x);
//    x = parseAndTurnToLLCode("f0 = t ", 8, x);
//    x = parseAndTurnToLLCode(" n = n - 1   ", 9, x);
//    x = parseAndTurnToLLCode(" }   ", 10, x);
//    parseAndTurnToLLCode(" a = b   ", 11, x);
    createChooseCode("a","b","c","d",1);



    cout << "-----ASSIGN------" << endl;


    for(auto line : allocateCodeStringsVector){
        cout << line << endl;
    }
    cout << "-----INIT------" << endl;

    for(auto line : initializeCodeStringsVector){
        cout << line << endl;
    }
    cout << "-----CODE------" << endl;

    for(auto line : codeStringsVector){
        cout << line << endl;
    }
    cout << "-----SYNTAX------" << endl;

    for(auto line : syntaxErrorVector){
        cout << line << endl;
    }
    return 0;
}