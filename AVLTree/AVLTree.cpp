#pragma onces
#include "AVLTree.hpp"
#include <iostream>
using namespace std;

template <typename T>
AVLTree<T>::AVLTree()
{
    root = nullptr;
}
template <typename T>
void AVLTree<T>::insert(T x)
{
    root = insertUtil(root, x);
}

template <typename T>
void AVLTree<T>::remove(T x)
{
    root = removeUtil(root, x);
}

template <typename T>
bool AVLTree<T>::search(T x)
{
    return searchUtil(root, x) != nullptr;
}

template <typename T>
void AVLTree<T>::inorder()
{
    inorderUtil(root);
    cout << endl;
}

template <typename T>
int AVLTree<T>::height(Node<T> *head)
{
    if (head == nullptr)
        return 0;
    return head->height;
}

template <typename T>
Node<T> *AVLTree<T>::leftRotation(Node<T> *head)
{
    Node<T> *newhead = head->right;
    head->right = newhead->left;
    newhead->left = head;
    head->height = 1 + max(height(head->left), height(head->right));
    newhead->height = 1 + max(height(head->left), height(head->right));
    return newhead;
}
template <typename T>
Node<T> *AVLTree<T>::rightRotation(Node<T> *head)
{
    Node<T> *newhead = head->left;
    head->left = newhead->right;
    newhead->right = head;
    head->height = 1 + max(height(head->left), height(head->right));
    newhead->height = 1 + max(height(head->left), height(head->right));
    return newhead;
}

template <typename T>
Node<T> *AVLTree<T>::insertUtil(Node<T> *head, T &x)
{
    if (head == nullptr)
    {

        Node<T> *tmp = new Node<T>(x);
        return tmp;
    }
    if (x > head->key)
    {
        head->right = insertUtil(head->right, x);
    }
    else
    {
        head->left = insertUtil(head->left, x);
    }
    head->height = 1 + max(height(head->left), height(head->right));
    // adjust balance
    int balance = height(head->left) - height(head->right);
    if (balance > 1)
    {
        if (x > head->left->key)
        { // LR
            head->left = leftRotation(head->left);
            return rightRotation(head);
        }
        else
        { // R
            return rightRotation(head);
        }
    }
    else if (balance < -1)
    {
        if (x > head->right->key)
        { // L
            return leftRotation(head);
        }
        else
        { // RL
            head->right = rightRotation(head->right);
            return leftRotation(head);
        }
    }
    return head;
}

template <typename T>
Node<T> *AVLTree<T>::removeUtil(Node<T> *head, T &x)
{
    if (head == NULL)
        return NULL;
    if (x < head->key)
    {
        head->left = removeUtil(head->left, x);
    }
    else if (x > head->key)
    {
        head->right = removeUtil(head->right, x);
    }
    else
    {
        Node<T> *r = head->right;
        if (head->right == NULL)
        {
            Node<T> *l = head->left;
            delete (head);
            head = l;
        }
        else if (head->left == NULL)
        {
            delete (head);
            head = r;
        }
        else
        {
            while (r->left != NULL)
                r = r->left;
            head->key = r->key;
            head->right = removeUtil(head->right, r->key);
        }
    }
    if (head == NULL)
        return head;
    head->height = 1 + max(height(head->left), height(head->right));
    int bal = height(head->left) - height(head->right);
    if (bal > 1)
    {
        if (height(head->left->left) >= height(head->left->right))
        {
            return rightRotation(head);
        }
        else
        {
            head->left = leftRotation(head->left);
            return rightRotation(head);
        }
    }
    else if (bal < -1)
    {
        if (height(head->right->left) >= height(head->right->right))
        {
            head->right = rightRotation(head->right);
            return leftRotation(head);
        }
        else
        {
            return leftRotation(head);
        }
    }
    return head;
}

template <typename T>
void AVLTree<T>::inorderUtil(Node<T> *head)
{
    if (head == nullptr)
        return;
    inorderUtil(head->left);
    cout << head->key << " ";
    inorderUtil(head->right);
}

template <typename T>
Node<T> *AVLTree<T>::searchUtil(Node<T> *head, T &x)
{
    if (head == nullptr)
        return nullptr;
    if (head->key == x)
        return head;
    if (head->key > x)
        return searchUtil(head->left);
    return searchUtil(head->right);
}
