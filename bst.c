#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bst.h"

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
    void* elem;
};

Node getNode(void* key, void* value, void* elem) {
    Node n = (Node) malloc(sizeof(struct node));
    n->less = NULL;
    n->more = NULL;
    n->parent = NULL;
    n->size = 1;
    n->height = 0;
    n->value = value;
    n->key = key;
    n->elem = elem;
    return n;
}

BST createBST(Comparator c, Destructor d, KeyFinder k, ValueFinder v) { 
    BST b = (BST) malloc(sizeof(struct bst));
    b->root = NULL;
    b->size = 0;
    b->height = 0;
    b->compare = c;
    b->delete = d;
    b->keyFind = k;
    b->valueFind = v;
    return b;
}

int getheight(Node n) {
    if (n == NULL) {
        return -1; 
    } else return n->height;
}

Node rotateRight(BST b, Node P) {
    Node X = P->parent;
    Node Q = P->more;
    if (Q == NULL) // there is no rotation to be had
        return P;
    Node B = Q->less;
    if (X != NULL) { // deal with your parents first
        if (X->less == P)
            X->less = Q;
        else if (X->more == P)
            X->more = Q;
    }
    Q->parent = X;
    Q->less = P; 
    P->parent = Q;
    if (B != NULL) 
        B->parent = P;
    P->more = B;
    int swap = Q->size;
    Q->size = P->size;
    P->size = swap;

    { // update heights
        int lessHeight, moreHeight;
        lessHeight = getheight(P->less);
        moreHeight = getheight(P->more);
        if (lessHeight > moreHeight) P->height = lessHeight + 1;
        else P->height = moreHeight + 1; 
        lessHeight = getheight(Q->less);
        moreHeight = getheight(Q->more);
        if (lessHeight > moreHeight) Q->height = lessHeight + 1;
        else Q->height = moreHeight + 1; 
    } 
    if (b->root == P) {
        b->root = Q;  
        b->size = Q->size;
    }
    return Q;
}

Node rotateLeft(BST b, Node Q) {
    Node X = Q->parent;
    Node P = Q->less;
    if (P == NULL) 
        return Q;
    Node B = P->more;
    if (X != NULL) { // deal with your parents first
        if (X->less == Q)
            X->less = P;
        else if (X->more == Q)
            X->more = P;
    }
    P->parent = X;
    P->more = Q;
    Q->parent = P;
    if (B != NULL)
        B->parent = Q;
    Q->less = B;
    int swap = P->size;
    P->size = Q->size;
    Q->size = swap;
    { // update heights
        int lessHeight, moreHeight;
        lessHeight = getheight(Q->less);
        moreHeight = getheight(Q->more);
        if (lessHeight > moreHeight) Q->height = lessHeight + 1;
        else Q->height = moreHeight + 1; 
        lessHeight = getheight(P->less);
        moreHeight = getheight(P->more);
        if (lessHeight > moreHeight) P->height = lessHeight + 1;
        else P->height = moreHeight + 1; 
    } 
    if (b->root == Q) {
        b->root = P;
        b->size = P->size;
        b->height = P->height;
    }
    return P;
}

int getsize(Node n) {
    if (n == NULL) return 0;
    else return n->size;
}


void insertElemBST(BST b, void* e) {
    void* key = b->keyFind(e);
    void* value = b->valueFind(e);
    Comparator compare = b->compare;
    Node root = b->root;
    if (root == NULL) {
        b->root = getNode(key, value, e);
        return;
    } else {
        Node parent = b->root;
        int lesschild = 0;
        int depth = 0;
        // depth first search the tree based on the bst property
        while (root != NULL) {
            int comparison = compare(key, root->key);
            if (comparison > 0) { // if key greater than root->key, go right/more
                parent = root;
                root = root->more;
                lesschild = 0;
                depth++;
            } else if (comparison < 0) { // else if key less than root->key, go left/less
                parent = root;
                root = root->less;
                lesschild = 1;
                depth++;
            } else { // if key is equal to root->key, set the value of the root to be the value of e
                root->value = value; 
                return;
            }
        }
        if (lesschild) { // if the last comparison took us left/less, place the new node on the less node of the parent
            parent->less = getNode(key, value, e); 
            parent->less->parent = parent;
        } else { // if the last comparison took us right/more, place the new node on the more node of the parent
            parent->more = getNode(key, value, e);
            parent->more->parent = parent;
        }
        int height = 0; 
        while (parent != NULL) { // TODO implement rebalancing
            parent->size++;
            if (parent->height < height)
                parent->height = height;
            depth--;
            Node less = parent->less;
            Node more = parent->more; 
            if (getheight(less) > getheight(more)) {
                parent = rotateLeft(b, parent);
            } else if (getheight(more) > getheight(less)) {
                parent = rotateRight(b, parent); 
            }
            parent = parent->parent; 
            height++;
        }
        b->size = b->root->size;
        b->height = b->root->height;
    }
}

/*
void insertPairBST(BST b, void* key, void* value) {
    Comparator compare = b->compare;
    Node root = b->root;
    if (root == NULL) {
        b->root = getNode(key, value);
        return;
    } else {
        Node parent = b->root;
        int lesschild = 0;
        int depth = 0;
        // depth first search the tree based on the bst property
        while (root != NULL) {
            int comparison = compare(key, root->key);
            if (comparison > 0) { // if key greater than root->key, go right/more
                parent = root;
                root = root->more;
                lesschild = 0;
                depth++;
            } else if (comparison < 0) { // else if key less than root->key, go left/less
                parent = root;
                root = root->less;
                lesschild = 1;
                depth++;
            } else { // if key is equal to root->key, set the value of the root to be the value of e
                root->value = value; 
                return;
            }
        }
        if (lesschild) { // if the last comparison took us left/less, place the new node on the less node of the parent
            parent->less = getNode(key, value); 
            parent->less->parent = parent;
        } else { // if the last comparison took us right/more, place the new node on the more node of the parent
            parent->more = getNode(key, value);
            parent->more->parent = parent;
        }
        int height = 0; 
        while (parent != NULL) { // TODO implement rebalancing
            parent->size++;
            if (parent->height < height)
                parent->height = height;
            depth--;
            Node less = parent->less;
            Node more = parent->more; 
            if (getheight(less) > getheight(more)) {
                parent = rotateLeft(b, parent);
            } else if (getheight(more) > getheight(less)) {
                parent = rotateRight(b, parent); 
            }
            parent = parent->parent; 
            height++;
        }
        b->size = b->root->size;
        b->height = b->root->height;
    }
}*/

int containsKeyBST(BST b, void* key) {
    Node root = b->root;
    Comparator compare = b->compare;
    while (root != NULL) {
        int comparison = compare(key, root->key);
        if (comparison > 0) {
            root = root->more;
        } else if (comparison < 0) {
            root = root->less;
        } else {
            return 1;
        }
    }
    return 0;
}

void* searchKeyBST(BST b, void* key) {
    Node root = b->root;
    Comparator compare = b->compare;
    while (root != NULL) {
        int comparison = compare(key, root->key);
        if (comparison > 0) {
            root = root->more;
        } else if (comparison < 0) {
            root = root->less;
        } else {
            return root->value;
        }
    }
    return NULL;
}

/* TODO
void* deleteKeyBST(BST b, void* key) { // returns the value associated or NULL if it isn't there
    if (b->root == NULL) return NULL;
    Node root = b->root;
    Comparator compare = b->compare;
    int lesschild = -1;
    while (root != NULL) {
        int comparison = compare(key, root->key); 
        if (comparison > 0) {
            root = root->more;
            lesschild = 0;
        } else if (comparison < 0) {
            root = root->less;
            lesschild = 1;
        } else { // you found it!
            Node replacement;
            if (root->less == NULL && root->more == NULL) { // leaf
                replacement == NULL;
                free(root);
            } else if (root->less == NULL) {
                Node hunter = root->more;
                while (hunter->more != NULL) {
                    hunter = hunter->more;               
                } // go to the farthest right node
            } else if (root->more == NULL) {
                Node hunter = root->less;
                while (hunter->less != NULL) {
                    hunter = hunter->less;
                } // go to the farthest left node
            } else {
            }
        }
    }
}*/

int testComparator(const void* k1, const void* k2) {
    int* a = (int*) k1;
    int* b = (int*) k2;
    return (*a) - (*b);
}

void testDestructor(void* e) {
    free(e);
}

void* testKeyFinder(void* e) {
    return e;
}

void* testValueFinder(void* e) {
    return e;
}

void nodeTraverse(Node n, int depth) {
    if (n == NULL) return;
    int i = 0;
    nodeTraverse(n->less, depth + 1);
    for (i; i < depth; i++) printf("  ");
    printf("key: %d depth: %d size: %d\n", *(int*)n->key, depth, n->size);
    nodeTraverse(n->more, depth + 1);
}

void traverse(BST b) {
    printf("traversal\n");
    nodeTraverse(b->root, 0);   
}

double maxDepthH(Node n, int depth) {
    if (n == NULL) {
       return depth - 1;
    } else {
       int depth_less = maxDepthH(n->less, depth + 1);
       int depth_more = maxDepthH(n->more, depth + 1);
       if (depth_less > depth_more) {
           return depth_less;
       } else return depth_more;
    }
}

int maxDepth(Node n) {
    return maxDepthH(n, 0);
}

void rotationTraversal(BST b, Node n, int depth) {
    if (n == NULL) {
        return;
    } 
    rotationTraversal(b, n->less, depth + 1);
    if (rand() > RAND_MAX / 2){
        rotateRight(b, n);
    } else {
        rotateLeft(b, n);
    }
    rotationTraversal(b, n->more, depth + 1);
}

static int i, N;

int main(int argc, char* argv[]) {
    BST b = createBST(&testComparator, &testDestructor, &testKeyFinder, &testValueFinder);
    char cmd;
    i = 0; N = 1000;
    int* key = (int*) malloc(sizeof(int));
    while (1) { 
        scanf("%c", &cmd);
        if (cmd == 'i') {
            scanf("%d", key);
            int* new_key = (int*) malloc(sizeof(int));
            *new_key = *key;
            naive_insert(b, (void*) new_key);
            printf("inserted %d!\n", *key);
        } else if (cmd == 'c') {
            if (containsKey(b, (void*) key)) {
                printf("yes!\n");
            } else {
                printf("no!\n");
            }
        } else if (cmd == 't') {
            traverse(b);
        } else if (cmd == 'd') {
            printf("%d\n", maxDepth(b->root));
        } else if (cmd == 'r') {
            for (; i < N; i++) {
                int* new_key = (int*) malloc(sizeof(int));
                if (new_key == NULL) break;
                *new_key = rand();
                naive_insert(b, (void*) new_key);
                assert(containsKey(b, new_key));
            }
            N += 1000;
            printf("size %d depth %d height %d\n", b->size, maxDepth(b->root), b->height);    
        }
    } 
}
