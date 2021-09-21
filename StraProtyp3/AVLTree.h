#pragma once


template < class T >
class AVLTree
{
private:
    struct node {
        T data;
        size_t hash;
        node* left;
        node* right;

        // AVL-Tree.
        int height = 1;
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

    int max(int a, int b) { return a > b ? a : b; }

    int getHeight(node* t)
    {
        if (t) return t->height;
        else return 0;
    }

    int getBalance(node* t)
    {
        if (t == nullptr) return 0;
        else
        {
            return getHeight(t->left) - getHeight(t->left);
        }
    }

    node* rightRotate(node* t)
    {
        node* y = t->left;
        node* t3 = y->right;

        y->right = t;
        t->left = t3;

        t->height = 1 + max(getHeight(t->left), getHeight(t->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));

        return y;
    }

    node* leftRotate(node* t)
    {
        node* y = t->right;
        node* t2 = y->left;

        y->left = t;
        t->right = t2;

        t->height = 1 + max(getHeight(t->left), getHeight(t->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));

        return y;
    }

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


        // Update height of ancestor.
        t->height = 1 + max(
            getHeight(t->left), getHeight(t->right)
        );

        // Get balance factor.
        int balance = getBalance(t);

        // Balance Tree out, if needed;
        // we have 4 cases.
        if (balance > 1 && hash < t->left->hash) // Left-Left-Rot.
        {
            return rightRotate(t);
        }
        if (balance < -1 && hash > t->right->hash) // Right-Right-Rot.
        {
            return leftRotate(t);
        }
        if (balance > 1 && hash > t->left->hash) // Left-Right-Rot.
        {
            t->left = leftRotate(t->left);
            return rightRotate(t);
        }
        if (balance < -1 and hash < t->right->hash) // Right-Left-Rot.
        {
            t->right = rightRotate(t->right);
            return leftRotate(t);
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

    node* remove(size_t hash, node* t)
    {
        node* temp;
     
        if (t == nullptr)
        {
            return nullptr;
        }
        else if (hash < t->hash)
        {
            t->left = remove(hash, t->left);
        }
        else if (hash > t->hash)
        {
            t->right = remove(hash, t->right);
        }
        else
        {
            if (t->left == nullptr)
            {
                temp = t->right;
                delete t;
                t = nullptr;
                return temp;
            }
            else if (t->right == nullptr)
            {
                temp = t->right;
                delete t;
                t = nullptr;
                return temp;
            }
            else // If both kids are alive.
            {
                temp = findMin(t->right);
                t->hash = temp->hash;
                t->right = remove(temp->hash, t->right);
            }

        }

        if (t == nullptr) return nullptr;

        // Update height of ancestor.
        t->height = 1 + max(getHeight(t->left), getHeight(t->right));

        // Get balance.
        int balance = getBalance(t);

        // Restore Tree balance if needed.
        if (balance > 1 && getBalance(t->left) >= 0) // Left-Left
        {
            return rightRotate(t);
        }
        if (balance < -1 && getBalance(t->right) <= 0) // Right-Right
        {
            return leftRotate(t);
        }
        if (balance > 1 && getBalance(t->left) < 0) // Left-Right
        {
            t->left = leftRotate(t->left);
            return rightRotate(t);
        }
        if (balance < -1 && getBalance(t->right) > 0) // Right-Left
        {
            t->right = rightRotate(t->right);
            return leftRotate(t);
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