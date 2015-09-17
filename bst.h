#ifndef __BST__
#define __BST__

typedef struct bst *BST;


typedef struct node *Node;

struct bst {
    Node root; // root of tree
    int size; // number of elements
    int height; // height of tree
    Comparator compare; // compares two keys
    Destructor delete; // deletes an element
    KeyFinder keyFind; // gets the key from an element passed
    ValueFinder valueFind; // gets the value from an element passed
};


struct node {
    Node parent; // the parent to this node
    Node less; // the node to the left, or the node that is less than this one
    Node more; // the node to the right, or the node that is greater than this one
    int size; // number of elements contained in the tree below this one
    int height; // height of tree rooted at this node
    void* value; // the value of the element at this node
    void* key; // the key of the element at this node
};

// compares two elements of the tree
typedef int (*Comparator) (const void*, const void*);

// called on deletion on an element of the tree
typedef void (*Destructor) (void*);

// called on insertion to set the element's key
typedef void* (*KeyFinder) (void*);

// called on insertion to set the element's value
typedef void* (*ValueFinder) (void*);

// creates a new binary search tree with the given functions
BST newBST(Comparator c, Destructor d, KeyFinder k, ValueFinder v);

void insertElemBST(BST b, void* e); // inserts an element, extracting its key and value as determined by the KeyFinder and ValueFinder provided to b

void insertPairBST(BST b, void* k, void* v); // unsafe, better to use insertElemBST

// returns the element associated with the given key
void* searchKeyBST(BST b, void* k);

// returns true if the BST contains this key
int containsKeyBST(BST b, void* k);

void deleteKeyBST(BST b, void* k); // deletes the node containing the given key

void deleteElemBST(BST b, void* e); // deletes the node containing e's key but only if the values match

#endif
