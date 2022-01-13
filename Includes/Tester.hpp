#ifndef TESTER_HPP
#define TESTER_HPP

#include <Expressions.hpp>
#include <Value.hpp>
#include <Identifiers.hpp>

#include <Utility.hpp>

class Tester {
public:
	static void testAssembly();
	static void testEnvironment();
	static void testExpressions();
	static void testIdentifiers();
};

#endif