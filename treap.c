#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct Data {
    char firstName[20];
    char lastName[20];
    int age;
} Data;

// Main struct for Treap
typedef struct TreapNode {
    int key;
    int priority; // Random value
    Data data;
    struct TreapNode *left;
    struct TreapNode *right;
} TreapNode;

// Helper function for creating TreapNode
TreapNode* createNode(int key, Data d) {
    TreapNode* node = (TreapNode*)malloc(sizeof(TreapNode));
    node->key = key;
    node->priority = rand();
    node->data = d;
    node->left = node->right = NULL;
    return node;
}

// Split Treap (Helper function)
void split(TreapNode* root, int key, TreapNode** left, TreapNode** right) {
    if (!root) {
        *left = *right = NULL;
    } else if (root->key <= key) {
        split(root->right, key, &root->right, right);
        *left = root;
    } else {
        split(root->left, key, left, &root->left);
        *right = root;
    }
}

// Merge Treap (like two sub-trees)(Helper function)
TreapNode* merge(TreapNode* left, TreapNode* right) {
    if (!left) return right;
    if (!right) return left;
    
    if (left->priority > right->priority) {
        left->right = merge(left->right, right);
        return left;
    } else {
        right->left = merge(left, right->left);
        return right;
    }
}

// Main insert function
TreapNode* tt_insert(TreapNode* root, int key, Data d) {
    TreapNode* node = createNode(key, d);
    TreapNode *left = NULL, *right = NULL;
    split(root, key, &left, &right);
    return merge(merge(left, node), right);
}

// Reqursive search in tree
TreapNode* tt_search(TreapNode* root, int key) {
    if (!root) return NULL;
    if (key == root->key) return root;
    return key < root->key ? 
        tt_search(root->left, key) : 
        tt_search(root->right, key);
}

// Main delete function
TreapNode* tt_delete(TreapNode* root, int key) {
    if (!root) return NULL;
    
    if (root->key == key) {
        TreapNode* temp = merge(root->left, root->right);
        free(root);
        return temp;
    }
    
    if (key < root->key) {
        root->left = tt_delete(root->left, key);
    } else {
        root->right = tt_delete(root->right, key);
    }
    return root;
}

// Free Treap
void freeTreap(TreapNode* root) {
    if (root) {
        freeTreap(root->left);
        freeTreap(root->right);
        free(root);
    }
}

// ---- TESTS ---
double tt_get_current_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}


int tt_count_nodes(TreapNode* root) {
    if (!root) return 0;
    return 1 + tt_count_nodes(root->left) + tt_count_nodes(root->right);
}

void test_treap(int num_operations) {
    TreapNode *root = NULL;
    Data d = {"Test", "User", 25};
    double start, end;
    int node_count = 0;
    size_t memory_used_kb = 0;

    // Insert
    srand(time(NULL));
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        root = tt_insert(root, key, d);
    }
    end = tt_get_current_time();
    printf("Insert: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    // Memory usage calc
    node_count = tt_count_nodes(root);
    memory_used_kb = (node_count * sizeof(TreapNode)) / 1024;

    // Search
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        tt_search(root, key);
    }
    end = tt_get_current_time();
    printf("Search: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    // Delete
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        root = tt_delete(root, key);
    }
    end = tt_get_current_time();
    printf("Delete: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    printf("Memory used: %zu KB\n", memory_used_kb);

    freeTreap(root);
}
// int main() {
//     test_treap(1000000); // Тест на 10,000 операций
//     return 0;
// }