#include AVLTree.hpp
#include AVLTree.cpp
#include iostream
using namespace std;
int main()
{
    AVLTreefloat t = new AVLTreefloat();
    t-insert(1);
    t-insert(2);
    t-insert(3);
    t-insert(4);
    t-insert(5);
    t-insert(6);
    t-insert(7);
    t-insert(8);
    t-inorder();
    t-remove(7);
    t-inorder();
    return 0;
}