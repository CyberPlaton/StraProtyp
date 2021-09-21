#pragma once


template < class T >
class BST
{
private:
    struct node {
        T data;
        size_t hash;
        node* left;
        node* right;
    };


    node* root;

public:
    BST() {
        root = nullptr;
    }

    ~BST() {
        root = makeEmpty(root);
    }


    void insert(size_t hash, T data) {
        root = insert(hash, data, root);
    }

    void remove(size_t hash) {
        root = remove(hash, root);
    }

    T search(size_t hash) {
        root = find(root, hash);
        if (root)
        {
            return root->data;
        }

        return nullptr;
    }


private:
    /*
    * Delete Everyting.
    */
    node* makeEmpty(node* t) {
        if (t == nullptr) return nullptr;
        else
        {
            makeEmpty(t->left);
            makeEmpty(t->right);
            delete t;
        }

        return nullptr;
    }



    node* insert(size_t hash, T data, node* t)
    {
        if (t == nullptr)
        {
            t = new node;
            t->hash = hash;
            t->data = data;
            t->left = t->right = nullptr;
        }
        else if (hash < t->hash)
        {
            t->left = insert(hash, data, t->left);
        }
        else if (hash > t->hash)
        {
            t->right = insert(hash, data, t->right);
        }

        return t;
    }



    node* findMin(node* t)
    {
        if (t == nullptr)
            return nullptr;
        else if (t->left == nullptr)
            return t;
        else
            return findMin(t->left);
    }

    node* findMax(node* t) {
        if (t == nullptr)
            return nullptr;
        else if (t->right == nullptr)
            return t;
        else
            return findMax(t->right);
    }

    node* remove(size_t hash, node* t) {
        node* temp;
        if (t == nullptr)
            return nullptr;
        else if (hash < t->hash)
            t->left = remove(hash, t->left);
        else if (hash > t->hash)
            t->right = remove(hash, t->right);
        else if (t->left && t->right)
        {
            temp = findMin(t->right);
            t->hash = temp->hash;
            t->right = remove(t->hash, t->right);
        }
        else
        {
            temp = t;
            if (t->left == nullptr)
                t = t->right;
            else if (t->right == nullptr)
                t = t->left;
            delete temp;
        }

        return t;
    }

    void inorder(node* t) {
        if (t == nullptr)
            return;
        inorder(t->left);
        inorder(t->right);
    }

    node* find(node* t, size_t hash) {
        if (t == nullptr)
            return nullptr;
        else if (hash < t->hash)
            return find(t->left, hash);
        else if (hash > t->hash)
            return find(t->right, hash);
        else
            return t;
    }
};