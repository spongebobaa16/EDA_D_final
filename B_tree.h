#ifndef B_TREE_H
#define B_TREE_H
#include <vector>
#include <iostream>
using namespace std;

// https://www.cprogramming.com/tutorial/lesson18.html
class Solver;

class Node
{
public:
    Node() {}
    Node(int _index) : parent(0), left(0), right(0), index(_index) {}
    ~Node() {}
    bool isLeftChild() { return (parent->left == this); }
    bool isChildOf(Node *_p) { return (this == _p->left || this == _p->right); }
    Node *parent;
    Node *left;
    Node *right;
    int index;
};

class B_Tree
{
public:
    B_Tree() : root(0), dummy(new Node(-1)) {}
    ~B_Tree() {}

    void create_tree(const Solver &s); // initially create Node* for all blocks and push into Tree_vec.
    void insert(int index, int parent, bool parent_left, bool child_left);
    // insert node(index) to be node(parent)'s parent_left child. (if parent_left: true -> left.   else -> right)
    // original child become new inserted node's child_left child. (if child_left: true -> left.   else -> right)

    // Node*   search(int index);    // == Tree_vec[index]

    // for SA
    void rotate(int index);
    void remove(int index, bool child_left);
    void swap(int index1, int index2);
    void move(int index1, int index2, bool child_left); // move index1 to index2's left(right) child, depends on child_left == 1(0)

    void destroy_tree();

    // Debug
    void printTreePreorder(Node *node);
    void printTree(const string &prefix, Node *parent, bool isLeft, bool isRoot); // left subtree is on the top

    void printTree()
    {
        printTree("", root, false, true);
    }

    Node *root, *dummy;
    vector<Node *> Tree_vec;
};

#endif