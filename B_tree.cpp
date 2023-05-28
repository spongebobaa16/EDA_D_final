#include "B_tree.h"
#include "Solver.h"

void B_Tree::create_tree(const Solver& s){
    for(int i=0; i<s.Modules.size(); i++){
        Node* newNode = new Node(i);
        Tree_vec.push_back(newNode);
    }


    // Manually initial B_Tree below, for debugging 
    
    root = Tree_vec[0];
    insert(1, 0, true, false);
    insert(2, 0, false, false);
    insert(3, 1, false, false);
    insert(4, 3, true, false);
    insert(5, 2, true, false);
    insert(6, 2, false, false);
    insert(7, 6, true, false);



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

void B_Tree::printTreePreorder(Node* node){ // for debug
    if(node == NULL){
        return;
    } 
    
    cout << node->index << " ";
    if(node->parent != NULL) cout << " parent: " << node->parent->index << endl;
    else cout << "NO parent" << endl;
    printTreePreorder(node->left);
    printTreePreorder(node->right);
}