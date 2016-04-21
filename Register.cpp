/*
 * Register.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Tam Nguyen
 */

#include "Register.h"
#include <iostream>
extern void yyerror(const char *msg);

using namespace std;

vector<string> Register::pool;
set<string> Register::used;

void Register::init() {
	for (int i = 0; i < 8; i++)
		pool.push_back("$s" + std::to_string(i));
	for (int i = 0; i < 10; i++)
		pool.push_back("$t" + std::to_string(i));
}


unique_ptr<Register> Register::allocate() {
	if (pool.empty()) {
		yyerror("Running out of registers");
		return nullptr;
	}
	auto registerName = pool.back();
	pool.pop_back();
	used.insert(registerName); // used
	unique_ptr<Register> ret(new Register(registerName));
	return ret;
}


ostream& operator<<(ostream& os, const Register& rhs) {
	os << rhs.getName();
	return os;
}
