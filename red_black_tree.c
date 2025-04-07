#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


// enum for node color
typedef enum
{
    RED,
    BLACK
} Color;

typedef struct Node
{
    int key;             
    void *value;        
    struct Node *left;   
    struct Node *right;  
    struct Node *parent; 
    Color color;         // node's color (RED or BLACK)
} Node;


typedef struct RedBlackTree
{
    Node *root;
} RedBlackTree;

// function prototypes
void rb_insert(RedBlackTree *tree, int key, void *value);
void insert_fixup(RedBlackTree *tree, Node *node);
void left_rotate(RedBlackTree *tree, Node *x);
void right_rotate(RedBlackTree *tree, Node *x);
void rb_delete(RedBlackTree *tree, int key);
void delete_node(RedBlackTree *tree, Node *z);
void delete_fixup(RedBlackTree *tree, Node *x, Node *x_parent);
void transplant(RedBlackTree *tree, Node *u, Node *v);
Node *find_min(Node *node);

Node *create_node(int key, void *value)
{
    Node *node = (Node *)malloc(sizeof(Node));

    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->color = RED; // new nodes are always red initially
    return node;
}

Color get_color(Node *node)
{
    // NULL pointers represent leaf nodes which are always black
    return (node == NULL) ? BLACK : node->color;
}

void rb_insert(RedBlackTree *tree, int key, void *value)
{
    Node *new_node = create_node(key, value);

    // if tree is empty set new node as root
    if (!tree->root)
    {
        tree->root = new_node;
    }
    else
    {
        // find insertion positioт
        Node *current = tree->root;
        Node *parent = NULL;

        while (current)
        {
            parent = current;
            if (key < current->key)
                current = current->left;
            else
                current = current->right;
        }

        // set parent pointer and insert as left/right child
        new_node->parent = parent;
        if (key < parent->key)
            parent->left = new_node;
        else
            parent->right = new_node;
    }

    // fix any Red-Black Tree misbalances
    insert_fixup(tree, new_node);
}

void insert_fixup(RedBlackTree *tree, Node *node)
{
    // continue fixing while parent is red (violation)
    while (get_color(node->parent) == RED)
    {
        // parent is left child of grandparent
        if (node->parent == node->parent->parent->left)
        {
            Node *uncle = node->parent->parent->right;

            // case 1: uncle is red - recolor only
            if (get_color(uncle) == RED)
            {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent; // Move up to grandparent to check for balance
            }
            else
            {
                // case 2: uncle is black and node is right child - left rotate
                if (node == node->parent->right)
                {
                    node = node->parent;
                    left_rotate(tree, node);
                }
                // case 3: Uncle is black and node is left child - recolor and right rotate
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                right_rotate(tree, node->parent->parent);
            }
        }
        else
        {
            // mirror cases when parent is right child of grandparent
            Node *uncle = node->parent->parent->left;

            if (get_color(uncle) == RED)
            {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    right_rotate(tree, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                left_rotate(tree, node->parent->parent);
            }
        }
    }

    // ensure root is always black
    tree->root->color = BLACK;
}

void left_rotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->right; // y is x right child

    // turn y left subtree into x right subtree
    x->right = y->left;
    if (y->left)
        y->left->parent = x;

    // link x's parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y; // x was root
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // put x on y's left
    y->left = x;
    x->parent = y;
}

void right_rotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->left; // y is x left child

    // turn y right subtree into x left subtree
    x->left = y->right;
    if (y->right)
        y->right->parent = x;

    // link x parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y; // x was root
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    //put x on y right
    y->right = x;
    x->parent = y;
}

// main fucntion for delete
void rb_delete(RedBlackTree *tree, int key)
{
    Node *temp = tree->root;

    while (temp != NULL)
    {
        if (key == temp->key)
            break;
        temp = (key < temp->key) ? temp->left : temp->right;
    }
    if (temp == NULL)
        return; // key not found
    delete_node(tree, temp);
}

// helper function for replace node with its successor
void transplant(RedBlackTree *tree, Node *u, Node *v)
{
    if (u->parent == NULL)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v != NULL)
        v->parent = u->parent;
}

// find max node (successor) for replacement
Node *find_max(Node *node)
{
    while (node->right != NULL)
        node = node->right;
    return node;
}

// helper function for delete(deling node)
void delete_node(RedBlackTree *tree, Node *z) {
    Node *y = z;
    Node *x;
    Color y_original_color = y->color;
    Node *x_parent = NULL; // to track x parent

    if (z->left == NULL) {
        x = z->right;
        x_parent = z->parent; // capture parent before transplant
        transplant(tree, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        x_parent = z->parent; // capture parent before transplant
        transplant(tree, z, z->left);
    } else {  // replace with successor
        y = find_max(z->left);
        y_original_color = y->color;
        x = y->left;
        x_parent = y->parent; // x parent is y parent initially

        if (y->parent != z) {
            transplant(tree, y, y->left);
            y->left = z->left;
            y->left->parent = y;
        }
        transplant(tree, z, y);
        y->right = z->right;
        y->right->parent = y;
        y->color = z->color;
    }

    free(z);

    if (y_original_color == BLACK) {
        // pass x and x_parent to delete_fixup
        delete_fixup(tree, x, x_parent);
    }
}
// helper function for delete (fixing up after deletion)
void delete_fixup(RedBlackTree *tree, Node *x, Node *x_parent) {
    // fix the tree until x is root or x becomes red
    while (x != tree->root && get_color(x) == BLACK) {
        if (x_parent == NULL) {
            x = tree->root;
            continue;
        }

        // case when x is left child
        if (x == x_parent->left) {
            Node *sibling = x_parent->right;
            if (sibling == NULL) {
                break; 
            }

            // case 1: sibling is red
            if (get_color(sibling) == RED) {
                sibling->color = BLACK;
                x_parent->color = RED;
                left_rotate(tree, x_parent);
                sibling = x_parent->right;
                if (sibling == NULL) {
                    break; 
                }
            }

            // case 2: both sibling's children are black
            if (get_color(sibling->left) == BLACK && 
                get_color(sibling->right) == BLACK) {
                sibling->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                // case 3: sibling's right child is black (left is red)
                if (get_color(sibling->right) == BLACK) {
                    if (sibling->left) sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(tree, sibling);
                    sibling = x_parent->right;
                }
                // case 4: sibling's right child is red
                sibling->color = x_parent->color;
                x_parent->color = BLACK;
                if (sibling->right) sibling->right->color = BLACK;
                left_rotate(tree, x_parent);
                x = tree->root;
            }
        } else {
            // mirror cases when x is right child
            Node *sibling = x_parent->left;
            if (sibling == NULL) {
                break; 
            }

            // case 1 (mirror): sibling is red
            if (get_color(sibling) == RED) {
                sibling->color = BLACK;
                x_parent->color = RED;
                right_rotate(tree, x_parent);
                sibling = x_parent->left;
                if (sibling == NULL) {
                    break; 
                }
            }

            // case 2 (mirror): both sibling's children are black
            if (get_color(sibling->right) == BLACK && 
                get_color(sibling->left) == BLACK) {
                sibling->color = RED;
                x = x_parent;
                x_parent = x->parent;
            } else {
                // case 3 (mirror): sibling's left child is black (right is red)
                if (get_color(sibling->left) == BLACK) {
                    if (sibling->right) sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(tree, sibling);
                    sibling = x_parent->left;
                }
                // case 4 (mirror): sibling's left child is red
                sibling->color = x_parent->color;
                x_parent->color = BLACK;
                if (sibling->left) sibling->left->color = BLACK;
                right_rotate(tree, x_parent);
                x = tree->root;
            }
        }
    }

    if (x != NULL) {
        x->color = BLACK;
    }
}
// main search function
Node *rb_search(RedBlackTree *tree, int key)
{
    Node *current = tree->root;
    while (current != NULL && current->key != key)
    {
        if (key < current->key)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }
    return current;
}

void free_node(Node *node) {
    if (node == NULL) return;
    
    free_node(node->left);
    free_node(node->right);
    
    free(node);
}

void free_red_black_tree(RedBlackTree *tree) {
    if (tree == NULL) return;
    
    free_node(tree->root);
    
    free(tree);
}
// ---- TESTS ----
double rb_get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}


int count_nodes(Node *node) {
    if (node == NULL) return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

void test_red_black_tree(int num_operations) {
    RedBlackTree tree;
    tree.root = NULL;
    
    srand(time(NULL));
    double start, end;
    size_t memory_used_kb = 0;
    int node_count = 0;

    // Insert
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        Node *existing = rb_search(&tree, key);
        if (!existing) {
            rb_insert(&tree, key, NULL);
        }
    }
    end = rb_get_current_time();
    node_count = count_nodes(tree.root);
    memory_used_kb = (node_count * sizeof(Node) + sizeof(RedBlackTree)) / 1024; 
    printf("Insert: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start) / num_operations);

    // Search
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        rb_search(&tree, key);
    }
    end = rb_get_current_time();
    printf("Search: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start) / num_operations);

    // Delete
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        rb_delete(&tree, key);
    }
    end = rb_get_current_time();
    printf("Delete: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start) / num_operations);

    printf("Memory used: %zu KB (for %d nodes)\n", memory_used_kb, node_count);

    free_node(tree.root);
    tree.root = NULL;
}


// int main()
// {

    
//     test_red_black_tree(1000000);

//     return 0;
// }