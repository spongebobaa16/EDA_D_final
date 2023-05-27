#include "B_tree.h"
#include "Solver.h"

void B_Tree::create_tree(const Solver& s){
    
}

void B_Tree::insert(int index, int parent, bool parent_left, bool child_left){
    int original_child = -1;
    if(parent_left){
        if(Tree_vec[parent]->left != 0) original_child = Tree_vec[parent]->left->index;
        Tree_vec[parent]->left = Tree_vec[index];
    }
    else{
        if(Tree_vec[parent]->right != 0) original_child = Tree_vec[parent]->right->index;
        Tree_vec[parent]->right = Tree_vec[index];
    }
    
    Tree_vec[index]->parent = Tree_vec[parent];
    
    if(original_child != -1){
        Tree_vec[original_child]->parent = Tree_vec[index];
        if(child_left)
            Tree_vec[index]->left = Tree_vec[original_child];
        else
            Tree_vec[index]->right = Tree_vec[original_child];
    } 
}