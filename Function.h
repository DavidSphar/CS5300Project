/*
 * Function.h
 *
 *  Created on: Apr 7, 2016
 *      Author: tamnguyen
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <iostream>
#include <vector>
#include <utility>
#include "Type.h"

class Function {
public:
	Function(std::string n, std::vector<std::pair<std::string, std::shared_ptr<Type>>> a, std::shared_ptr<Type> r, bool d = false):
		name(n), args(a), returnType(r), defined(d) {}
	std::string getName() const { return name; }
	std::vector<std::pair<std::string, std::shared_ptr<Type>>> getArgs() const { return args; }
	std::shared_ptr<Type> getReturnType() const { return returnType; }
	bool isDefined() const { return defined; }
	void setDefined(bool d) { defined = d; }
	friend std::ostream& operator<<(std::ostream& os, const Function& rhs) {
		os << rhs.name << std::endl;
		for (auto& elem: rhs.args) os << elem.first << ", " << elem.second->name() << std::endl;
		if (rhs.returnType != nullptr) os << rhs.returnType->name() << std::endl;
		return os;
	}
private:
	std::string name;
	std::vector<std::pair<std::string, std::shared_ptr<Type>>> args;
	std::shared_ptr<Type> returnType;
	bool defined;
};

#endif /* FUNCTION_H_ */
