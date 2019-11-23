#ifndef SEMANTICS_H
#define SEMANTICS_H
#include <vector>
#include "token.h"
#include "node.h"

using namespace std;

//define a variable on a stack
struct stack_t
{
	Token token;
	int scope;
	int lineNumber;
};

//stack that holds variables
extern vector<stack_t> stack;

//functions
void semantics(node_t*);
void checkVar(stack_t);
int checkVarExists(stack_t);
int find(stack_t);
int compareScope(stack_t);
void removeLocalVars(int);
void printStack();

#endif
