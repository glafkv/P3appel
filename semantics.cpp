#include "semantics.h"
#include "token.h"
#include "node.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

//max number of allowed variable definitions
const int MAX_VARS = 100;

//the scope of the variable(s)
static int varScope = 0;

//used to test an individual variable on the stack
static stack_t temp;

//the stack
vector<stack_t> stack;

//current location of stack
int stackLoc = 0;

//number of variables on the stack
int stackVars = 0;

//recursively search through the entire parse tree
void semantics(node_t *root)
{
	if(root == NULL){
		return;
	} 
	//<program> -> <vars><block>
	if(root->label == "<program>")
	{
		//process vars and block
		semantics(root->child1);
		semantics(root->child2);
		
		removeLocalVars(varScope);
		varScope--;
		
		cout << "Semantics OK\n";
		return;
	}
	
	//<block> -> start <vars><stats> stop
	if(root->label == "<block>")
	{
		//increment the scope
		varScope++;
		
		//process vars and stats
		semantics(root->child1);
		semantics(root->child2);
		
		//decrement the scope and remove all local variables that were defined in this scope
		removeLocalVars(varScope);
		varScope--;
	}
	
	//<vars>-> empty | var Identifier : Integer <vars>
	if(root->label == "<vars>")
	{
		//check if the token is an empty production or not
		temp.token = root->tokens.front();
		temp.scope = varScope;
		root->tokens.erase(root->tokens.begin());		
		
		if(temp.token.desc != "EMPTY")
		{
			//check if an identifier has been declared in the scope or not
			checkVar(temp);
		
			//increment the number of variables on the stack
			stackVars++;
			
			//process <vars>
			semantics(root->child1);
		}
	}
	
	//<expr> -> <A> + <expr> | <A>
	if(root->label == "<expr>")
	{
		//if the token vector was empty
		if(root->tokens.empty())
		{
			//process <A>
			semantics(root->child1);
		}
		else
		{
			//process expr and A
			semantics(root->child1);
			semantics(root->child2);
		}
	}
	
	//<A> -> <N> - <A> | <N>
/*	if(root->label == "<A>")
	{
		//if the tokens vector is empty
		if(root->tokens.empty())
		{
			//process <N>
			semantics(root->child1);
		}
		else
		{
			//process N and A
			semantics(root->child1);
			semantics(root->child2);
		}
	}*/
	//<N> -> <M> / <N> | <M> * <N> | <M>
	/*if(root->label == "<N>")
	{
		//if the tokens vector is empty
		if(root->tokens.empty())
		{
			//process <M>
			semantics(root->child1);
		}
		else
		{
			//process <M> and <N>
			semantics(root->child1);
			semantics(root->child2);
		}
	}*/
	//<M> -> -<M> | <R>
	/*if(root->label == "<M>")
	{
		//process <M> or <R>
		semantics(root->child1);
	}*/
	//<R> -> [<expr>] | Identifier | Integer
	if(root->label == "<R>")
	{
		//if the tokens vector is empty
		if(root->tokens.empty())
		{
			//process <expr>
			semantics(root->child1);
		}
		else
		{
			//if an identifier was used, verify it was defined and can be accessed
			temp.token = root->tokens.front();
			temp.scope = varScope;
			
			//check if the variable can be accessed in the scope
			if(temp.token.ID == IDtk)
			{
				compareScope(temp);
			}
			return;
		}
	}
	//<stats> -> <stat> ; <mStat>
	/*if(root->label == "<stats>")
	{
		//process <stat> and <mSTat>
		semantics(root->child1);
		semantics(root->child2);
	}*/
	
	//<stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
	/*if(root->label == "<stat>")
	{
		//process all
		semantics(root->child1);
	}*/
	//<mStat> -> empty | <stat> ; <mStat>
	/*if(root->label == "<mStat>")
	{
		//check if empty
		if(root->tokens.empty())
		{
			//process <stat> and <mStat>
			semantics(root->child1);
			semantics(root->child2);
		}
	}*/
	//<in> -> in Identifier
	if(root->label == "<in>")
	{
		//verify the ID exists
		temp.token = root->tokens.front();
		temp.scope = varScope;
		
		//check if the bariable was defined and may be accessed
		compareScope(temp);
		
		return;
	}
	//<out> -> out <expr>
	/*if(root->label == "<out>")
	{
		//process <expr>
		semantics(root->child1);
	}*/
	//<if> -> cond((<expr><RO><expr>))<stat>
	/*if(root->label == "<if>")
	{
		//process <expr><RO><expr><stat>
		semantics(root->child1);	
		semantics(root->child2);
		semantics(root->child3);
		semantics(root->child4);
	}*/
	//<loop> -> iterate((<expr><RO><expr>))<stat>
/*	if(root->label == "<loop>")
	{
		//process
		semantics(root->child1);
		semantics(root->child2);
		semantics(root->child3);
		semantics(root->child4);
	}*/
	
	//<assign> -> Identifier << <expr>
	if(root->label == "<assign>")
	{
		//check if ID was defined
		temp.token = root->tokens.front();
		temp.scope = varScope;
		
		//see if it's accessable
		compareScope(temp);
		
		//process <expr>
		semantics(root->child1);
		
		return;
	}
}
//check if the variable has been defined in the scope. if it has, print an error. if not, push onto stack
void checkVar(stack_t var)
{
	//check if its been defined
	int varDefined = find(var);
	
	//if its been defined, error. if not, ,push on stack
	if(varDefined > 0)
	{
		cout << "Semantics error: The variable '" << getTokenDesc(var.token) << "' on line " << var.token.lineNumber << " has already been defined in this scope on line " << stack.at(varDefined).token.lineNumber << ".\n";
		exit(EXIT_FAILURE);
	}
	else
	{
		stack.push_back(var);;
		stackLoc++;
		//printStack();
	}
}
//search the stack to see if variable has been defined
int checkVarExists(stack_t var)
{
	for(int counter = 0; counter < stack.size(); counter++)
	{
		//return location of this id in the stack
		if(stack.at(counter).token.desc == var.token.desc)
			return counter;
	}
	return -1;
}
//find the variable
int find(stack_t var)
{
	for(int counter = 0; counter < stack.size(); counter++)
	{
		//if id matches at this location and the two ids have the same scope, retrn location
		if((stack.at(counter).token.desc == var.token.desc) && (stack.at(counter).scope == var.scope))
		{
			return counter;
		}
	}
	return -1;
}

//compare scope so the variable may be accessed
int compareScope(stack_t var)
{
	//check if variable has been defined and is on stack
	int varLoc = checkVarExists(var);
	
	//if scope is greater than scope given, the variable is not accessable. print error
	if(varLoc >= 0)
	{
		if(stack.at(varLoc).scope > var.scope)
		{
			cout << "Semantics Error: The variable '" << getTokenDesc(var.token) << "' on line " << var.token.lineNumber << " cannot be accessed in this scope.\n";

			exit(EXIT_FAILURE);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		//if variable was not located, error
		cout << "Semantics Error: The variable '" << getTokenDesc(var.token) << "' on line " << var.token.lineNumber << " is not on the stack, and has not yet been defined or cannot be accessed in this scope.\n";
		exit(EXIT_FAILURE);
	}
}
//delete all variables that were defined in the block when <block> ends
void removeLocalVars(int scope)
{
	if(scope > 0)
	{
		//go through stack
		for(int counter = 0; counter < stack.size(); counter++)
		{
			//remove the variable from the stack
			if(stack.at(counter).scope == scope)
				stack.erase(stack.begin() + counter);
		}
	}
}
//print the stack
void printStack()
{
	for(int i = 0; i < stack.size(); i++)
	{
		cout <<  "\tIndex " << i << " = " << stack.at(i).token.desc << ", scope is " << stack.at(i).scope << "\n";
	}
}
