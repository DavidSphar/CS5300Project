/*
 * CodeGenerator.h
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */

#ifndef CODEGENERATOR_H_
#define CODEGENERATOR_H_

#include <memory>
#include <vector>
#include <string>
#include "SymbolTable.h"
#include "LValue.h"
#include "LabelManager.h"


class CodeGenerator {
public:
	static std::shared_ptr<SymbolTable> getSymbolTable();
	static std::shared_ptr<LabelManager> getLabelManager();
    
    static std::vector<std::string>* identList(std::vector<std::string>* list, char* ident);
    static Type* simpleType(char* text);
    static void addVariables(std::vector<std::string>* idList, Type* type);
    static LValue* loadLValue(char* text);
    static Expression* lvalueExpression(LValue* lvalue);
    static Expression* charExpression(char c);
    static Expression* intExpression(int value);
    static Expression* stringExpression(char *text);
    static Expression* trueExpression();
    static Expression* falseExpression();
    static Expression* eval(Expression* first, Expression* second, std::string op);
    static Expression* evalConstant(Expression* first, Expression* second, std::string op);
    static Expression* evalUnary(Expression* first, std::string op);
    static Expression* evalUnaryConstant(Expression* first, std::string op);
    static Expression* evalChr(Expression* first);
    static Expression* evalOrd(Expression* first);
    static Expression* evalPred(Expression* first);
    static Expression* evalSucc(Expression* first);
    
    static void assignment(LValue* rhs, Expression* lhs);
    static void stop();
    static void read(std::vector<LValue*>* list);
    static void write(std::vector<Expression*>* list);
    static std::vector<Expression*>* exprList(std::vector<Expression*>* list, Expression *expression);
    static std::vector<LValue*>* lvalueList(std::vector<LValue*>* list, LValue *lvalue);
    static void start();
    static void end();
    static void addConstant(char *text, Expression* expression);

    //if statement
    static void newIf();
    static void ifBranch(Expression* condition);
    static void ifBranchEnd();
    static void labelElseIfBranch();
    static void endIf();
    //while statement
    static void newLoop();
    static void loopCheck(Expression* condition, bool boolValue);
    static void endLoop();
    //repeat statement
    static void repeatCheck(Expression* condition);
    //for statement
    static LValue* setupForLoop(char*text, Expression* rhs);
    static void forToHead(LValue* lhs, Expression* rhs);
    static void forDowntoHead(LValue* lhs, Expression* rhs);
    static void endForTo(LValue* lhs);
    static void endForDownto(LValue* lhs);

    //function
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* parameter(int ref, std::vector<std::string>* identList, Type* type);
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* parameterList(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* finalList, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* list);
    static Function* createFunction(char* name, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* paramList, Type* returnType);
    static void functionForward(Function* func);
    static void functionDeclare(Function* func);
    static void endFunction();
    static Expression* functionCall(char*name, std::vector<Expression*>* expressionList);
    static void functionReturn(Expression* expression);
private:
	static std::shared_ptr<SymbolTable> symbolTable;
	static std::shared_ptr<SymbolTable> getPredefinedTable();
	static std::shared_ptr<LabelManager> labelManager;
};

#endif /* CODEGENERATOR_H_ */
