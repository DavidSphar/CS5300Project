all: cpslcompiler
cpslcompiler:   cpsl.lex cpsl.y
				bison -d -v cpsl.y
				flex cpsl.lex
				g++ -o $@ cpsl.tab.c lex.yy.c CodeGenerator.cpp Expression.cpp Register.cpp StringTable.cpp SymbolTable.cpp Type.cpp LValue.cpp -lfl -std=c++11
clean:			
	rm -f cpslcompiler lex.yy.c cpsl.tab.c cpsl.tab.h cpsl.output out.asm

