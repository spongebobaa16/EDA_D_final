#ifndef B_TREE_H
#define B_TREE_H
#include <vector>
#include <iostream>
using namespace std;

// https://www.cprogramming.com/tutorial/lesson18.html
class Solver;

class Node{
public:
    Node(){}
    Node(int _index): parent(0), left(0), right(0), index(_index) {}
    ~Node(){}
    
    Node *parent;
    Node *left;
    Node *right;
    int index;
};

class B_Tree{
public:
    B_Tree():root(0) {}
    ~B_Tree(){}

    void    create_tree(const Solver& s);  // initially create Node* for all blocks and push into Tree_vec.
    void    insert(int index, int parent, bool parent_left, bool child_left); 
    // insert node(index) to be node(parent)'s parent_left child. (if parent_left: true -> left.   else -> right)
    // original child become new inserted node's child_left child. (if child_left: true -> left.   else -> right)

    // Node*   search(int index);    // == Tree_vec[index]        
    
    // for SA
    void    rotate(int index);
    void    remove(int index, bool child_left);
    void    swap(int index1, int index2);

    void    destroy_tree();   


    // Debug
    void    printTreePreorder(Node* node);

    Node* root;
    vector<Node*> Tree_vec;
};

#endif