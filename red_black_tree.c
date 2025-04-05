#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>


// Enum for node color
typedef enum
{
    RED,
    BLACK
} Color;

typedef struct Node
{
    int key;             // Node's key (used for ordering)
    void *value;         // Pointer to stored data
    struct Node *left;   
    struct Node *right;  
    struct Node *parent; 
    Color color;         // Node's color (RED or BLACK)
} Node;


typedef struct RedBlackTree
{
    Node *root;
} RedBlackTree;

// Function prototypes
void rb_insert(RedBlackTree *tree, int key, void *value);
void insert_fixup(RedBlackTree *tree, Node *node);
void left_rotate(RedBlackTree *tree, Node *x);
void right_rotate(RedBlackTree *tree, Node *x);
void rb_delete(RedBlackTree *tree, int key);
void delete_node(RedBlackTree *tree, Node *z);
void delete_fixup(RedBlackTree *tree, Node *x);
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
    node->color = RED; // New nodes are always red initially
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

    // If tree is empty, new node becomes root
    if (!tree->root)
    {
        tree->root = new_node;
    }
    else
    {
        // Find insertion position
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

        // Set parent pointer and insert as left/right child
        new_node->parent = parent;
        if (key < parent->key)
            parent->left = new_node;
        else
            parent->right = new_node;
    }

    // Fix any Red-Black Tree misbalances
    insert_fixup(tree, new_node);
}

void insert_fixup(RedBlackTree *tree, Node *node)
{
    // Continue fixing while parent is red (violation)
    while (get_color(node->parent) == RED)
    {
        // Parent is left child of grandparent
        if (node->parent == node->parent->parent->left)
        {
            Node *uncle = node->parent->parent->right;

            // Case 1: Uncle is red - recolor only
            if (get_color(uncle) == RED)
            {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent; // Move up to grandparent
            }
            else
            {
                // Case 2: Uncle is black and node is right child - left rotate
                if (node == node->parent->right)
                {
                    node = node->parent;
                    left_rotate(tree, node);
                }
                // Case 3: Uncle is black and node is left child - recolor and right rotate
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                right_rotate(tree, node->parent->parent);
            }
        }
        else
        {
            // Mirror cases when parent is right child of grandparent
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

    // Ensure root is always black
    tree->root->color = BLACK;
}

void left_rotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->right; // y is x's right child

    // Turn y's left subtree into x's right subtree
    x->right = y->left;
    if (y->left)
        y->left->parent = x;

    // Link x's parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y; // x was root
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // Put x on y's left
    y->left = x;
    x->parent = y;
}

void right_rotate(RedBlackTree *tree, Node *x)
{
    Node *y = x->left; // y is x's left child

    // Turn y's right subtree into x's left subtree
    x->left = y->right;
    if (y->right)
        y->right->parent = x;

    // Link x's parent to y
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y; // x was root
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    // Put x on y's right
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

// helper function for replace node with successor
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

// find min node (successor) for replacement
Node *find_max(Node *node)
{
    while (node->right != NULL)
        node = node->right;
    return node;
}

// helper function for delete
void delete_node(RedBlackTree *tree, Node *z)
{
    Node *y = z;
    Node *x;
    Color y_original_color = y->color;

    // Case 1: No left child
    if (z->left == NULL)
    {
        x = z->right;
        transplant(tree, z, z->right);
    }
    // Case 2: No right child
    else if (z->right == NULL)
    {
        x = z->left;
        transplant(tree, z, z->left);
    }
    // Case 3: Two children
    else
    {
        y = find_max(z->left);
        y_original_color = y->color;
        x = y->left;

        // If predecessor isn't direct child
        if (y->parent != z)
        {
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
    if (y_original_color == BLACK)
    {
        delete_fixup(tree, x);
    }
}

// fixes tree after deletion
void delete_fixup(RedBlackTree *tree, Node *x)
{
    // Continue fixing until we reach root or x is red
    while (x != tree->root && get_color(x) == BLACK)
    {
        Node *parent = x ? x->parent : tree->root;

        // Case: x is left child
        if (x == parent->left)
        {
            Node *sibling = parent->right;

            // Case 1: Sibling is red (must have black children)
            if (get_color(sibling) == RED)
            {
                sibling->color = BLACK;
                parent->color = RED;
                left_rotate(tree, parent);
                sibling = parent->right;
            }

            // Case 2: Both sibling's children are black

            if (get_color(sibling->left) == BLACK &&
                get_color(sibling->right) == BLACK)
            {
                sibling->color = RED;
                x = parent; // Move up to parent
            }
            else
            {
                // Case 3: Sibling's right child is black (left is red)
                if (get_color(sibling->right) == BLACK)
                {
                    if (sibling->left)
                        sibling->left->color = BLACK;
                    sibling->color = RED;
                    right_rotate(tree, sibling);
                    sibling = parent->right;
                }

                // Case 4: Sibling's right child is red

                sibling->color = parent->color;
                parent->color = BLACK;
                if (sibling->right)
                    sibling->right->color = BLACK;
                left_rotate(tree, parent);
                x = tree->root; // Terminate loop
            }
        }
        // Mirror cases for right child
        else
        {
            Node *sibling = parent->left;

            if (get_color(sibling) == RED)
            {
                sibling->color = BLACK;
                parent->color = RED;
                right_rotate(tree, parent);
                sibling = parent->left;
            }

            if (get_color(sibling->right) == BLACK &&
                get_color(sibling->left) == BLACK)
            {
                sibling->color = RED;
                x = parent;
            }
            else
            {

                if (get_color(sibling->left) == BLACK)
                {
                    if (sibling->right)
                        sibling->right->color = BLACK;
                    sibling->color = RED;
                    left_rotate(tree, sibling);
                    sibling = parent->left;
                }

                sibling->color = parent->color;
                parent->color = BLACK;
                if (sibling->left)
                    sibling->left->color = BLACK;
                right_rotate(tree, parent);
                x = tree->root;
            }
        }
    }

    // Set root color to black
    if (x)
    {
        x->color = BLACK;
    }
    else if (tree->root)
    {
        tree->root->color = BLACK;
    }
}

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

double rb_get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void test_red_black_tree(int num_operations) {
    RedBlackTree tree;
    tree.root = NULL;
    
    srand(time(NULL));
    double start, end;
    
    // Тест вставки
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        rb_insert(&tree, key, NULL);
    }
    end = rb_get_current_time();
    printf("Red-Black Tree Insert: %.6f sec\n", end - start);
    
    // Тест поиска
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        rb_search(&tree, key);
    }
    end = rb_get_current_time();
    printf("Red-Black Tree Search: %.6f sec\n", end - start);
    
    // Тест удаления
    start = rb_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        rb_delete(&tree, key);
    }
    end = rb_get_current_time();
    printf("Red-Black Tree Delete: %.6f sec\n", end - start);

    free_red_black_tree(&tree);
}


// int main()
// {
//     RedBlackTree tree;
//     tree.root = NULL;

//     rb_insert(&tree, 10, "10");
//     rb_insert(&tree, 20, "20");
//     rb_insert(&tree, 30, "30");
//     rb_insert(&tree, 40, "40");
//     rb_insert(&tree, 35, "35");

//     rb_delete (&tree, 20);

//     return 0;
// }