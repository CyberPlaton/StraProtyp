#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>

#include "ColorConsole.h"








/*
* Original Author: https://github.com/Bibeknam/algorithmtutorprograms/blob/master/data-structures/avl-trees/AVLTree.cpp
* 
* 03.10.2021
* Modified by Bogdan Strohonov to Accept extra templated Data and
* have a search function for retrieving the templated Data.
*/

// class AVLTree implements the operations in AVL tree
template < class T >
class AVLTree2
{
private:

	// data structure that represents a node in the tree
	struct Node {
		size_t data = 0; // holds the key
		Node* parent = nullptr; // pointer to the parent
		Node* left = nullptr; // pointer to left child
		Node* right = nullptr; // pointer to right child
		int bf = 0; // balance factor of the node
		T storedData; // holds the templated type of data, e.g. a pointer to Gameobject
	};

	typedef Node* NodePtr;

private:


	NodePtr root;

	// initializes the nodes with appropirate values
	// all the pointers are set to point to the null pointer
	void initializeNode(NodePtr node, size_t key) {
		node->data = key;
		node->parent = nullptr;
		node->left = nullptr;
		node->right = nullptr;
		node->bf = 0;
	}

	void preOrderHelper(NodePtr node)
	{
		using namespace std;

		if (node != nullptr) {
			cout << node->data << " ";
			preOrderHelper(node->left);
			preOrderHelper(node->right);
		}
	}

	void inOrderHelper(NodePtr node)
	{
		using namespace std;

		if (node != nullptr) {
			inOrderHelper(node->left);
			cout << node->data << " ";
			inOrderHelper(node->right);
		}
	}

	void postOrderHelper(NodePtr node)
	{
		using namespace std;

		if (node != nullptr) {
			postOrderHelper(node->left);
			postOrderHelper(node->right);
			cout << node->data << " ";
		}
	}


	NodePtr searchTreeHelper(NodePtr node, size_t key)
	{
		using namespace std;

		if (node == nullptr || key == node->data) {
			return node;
		}


		// Test for same key.
		if (key == node->data)
		{
			cout << color(colors::RED);
			cout << "[AVLTree2::searchTreeHelper] Fatal Error! Same Keys found: " << key << white << endl;
			throw std::runtime_error("Fatal Error!");
		}


		if (key < node->data) {
			return searchTreeHelper(node->left, key);
		}
		return searchTreeHelper(node->right, key);
	}


	NodePtr deleteNodeHelper(NodePtr node, size_t key)
	{
		// search the key
		if (node == nullptr) return node;
		else if (key < node->data) node->left = deleteNodeHelper(node->left, key);
		else if (key > node->data) node->right = deleteNodeHelper(node->right, key);
		else {
			// the key has been found, now delete it

			// case 1: node is a leaf node
			if (node->left == nullptr && node->right == nullptr) {
				delete node;
				node = nullptr;
			}

			// case 2: node has only one child
			else if (node->left == nullptr) {
				NodePtr temp = node;
				node = node->right;
				delete temp;
			}

			else if (node->right == nullptr) {
				NodePtr temp = node;
				node = node->left;
				delete temp;
			}

			// case 3: has both children
			else {
				NodePtr temp = minimum(node->right);
				node->data = temp->data;
				node->right = deleteNodeHelper(node->right, temp->data);
			}

		}

		// Write the update balance logic here 
		updateBalance(node);

		return node;
	}




	// update the balance factor the node
	void updateBalance(NodePtr node) {
		if (node->bf < -1 || node->bf > 1) {
			rebalance(node);
			return;
		}

		if (node->parent != nullptr) {
			if (node == node->parent->left) {
				node->parent->bf -= 1;
			}

			if (node == node->parent->right) {
				node->parent->bf += 1;
			}

			if (node->parent->bf != 0) {
				updateBalance(node->parent);
			}
		}
	}

	// rebalance the tree
	void rebalance(NodePtr node)
	{
		if (node->bf > 0) {
			if (node->right->bf < 0) {
				rightRotate(node->right);
				leftRotate(node);
			}
			else {
				leftRotate(node);
			}
		}
		else if (node->bf < 0) {
			if (node->left->bf > 0) {
				leftRotate(node->left);
				rightRotate(node);
			}
			else {
				rightRotate(node);
			}
		}
		
	}

	void printHelper(NodePtr root, std::string indent, bool last)
	{
		using namespace std;

		// print the tree structure on the screen
		if (root != nullptr) {
			cout << indent;
			if (last) {
				cout << "R----";
				indent += "     ";
			}
			else {
				cout << "L----";
				indent += "|    ";
			}

			cout << root->data << "( BF = " << root->bf << ")" << endl;

			printHelper(root->left, indent, false);
			printHelper(root->right, indent, true);
		}
	}

public:
	AVLTree2()
	{
		root = nullptr;
	}

	int min(size_t a, size_t b) { return(a < b) ? a : b; }
	int max(size_t a, size_t b) { return(a > b) ? a : b; }


	// Pre-Order traversal
	// Node->Left Subtree->Right Subtree
	void preorder() {
		preOrderHelper(this->root);
	}

	// In-Order traversal
	// Left Subtree -> Node -> Right Subtree
	void inorder() {
		inOrderHelper(this->root);
	}

	// Post-Order traversal
	// Left Subtree -> Right Subtree -> Node
	void postorder() {
		postOrderHelper(this->root);
	}

	// search the tree for the key k
	// and return the corresponding node
	NodePtr searchTree(size_t k) {
		return searchTreeHelper(this->root, k);
	}

	// find the node with the minimum key
	NodePtr minimum(NodePtr node) {
		while (node->left != nullptr) {
			node = node->left;
		}
		return node;
	}

	// find the node with the maximum key
	NodePtr maximum(NodePtr node) {
		while (node->right != nullptr) {
			node = node->right;
		}
		return node;
	}

	// find the successor of a given node
	NodePtr successor(NodePtr x) {
		// if the right subtree is not null,
		// the successor is the leftmost node in the
		// right subtree
		if (x->right != nullptr) {
			return minimum(x->right);
		}

		// else it is the lowest ancestor of x whose
		// left child is also an ancestor of x.
		NodePtr y = x->parent;
		while (y != nullptr && x == y->right) {
			x = y;
			y = y->parent;
		}
		return y;
	}

	// find the predecessor of a given node
	NodePtr predecessor(NodePtr x) {
		// if the left subtree is not null,
		// the predecessor is the rightmost node in the 
		// left subtree
		if (x->left != nullptr) {
			return maximum(x->left);
		}

		NodePtr y = x->parent;
		while (y != nullptr && x == y->left) {
			x = y;
			y = y->parent;
		}

		return y;
	}

	// rotate left at node x
	void leftRotate(NodePtr x) {
		NodePtr y = x->right;
		x->right = y->left;
		if (y->left != nullptr) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->left) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;

		// update the balance factor
		x->bf = x->bf - 1 - max(0, y->bf);
		y->bf = y->bf - 1 + min(0, x->bf);
	}

	// rotate right at node x
	void rightRotate(NodePtr x) {
		NodePtr y = x->left;
		x->left = y->right;
		if (y->right != nullptr) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		}
		else if (x == x->parent->right) {
			x->parent->right = y;
		}
		else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;

		// update the balance factor
		x->bf = x->bf + 1 - min(0, y->bf);
		y->bf = y->bf + 1 + max(0, x->bf);
	}

	// search for the object stored with given key and
	// return in specified template datatype of AVLTree2
	T findStoredData(size_t key)
	{
		return searchTree(key)->storedData;
	}

	// insert the key to the tree in its appropriate position
	void insert(size_t key, T object) {
		// PART 1: Ordinary BST insert
		NodePtr node = new Node;
		node->parent = nullptr;
		node->left = nullptr;
		node->right = nullptr;
		node->data = key;
		node->bf = 0;
		node->storedData = object;
		NodePtr y = nullptr;
		NodePtr x = this->root;

		while (x != nullptr) {
			y = x;
			if (node->data < x->data) {
				x = x->left;
			}
			else {
				x = x->right;
			}
		}

		// y is parent of x
		node->parent = y;
		if (y == nullptr) {
			root = node;
		}
		else if (node->data < y->data) {
			y->left = node;
		}
		else {
			y->right = node;
		}

		// PART 2: re-balance the node if necessary
		updateBalance(node);

	}

	NodePtr getRoot() {
		return this->root;
	}

	// delete the node from the tree
	NodePtr deleteNode(size_t data)
	{
		NodePtr deletedNode = deleteNodeHelper(this->root, data);
		return deletedNode;
	}

	// print the tree structure on the screen
	void prettyPrint() {
		printHelper(this->root, "", true);
	}

};