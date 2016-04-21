/*
 * Type.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Tam Nguyen
 */

#ifndef TYPE_H_
#define TYPE_H_

#include <string>
#include <memory>
#include <map>

// abstract base class
class Type {
public:
	virtual int size() = 0;
	virtual std::string name() = 0;
	virtual ~Type() = default;
};

class BuiltIn: public Type {
	virtual int size() = 0;
	virtual std::string name() = 0;
};

// integer
class IntType : public BuiltIn {
public:
	int size() { return 4; }
	std::string name() { return "integer"; }
};

// character
class CharType: public BuiltIn {
public:
	int size() { return 4; }
	std::string name() { return "char"; }
};


// boolean
class BoolType: public BuiltIn {
public:
	int size() { return 4; }
	std::string name() { return "boolean"; }
};

// string
class StringType: public BuiltIn {
public:
	int size() { return 4; }
	std::string name() { return "string"; }
};

// type factory
class BuiltInType {
public:
	static std::shared_ptr<Type> getInt();
	static std::shared_ptr<Type> getChar();
	static std::shared_ptr<Type> getBool();
	static std::shared_ptr<Type> getString();
private:
	static std::shared_ptr<Type> intType;
	static std::shared_ptr<Type> charType;
	static std::shared_ptr<Type> boolType;
	static std::shared_ptr<Type> stringType;
};

class ArrayType : public Type {
public:
	ArrayType(int lower, int upper, std::shared_ptr<Type> type): Type(), lowerBound(lower), upperBound(upper), baseType(type) {}
	int size() { return (upperBound - lowerBound + 1) * baseType->size(); }
	std::string name() { return typeName; }
	void setName(std::string text) { typeName = text; }
	int getLowerBound() const { return lowerBound; }
	int getUpperBound() const { return upperBound; }
	std::shared_ptr<Type> getBaseType() const { return baseType; }
private:
	std::string typeName;
	int lowerBound;
	int upperBound;
	std::shared_ptr<Type> baseType;
};

class RecordType: public Type {
public:
	RecordType(): Type(), fieldMap(), fieldOffsetMap(), currentOffset(0) {}
	int size();
	std::string name() { return typeName; }
	void setName(std::string text) { typeName = text; }
	void addField(std::string fieldName, std::shared_ptr<Type> fieldType);
	std::shared_ptr<Type> getFieldType(std::string fieldName) const;
	int getFieldOffset(std::string fieldName) const;
private:
	std::string typeName;
	std::map<std::string, std::shared_ptr<Type>> fieldMap;
	std::map<std::string, int> fieldOffsetMap;
	int currentOffset;
};


class ReferenceType : public Type {
public:
	ReferenceType(std::shared_ptr<Type> refType): Type(), baseType(refType) {}
	int size() { return 4; }
	std::string name() { return "reference " + baseType->name(); }
	std::shared_ptr<Type> getBaseType() const { return baseType; }
private:
	std::shared_ptr<Type> baseType;
};


#endif /* TYPE_H_ */
