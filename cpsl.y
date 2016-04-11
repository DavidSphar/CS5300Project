/* 
 * CS 5300 Compiler Construction - Spring 2016
 * The syntax analyzer for CPSL
 * Author: Tam Nguyen
 * Email: tamnguyenthe91@gmail.com
 * A-number: A02056664
 */


%{
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <utility>
#include "Expression.h"
#include "CodeGenerator.h"
#include "LValue.h"

using namespace std;

extern int yylex();
extern int yylineno; 
void yyerror(const char *msg);

ofstream fout;

%}

%union {
    int intVal;
    char *strVal;
    char charVal;
    std::vector<std::string> *identList;
    std::vector<Expression*> *exprList;
    std::vector<LValue*> *lvalueList;
    Expression *expr;
    Type *typeVal;
    LValue *lvalue;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>>* paramList;
}

%error-verbose
%token ARRAY
%token ELSE
%token IF
%token RECORD
%token THEN
%token WRITE
%token BEGIN_
%token ELSEIF
%token OF
%token REF
%token TO
%token CHR
%token END
%token ORD
%token REPEAT
%token TYPE
%token CONST
%token FOR
%token PRED
%token RETURN
%token UNTIL
%token DO
%token FORWARD
%token PROCEDURE
%token STOP
%token VAR
%token DOWNTO
%token FUNCTION
%token READ
%token SUCC
%token WHILE
%token IDENTIFIER
%token LEQ
%token GEQ
%token NEQ
%token ASSIGN
%token PLUS
%token MINUS
%token MULT
%token DIV
%token AND
%token OR
%token NOT
%token EQ
%token GT
%token LT
%token MOD
%token DOT
%token COMMA
%token COLON
%token SEMICOLON
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token INTEGER
%token CHARACTER
%token STRING
%token TRUE_LIT
%token FALSE_LIT

%left AND OR
%right NOT
%nonassoc EQ LEQ GEQ GT LT NEQ
%left PLUS MINUS
%left DIV MOD MULT
%right UMINUS


%type <intVal> INTEGER AllVar
%type <charVal> CHARACTER
%type <strVal> STRING IDENTIFIER
%type <identList> IdentList
%type <exprList> WriteArgs AllArguments Arguments
%type <lvalueList> ReadArgs
%type <expr> Expression FunctionCall ProcedureCall
%type <typeVal> Type SimpleType
%type <lvalue> LValue ForStart ForToHead ForDowntoHead 
%type <paramList> FormalParameter FormalParameters AllFormalParameters

%%

Program : ProgramStart Block DOT { CodeGenerator::end();}
		;
		
ProgramStart : AllConstantDecls AllTypeDecls AllVarDecls PFDecls { CodeGenerator::start(); }
			 ;

AllConstantDecls : CONST ConstantDecls
			  	 |
			  	 ;

ConstantDecls : ConstantDecls ConstantDecl
		   	  | ConstantDecl
		   	  ;

ConstantDecl : IDENTIFIER EQ Expression SEMICOLON { CodeGenerator::addConstant($1, $3); }
		  ;

PFDecls : PFDecls ProcedureDecl
        | PFDecls FunctionDecl
		|
		;

ProcedureDecl : PROCEDURE IDENTIFIER LPAREN AllFormalParameters RPAREN SEMICOLON FORWARD SEMICOLON { Function* func = CodeGenerator::createFunction($2, $4, nullptr);  CodeGenerator::functionForward(func); }
			  | ProcedureStart Body SEMICOLON { CodeGenerator::endFunction(); }
			  ;

ProcedureStart : PROCEDURE IDENTIFIER LPAREN AllFormalParameters RPAREN SEMICOLON { Function* func = CodeGenerator::createFunction($2, $4, nullptr); CodeGenerator::functionDeclare(func); }
			   ;

FunctionDecl : FUNCTION IDENTIFIER LPAREN AllFormalParameters RPAREN COLON Type SEMICOLON FORWARD SEMICOLON { Function* func = CodeGenerator::createFunction($2, $4, $7); CodeGenerator::functionForward(func); }
   			 | FunctionStart Body SEMICOLON { CodeGenerator::endFunction(); }
   			 ;

FunctionStart : FUNCTION IDENTIFIER LPAREN AllFormalParameters RPAREN COLON Type SEMICOLON { Function* func = CodeGenerator::createFunction($2, $4, $7); CodeGenerator::functionDeclare(func); }
			  ;

AllFormalParameters : FormalParameters { $$ =  $1; }
					| { $$ = nullptr; }
					;

FormalParameters : FormalParameters SEMICOLON FormalParameter { $$ = CodeGenerator::parameterList($1, $3); }
				 | FormalParameter { $$ = CodeGenerator::parameterList(nullptr, $1); }
				 ;

FormalParameter : AllVar IdentList COLON Type { $$ = CodeGenerator::parameter($1, $2, $4); }
			    ;

AllVar : VAR { $$ = 0; }
	   | REF { $$ = 1; }
	   | { $$ = 0; }
	   ;

Body : AllConstantDecls AllTypeDecls AllVarDecls Block
	 ;

Block : BEGIN_ StatementSequence END
	  ;

StatementSequence : StatementSequence SEMICOLON Statement
				  | Statement
				  ;

AllTypeDecls : TYPE TypeDecls
			 |
			 ;

TypeDecls : TypeDecls TypeDecl
		  | TypeDecl
		  ;

TypeDecl : IDENTIFIER EQ Type SEMICOLON
		 ;

Type : SimpleType { $$ = $1; }
	 | RecordType
	 | ArrayType
	 ;

SimpleType : IDENTIFIER { $$ = CodeGenerator::simpleType($1); }
		   ;

RecordType : RECORD FieldDecls END
		   ;

FieldDecls : FieldDecls FieldDecl
		   |
		   ;

FieldDecl : IdentList COLON Type SEMICOLON
		  ;

IdentList : IdentList COMMA IDENTIFIER { $$ = CodeGenerator::identList($1, $3); }
		  | IDENTIFIER { $$ = CodeGenerator::identList(nullptr, $1); }
		  ;

ArrayType : ARRAY LBRACKET Expression COLON Expression RBRACKET OF Type
		  ;

AllVarDecls : VAR VarDecls
			|
			;

VarDecls : VarDecls VarDecl
		 | 
		 ;

VarDecl : IdentList COLON Type SEMICOLON { CodeGenerator::addVariables($1, $3); }
		;

Statement : Assignment
		  | IfStatement
		  | WhileStatement
		  | RepeatStatement
		  | ForStatement
		  | StopStatement
		  | ReturnStatement
		  | ReadStatement
		  | WriteStatement
		  | ProcedureCall
		  | 
		  ;

Assignment : LValue ASSIGN Expression { CodeGenerator::assignment($1, $3); }
		   ;

IfStatement : IfHead ThenPart ElseIfList ElseClause END { CodeGenerator::endIf(); }
			;

IfHead : IF Expression { CodeGenerator::newIf(); CodeGenerator::ifBranch($2); }
	   ;

ThenPart : THEN StatementSequence { CodeGenerator::ifBranchEnd(); }
		  ;

ElseIfList : ElseIfList ElseIfHead ThenPart
		   |
		   ;

ElseIfHead : ElseIfStart Expression { CodeGenerator::ifBranch($2); }
		   ;
		   
ElseIfStart : ELSEIF { CodeGenerator::labelElseIfBranch(); }
			;

ElseClause : ElseStart StatementSequence {  CodeGenerator::ifBranchEnd(); }
		   |
		   ;
ElseStart : ELSE { CodeGenerator::labelElseIfBranch(); }
		  ;

WhileStatement : WhileHead DO StatementSequence END { CodeGenerator::endLoop(); }
			   ;

WhileHead : WhileStart Expression { CodeGenerator::loopCheck($2, true); }
		  ;
		  
WhileStart : WHILE { CodeGenerator::newLoop(); }

RepeatStatement : RepeatStart StatementSequence UNTIL Expression { CodeGenerator::repeatCheck($4); }
				;
				
RepeatStart : REPEAT { CodeGenerator::newLoop(); }
			;
	   
ForStatement : ForToStatement
			 | ForDowntoStatement
			 ;

ForToStatement : ForToHead DO StatementSequence END { CodeGenerator::endForTo($1); }
			   ;

ForDowntoStatement : ForDowntoHead DO StatementSequence END { CodeGenerator::endForDownto($1); }
				   ;

ForToHead : ForStart TO Expression { CodeGenerator::forToHead($1, $3); $$ = $1; }
		  ;

ForDowntoHead : ForStart DOWNTO Expression { CodeGenerator::forDowntoHead($1,$3); $$ = $1; }
			  ;

ForStart : FOR IDENTIFIER ASSIGN Expression { $$ = CodeGenerator::setupForLoop($2, $4); }
		 ; 

StopStatement : STOP { CodeGenerator::stop(); }
			  ;

ReturnStatement : RETURN Expression { CodeGenerator::functionReturn($2); }
				| RETURN { CodeGenerator::functionReturn(nullptr); }
				;

ReadStatement : READ LPAREN ReadArgs RPAREN { CodeGenerator::read($3); }
			  ;

ReadArgs : ReadArgs COMMA LValue { $$ = CodeGenerator::lvalueList($1, $3); }
		 | LValue { $$ = CodeGenerator::lvalueList(nullptr, $1); }
		 ;

WriteStatement : WRITE LPAREN WriteArgs RPAREN { CodeGenerator::write($3); }
			   ;

WriteArgs : WriteArgs COMMA Expression { $$ = CodeGenerator::exprList($1, $3); }
		  | Expression { $$ = CodeGenerator::exprList(nullptr, $1); }
		  ;

ProcedureCall : IDENTIFIER LPAREN AllArguments RPAREN { Expression* result = CodeGenerator::functionCall($1, $3); if (result != nullptr) delete result; }
			  ;

AllArguments : Arguments { $$ = $1; }
			 | { $$ = nullptr; }
			 ;

Arguments : Arguments COMMA Expression { $$ = CodeGenerator::exprList($1, $3); }
		  | Expression { $$ = CodeGenerator::exprList(nullptr, $1); }
		  ;

Expression : CHARACTER { $$ = CodeGenerator::charExpression($1); }
		   | INTEGER { $$ = CodeGenerator::intExpression($1); }   
		   | STRING { $$ = CodeGenerator::stringExpression($1); }
           | TRUE_LIT { $$ = CodeGenerator::trueExpression(); }
           | FALSE_LIT { $$ = CodeGenerator::falseExpression(); }
		   | Expression OR Expression { $$ = CodeGenerator::eval($1, $3, "or"); }
		   | Expression AND Expression { $$ = CodeGenerator::eval($1, $3, "and"); }
		   | Expression EQ Expression { $$ = CodeGenerator::eval($1, $3, "seq"); }
		   | Expression NEQ Expression { $$ = CodeGenerator::eval($1, $3, "sne"); }
		   | Expression LEQ Expression { $$ = CodeGenerator::eval($1, $3, "sle"); }
		   | Expression GEQ Expression { $$ = CodeGenerator::eval($1, $3, "sge"); }
		   | Expression LT Expression { $$ = CodeGenerator::eval($1, $3, "slt"); }
		   | Expression GT Expression { $$ = CodeGenerator::eval($1, $3, "sgt"); }
		   | Expression PLUS Expression { $$ = CodeGenerator::eval($1, $3, "add"); }
		   | Expression MINUS Expression { $$ = CodeGenerator::eval($1, $3, "sub"); }
		   | Expression MULT Expression { $$ = CodeGenerator::eval($1, $3, "mul"); }
		   | Expression DIV Expression { $$ = CodeGenerator::eval($1, $3, "div"); }
		   | Expression MOD Expression { $$ = CodeGenerator::eval($1, $3, "rem"); }
		   | NOT Expression { $$ = CodeGenerator::evalUnary($2, "not"); }
		   | MINUS Expression %prec UMINUS { $$ = CodeGenerator::evalUnary($2, "neg"); }
		   | LPAREN Expression RPAREN  { $$ = $2; }
		   | CHR LPAREN Expression RPAREN { $$ = CodeGenerator::evalChr($3); }
		   | ORD LPAREN Expression RPAREN { $$ = CodeGenerator::evalOrd($3); }
		   | PRED LPAREN Expression RPAREN { $$ = CodeGenerator::evalPred($3); }
		   | SUCC LPAREN Expression RPAREN { $$ = CodeGenerator::evalSucc($3); }
		   | FunctionCall
		   | LValue { $$ = CodeGenerator::lvalueExpression($1); }
		   ;

FunctionCall : IDENTIFIER LPAREN AllArguments RPAREN { $$ = CodeGenerator::functionCall($1, $3); }
			 ;

LValue : LValue DOT IDENTIFIER
	   | LValue LBRACKET Expression RBRACKET
	   | IDENTIFIER { $$ = CodeGenerator::loadLValue($1); }
	   ;

%%

int main(int argc, char **argv)
{
  extern FILE *yyin;

  if(argc > 1 && (yyin = fopen(argv[1], "r")) == NULL) {
    perror(argv[1]);
    return 1;
  }
  
  fout.open("out.asm");
  fout << ".text" << endl;
  fout << ".globl main" << endl;
  fout << "main:" << endl;
  fout << "\t" << "j prog" << endl;
  if(!yyparse()) {
    //std::cout << "The program is syntactically correct" << std::endl;
	return 0;
  } else {
    std::cout << "There is a syntax error in the program" << std::endl;
    return 1;
  }
  fout.close();
}

void yyerror(const char *msg)
{
	std::cout << msg << " at line " << yylineno << std::endl;
}
