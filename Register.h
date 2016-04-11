/*
 * Register.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Tam Nguyen
 */

#ifndef REGISTER_H_
#define REGISTER_H_
#include <memory>
#include <vector>
#include <set>
#include <iostream>

class Register {

public:
	Register(std::string registerName): name(registerName) {}
	Register(const Register& rhs) = delete;
	Register& operator=(const Register& rhs) = delete;

	~Register() { pool.push_back(name); used.erase(name); }

	static void init();
	static std::unique_ptr<Register> allocate();

	std::string getName() const { return name; }
	friend std::ostream& operator<<(std::ostream& os, const Register& rhs);
	static std::set<std::string> getUsed() { return used; }
private:
	std::string name;
	static std::vector<std::string> pool;
	static std::set<std::string> used;
};

#endif /* REGISTER_H_ */
