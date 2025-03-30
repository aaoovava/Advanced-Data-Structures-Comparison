#include <stdlib.h>

// Enum for node color
typedef enum { 
    RED,    
    BLACK   
} Color;

typedef struct Node {
    int key;                // Node's key (used for ordering)
    void *value;            // Pointer to stored data 
    struct Node *left;      // Pointer to left child
    struct Node *right;     // Pointer to right child
    struct Node *parent;    // Pointer to parent node
    Color color;            // Node's color (RED or BLACK)
} Node;

// Structure for Red-Black Tree (wrapper for root pointer)
typedef struct RedBlackTree {
    Node *root;             // Pointer to the root node of the tree
} RedBlackTree;

// Function prototypes
void insert(RedBlackTree *tree, int key, void *value);
void insert_fixup(RedBlackTree *tree, Node *node);
void left_rotate(RedBlackTree *tree, Node *x);
void right_rotate(RedBlackTree *tree, Node *x);
void delete(RedBlackTree *tree, int key);
void delete_node(RedBlackTree *tree, Node *z);
void delete_fixup(RedBlackTree *tree, Node *x);
void transplant(RedBlackTree *tree, Node *u, Node *v);
Node *find_min(Node *node);

Node* create_node(int key, void *value) {
    Node *node = (Node*)malloc(sizeof(Node));

    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->color = RED;  // New nodes are always red initially
    return node;
}


Color get_color(Node *node) {
    // NULL pointers represent leaf nodes which are always black
    return (node == NULL) ? BLACK : node->color;
}


void insert(RedBlackTree *tree, int key, void *value) {
    Node *new_node = create_node(key, value);
    
    // If tree is empty, new node becomes root
    if (!tree->root) {
        tree->root = new_node;
    } else {
        // Find insertion position
        Node *current = tree->root;
        Node *parent = NULL;
        
        while (current) {
            parent = current;
            if (key < current->key)
                current = current->left;
            else
                current = current->right;
        }
        
        // Set parent pointer and insert as left/right child
        new_node->parent = parent;
        if (key < parent->key)
            parent->left = new_node;
        else
            parent->right = new_node;
    }
    
    // Fix any Red-Black Tree property violations
    insert_fixup(tree, new_node);
}

void insert_fixup(RedBlackTree *tree, Node *node) {
    // Continue fixing while parent is red (violation)
    while (get_color(node->parent) == RED) {
        // Parent is left child of grandparent
        if (node->parent == node->parent->parent->left) {
            Node *uncle = node->parent->parent->right;
            
            // Case 1: Uncle is red - recolor only
            if (get_color(uncle) == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;  // Move up to grandparent
            } else {
                // Case 2: Uncle is black and node is right child - left rotate
                if (node == node->parent->right) {
                    node = node->parent;
                    left_rotate(tree, node);
                }
                // Case 3: Uncle is black and node is left child - recolor and right rotate
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                right_rotate(tree, node->parent->parent);
            }
        } else {
            // Mirror cases when parent is right child of grandparent
            Node *uncle = node->parent->parent->left;
            
            if (get_color(uncle) == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    right_rotate(tree, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                left_rotate(tree, node->parent->parent);
            }
        }
    }
    
    // Ensure root is always black
    tree->root->color = BLACK;
}

void left_rotate(RedBlackTree *tree, Node *x) {
    Node *y = x->right;  // y is x's right child
    
    // Turn y's left subtree into x's right subtree
    x->right = y->left;
    if (y->left)
        y->left->parent = x;
    
    // Link x's parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y;  // x was root
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    
    // Put x on y's left
    y->left = x;
    x->parent = y;
}


void right_rotate(RedBlackTree *tree, Node *x) {
    Node *y = x->left;  // y is x's left child
    
    // Turn y's right subtree into x's left subtree
    x->left = y->right;
    if (y->right)
        y->right->parent = x;
    
    // Link x's parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y;  // x was root
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    
    // Put x on y's right
    y->right = x;
    x->parent = y;
}

// main fucntion for delete
void delete(RedBlackTree *tree, int key) {
    Node *temp = tree->root;

    while (temp != NULL) {
        if (key == temp->key) break;
        temp = (key < temp->key) ? temp->left : temp->right;
    }
    if (temp == NULL) return; // key not found
    delete_node(tree, temp);
}

// helper function for replace node with successor
void transplant(RedBlackTree *tree, Node *u, Node *v) {
    if (u->parent == NULL)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v != NULL)
        v->parent = u->parent;
}

// find min node (successor) for replacement
Node *find_max(Node *node) {
    while (node->right != NULL)
        node = node->right;
    return node;
}

// helper function for delete
void delete_node(RedBlackTree *tree, Node *z) {
    Node *y = z;                // Node that will replace z
    Node *x;                    // Node that will replace y
    Color y_original_color = y->color;

    // Case 1: No left child
    if (z->left == NULL) {
        x = z->right;
        transplant(tree, z, z->right);
    } 
    // Case 2: No right child
    else if (z->right == NULL) {
        x = z->left;
        transplant(tree, z, z->left);
    } 
    // Case 3: Two children
    else {
        y = find_max(z->left);  // Find predecessor (max in left subtree)
        y_original_color = y->color;
        x = y->left;

        // If predecessor isn't direct child
        if (y->parent != z) {
            transplant(tree, y, y->left);
            y->left = z->left;
            y->left->parent = y;
        }

        // Replace z with y
        transplant(tree, z, y);
        y->right = z->right;
        y->right->parent = y;
        y->color = z->color;
    }

    free(z);

    // Fix violations if deleted node was black
    if (y_original_color == BLACK) {
        delete_fixup(tree, x);
    }
}

// fixes tree after deletion
void delete_fixup(RedBlackTree *tree, Node *x) {
    // Continue fixing until we reach root or x is red
    while (x != tree->root && get_color(x) == BLACK) {
        Node *parent = x ? x->parent : tree->root;

        // Case: x is left child
        if (x == parent->left) {
            Node *sibling = parent->right;
            
            // Case 1: Sibling is red (must have black children)
            if (get_color(sibling) == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                left_rotate(tree, parent);
                sibling = parent->right;  
            }

            // Case 2: Both sibling's children are black
             
            if (get_color(sibling->left) == BLACK && 
                get_color(sibling->right) == BLACK) {
                sibling->color = RED;
                x = parent;  // Move up to parent
            } 
            else {
                // Case 3: Sibling's right child is black (left is red) 
                if (get_color(sibling->right) == BLACK) {
                    if (sibling->left) sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(tree, sibling);
                    sibling = parent->right;
                }
                
                // Case 4: Sibling's right child is red
                
                sibling->color = parent->color;
                parent->color = BLACK;
                if (sibling->right) sibling->right->color = BLACK;
                left_rotate(tree, parent);
                x = tree->root;  // Terminate loop
            }
        } 
        // Mirror cases for right child
        else {
            Node *sibling = parent->left;
            
            // Case 1 (mirror)
            if (get_color(sibling) == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                right_rotate(tree, parent);
                sibling = parent->left;
            }

            // Case 2 (mirror)
            if (get_color(sibling->right) == BLACK && 
                get_color(sibling->left) == BLACK) {
                sibling->color = RED;
                x = parent;
            } 
            else {
                // Case 3 (mirror)
                if (get_color(sibling->left) == BLACK) {
                    if (sibling->right) sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(tree, sibling);
                    sibling = parent->left;
                }
                
                // Case 4 (mirror)
                sibling->color = parent->color;
                parent->color = BLACK;
                if (sibling->left) sibling->left->color = BLACK;
                right_rotate(tree, parent);
                x = tree->root;
            }
        }
    }

    /* Final cleanup:
     * - If x exists, make it black (could be red after case 4)
     * - Ensure root is black (handles NULL x case) */
    if (x) {
        x->color = BLACK;
    } else if (tree->root) {
        tree->root->color = BLACK;
    }
}

Node *search(RedBlackTree *tree,int key) {
    Node *current = tree->root;
    while (current != NULL && current->key != key) {
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current;
}

int main() {
    RedBlackTree tree;
    tree.root = NULL;
    
    insert(&tree, 10, "10");
    insert(&tree, 20, "20");
    insert(&tree, 30, "30");
    insert(&tree, 40, "40");
    insert(&tree, 35, "35");


    delete(&tree, 20);

    
    
    return 0;
}