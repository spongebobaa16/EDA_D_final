#include "B_tree.h"
#include "Solver.h"
#include "util.h"

void B_Tree::create_tree(const Solver &s)
{
    for (int i = 0; i < s.Modules.size(); i++)
    {
        Node *newNode = new Node(i);
        Tree_vec.push_back(newNode);
    }

    // Manually initial B_Tree below, for debugging

    root = Tree_vec[0];
    for (int i = 1; i < s.Modules.size(); i++)
    {
        if (i % 2 == 1)
        {
            insert(i, (i - 1) / 2, true, false);
        }
        else
        {
            insert(i, (i - 1) / 2, false, false);
        }
    }
    printTree();
    // remove(6, 0);
    // printTree();
    // ─────0
    //     L├────1
    //      │   L├────3
    //      │    │   L├────7
    //      │    │   R└───8
    //      │   R└───4
    //      │       L├────9
    //     R└───2
    //         L├────5
    //         R└───6
}

void B_Tree::insert(int index, int parent, bool parent_left, bool child_left)
{
    int original_child = -1;
    if (parent_left)
    {
        if (Tree_vec[parent]->left != 0)
            original_child = Tree_vec[parent]->left->index;
        Tree_vec[parent]->left = Tree_vec[index];
    }
    else
    {
        if (Tree_vec[parent]->right != 0)
            original_child = Tree_vec[parent]->right->index;
        Tree_vec[parent]->right = Tree_vec[index];
    }

    Tree_vec[index]->parent = Tree_vec[parent];

    if (original_child != -1)
    {
        Tree_vec[original_child]->parent = Tree_vec[index];
        if (child_left)
            Tree_vec[index]->left = Tree_vec[original_child];
        else
            Tree_vec[index]->right = Tree_vec[original_child];
    }
}

// void B_Tree::rotate(int index)
// {
// }
void B_Tree::remove(int index, bool child_left) // if child_left == True -> swap with left child
{
    Node *_to_remove = Tree_vec[index];
    if (_to_remove == 0)
        return;
    if (_to_remove->left == 0 && _to_remove->right == 0) // case 1 : delete leaf node
    {
        if (_to_remove->isLeftChild())
            _to_remove->parent->left = 0;
        else
            _to_remove->parent->right = 0;
    }
    else if (_to_remove->left == 0 || _to_remove->right == 0) // case 2 : delete node with single child
    {
        Node *_to_remove_child = (_to_remove->right != 0) ? _to_remove->right : _to_remove->left;
        if (_to_remove->isLeftChild())
            _to_remove->parent->left = _to_remove_child;
        else
            _to_remove->parent->right = _to_remove_child;
        _to_remove_child->parent = _to_remove->parent;
    }
    else // case 3 : delete node with two children
    {
        while (_to_remove->left != 0 || _to_remove->right != 0)
        {
            Node *child_to_swap = child_left ? _to_remove->left : _to_remove->right;
            if (child_to_swap == 0)
                child_to_swap = !child_left ? _to_remove->left : _to_remove->right;
            swap(_to_remove->index, child_to_swap->index);
        }
        (_to_remove->isLeftChild() ? _to_remove->parent->left : _to_remove->parent->right) = 0;
    }
    Tree_vec[index] = 0;
    delete _to_remove;
}

/*
three cases:
    1.  no relationship
    2. one node is root
    3. there is parent/child relationship between node1 & node2 -> make node1 always the parent
*/

void B_Tree::swap(int index1, int index2) // deal with normal case first (check) , next root (check) , next nodes with parent/child relationship
{                                         // if node1 node2's number of children is not the same ? (check)
    Node *node1 = Tree_vec[index1], *node2 = Tree_vec[index2];
    if (node1->depth() > node2->depth()) // make node1 always the elder
    {
        Node *tmp1 = node1;
        node1 = node2;
        node2 = tmp1;
    }

    bool PCrelationship = node1->isChildOf(node2) || node2->isChildOf(node1); // if node1 & node2 have parent-child relationship
    bool node2Left = 0;
    if (node2->parent != 0)
        node2Left = node2->isLeftChild(); // see if node2 is left child or not
    Node *root_original = root;
    if (node1 == root || node2 == root)
        root = node1 == root ? node2 : node1;

    // swap parent

    // if (node1->isChildOf(node2) || node2->isChildOf(node1))
    // {
    //     PCrelationship = 1;
    //     if (node1->isChildOf(node2)) // make node1 always the parent
    //     {
    //         Node *tmp1 = node1;
    //         node1 = node2;
    //         node2 = tmp1;
    //     }
    // }
    Node *tmp = node1->parent;

    // parent_childi see nodei is its parent's left or right child
    Node *&parent_child1 = (node1 != root_original) ? (node1->isLeftChild() ? node1->parent->left : node1->parent->right) : dummy;
    Node *&parent_child2 = (node2 != root_original) ? (node2->isLeftChild() ? node2->parent->left : node2->parent->right) : dummy;

    if (!PCrelationship)
    {
        node1->parent = node2->parent;
        node2->parent = tmp;
        parent_child1 = node2; // original node1's parent's child should be update to node2
        parent_child2 = node1; // original node2's parent's child should be update to node1
    }
    else
    {
        node1->parent = node2;
        node2->parent = tmp;
        parent_child1 = node2; // original node1's child should be update to node2
        // (node2Left ? node2->left : node2->right) = node1; // original node2's child should be update to node1
    }

    // swap left child
    if (!PCrelationship || !node2Left)
    {
        tmp = node1->left;
        node1->left = node2->left;
        node2->left = tmp;
        if (node1->left != 0)
            node1->left->parent = node1; // node1's left child's parent should = node1
        if (node2->left != 0)
            node2->left->parent = node2; // node2's left child's parent should = node2
    }
    else
    {
        node1->left = node2->left;
        node2->left = node1;
        if (node1->left != 0)
            node1->left->parent = node1;
        // node1->parent is already node2
    }

    // swap right child
    if (!PCrelationship || node2Left)
    {
        tmp = node1->right;
        node1->right = node2->right;
        node2->right = tmp;
        if (node1->right != 0)
            node1->right->parent = node1; // node1's left child's parent should = node1
        if (node2->right != 0)
            node2->right->parent = node2; // node2's left child's parent should = node2
    }
    else
    {
        node1->right = node2->right;
        node2->right = node1;
        if (node1->right != 0)
            node1->right->parent = node1;
        // node1->parent is already node2
    }
}

void B_Tree::move(int index1, int index2, bool parent_left, bool child_left)
{
    remove(index1, 1);
    Tree_vec[index1] = new Node(index1);
    insert(index1, index2, parent_left, child_left);
}

void B_Tree::printTreePreorder(Node *node)
{ // for debug
    if (node == NULL)
    {
        return;
    }

    cout << node->index << " ";
    if (node->parent != NULL)
        cout << " parent: " << node->parent->index << endl;
    else
        cout << "NO parent" << endl;
    printTreePreorder(node->left);
    printTreePreorder(node->right);
}

void B_Tree::printTree(const string &prefix, Node *parent, bool isLeft, bool isRoot)
{
    if (parent == nullptr)
        return;

    if (isRoot)
    {
        cout << "─────";
    }
    else
    {
        cout << prefix << (isLeft ? "L├────" : "R└───");
    }
    cout << parent->index << endl;

    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->left, true, false);
    printTree(prefix + (isLeft ? " │   " : "    "),
              parent->right, false, false);
}