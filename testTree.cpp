#include "testTree.h"
#include <string>
#include <iostream>

void preorder(node_t *node, int level)
{
	if(node == NULL)
		return;
	
	//the output for the nodes
	string line;
		
	//add two spaces per level
	for(int counter = 0; counter < level; counter++)
	{
		line.append("  ");
	}
	//append the nodes label to the line
	line.append(node->label);
	line.append(" ");
	
	//append tokens of a node to the output line
	for(int counter = 0; counter < node->tokens.size(); counter++)
	{
		line.append(node->tokens[counter].desc);
		
		if( ((counter+1) != node->tokens.size()) && (node->tokens[counter].ID != OPtk))
			line.append(",");
		
		line.append(" ");
	}
	
	cout << line << "\n";
	//print the children of the nodes
	preorder(node->child1, level + 1);
	preorder(node->child2, level + 1);
	preorder(node->child3, level + 1);
	preorder(node->child4, level + 1);
}
