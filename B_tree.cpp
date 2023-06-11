#include "B_tree.h"
#include "Solver.h"

void B_Tree::create_tree(const Solver &s)
{
    for (int i = 0; i < s.Modules.size(); i++)
    {
        Node *newNode = new Node(i);
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
    printTree();
    swap(3, 6);
    printTree();
    // insert(1, 0, false, false);
    // insert(2, 1, true, false);
    // insert(3, 2, true, false);
    // insert(4, 3, true, false);
    // insert(5, 1, false, false);
    // insert(6, 5, true, false);
    // insert(7, 0, true, false);
    // insert(8, 7, false, false);
    // insert(9, 8, false, false);
    // insert(10, 9, false, false);
    // insert(11, 8, true, false);

    // insert(1, 0, true, false);
    // insert(2, 1, true, false);
    // insert(3, 1, false, false);
    // insert(4, 0, false, false);
    // insert(5, 4, false, false);

    // insert(1, 0, true, false);
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

void B_Tree::rotate(int index)
{
}
void B_Tree::remove(int index, bool child_left) // if child_left == True -> delete left child
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
        Node *child_to_replace = child_left ? _to_remove->left : _to_remove->right;
        if (child_left)
        {
        }
        else
        {
            _to_remove->left->parent = child_to_replace;
        }
    }
    Tree_vec[index] = 0;
    delete _to_remove;
}
void B_Tree::swap(int index1, int index2)
{
    Node *node1 = Tree_vec[index1], *node2 = Tree_vec[index2];
    if (node1 == root || node2 == root)
        root = node1 == root ? node2 : node1;

    // swap parent
    Node *tmp = node1->parent;
    if (!node1->isChildOf(node2) && !node2->isChildOf(node1)) // if not parent-child relationship
    {
        bool pos1 = node1->isLeftChild(), pos2 = node2->isLeftChild(); // pos1 denote if node1 is the leftchild of its parent
        node1->parent = node2->parent;
        node2->parent = tmp; // parent-><child> have to update as well !!!
    }
    // swap left child
    tmp = node1->left;
    node1->left = node2->left;
    node2->left = tmp;

    // swap right child
    tmp = node1->right;
    node1->right = node2->right;
    node2->right = tmp;
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