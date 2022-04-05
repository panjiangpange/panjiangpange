#pragma once
#include <iostream>
using namespace std;

template <typename T>
struct Node
{
    T key;
    int height;
    Node *left, *right;
    Node(T x)
    {
        key = x;
        height = 1;
        left = nullptr, right = nullptr;
    }
};

template <typename T>
class AVLTree
{
public:
    AVLTree();
    int getN();
    void insert(T x);
    void remove(T x);
    bool search(T x);
    void inorder();

private:
    int n;
    Node<T> *root;
    int height(Node<T> *head);

    Node<T> *leftRotation(Node<T> *head);
    Node<T> *rightRotation(Node<T> *head);
    Node<T> *insertUtil(Node<T> *head, T &x);
    Node<T> *removeUtil(Node<T> *head, T &x);
    Node<T> *searchUtil(Node<T> *head, T &x);
    void inorderUtil(Node<T> *head);
};