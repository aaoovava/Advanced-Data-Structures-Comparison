#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// Weight-Balanced Tree Node
typedef struct WBNode {
    int key;
    void *value;
    int weight;  // Size of subtree including this node
    struct WBNode *left;
    struct WBNode *right;
} WBNode;

typedef struct WBTree {
    WBNode *root;
} WBTree;

// Balance threshold (α = 0.25 for typical BB[α] tree)
#define ALPHA 0.25
#define MIN_WEIGHT 4  // Minimum weight for rebalancing

// Create new node
WBNode* create_wbnode(int key, void *value) {
    WBNode *node = (WBNode*)malloc(sizeof(WBNode));
    node->key = key;
    node->value = value;
    node->weight = 1;
    node->left = node->right = NULL;
    return node;
}

// Update weight of node based on children
void update_weight(WBNode *node) {
    int left_weight = node->left ? node->left->weight : 0;
    int right_weight = node->right ? node->right->weight : 0;
    node->weight = 1 + left_weight + right_weight;
}

// Check if node needs rebalancing
int needs_rebalance(WBNode *node) {
    if(node->weight < MIN_WEIGHT) return 0;
    
    int left = node->left ? node->left->weight : 0;
    int right = node->right ? node->right->weight : 0;
    
    // Check α-balance condition
    return (left > (1 - ALPHA) * (left + right + 1)) ||
           (right > (1 - ALPHA) * (left + right + 1));
}

// Right rotation
WBNode* wb_right_rotate(WBNode *y) {
    WBNode *x = y->left;
    y->left = x->right;
    x->right = y;
    update_weight(y);
    update_weight(x);
    return x;
}

// Left rotation
WBNode* wb_left_rotate(WBNode *x) {
    WBNode *y = x->right;
    x->right = y->left;
    y->left = x;
    update_weight(x);
    update_weight(y);
    return y;
}

// Rebalance node if needed
WBNode* balance(WBNode *node) {
    if(!node) return node;
    
    int left = node->left ? node->left->weight : 0;
    int right = node->right ? node->right->weight : 0;
    
    if(left > (1 - ALPHA) * (left + right + 1)) {
        if(node->left->right && 
          (node->left->right->weight > ALPHA * node->weight)) {
            node->left = wb_left_rotate(node->left);
        }
        node = wb_right_rotate(node);
    }
    else if(right > (1 - ALPHA) * (left + right + 1)) {
        if(node->right->left && 
          (node->right->left->weight > ALPHA * node->weight)) {
            node->right = wb_right_rotate(node->right);
        }
        node = wb_left_rotate(node);
    }
    
    return node;
}

// Insert key into tree
WBNode* wb_insert(WBNode *root, int key, void *value) {
    if(!root) return create_wbnode(key, value);
    
    if(key < root->key) {
        root->left = wb_insert(root->left, key, value);
    } else {
        root->right = wb_insert(root->right, key, value);
    }
    
    update_weight(root);
    
    if(needs_rebalance(root)) {
        return balance(root);
    }
    return root;
}

// Search for key
WBNode* wb_search(WBNode *root, int key) {
    if(!root) return NULL;
    
    if(key == root->key) return root;
    if(key < root->key) return wb_search(root->left, key);
    return wb_search(root->right, key);
}

// Delete key from tree
WBNode* wb_delete(WBNode *root, int key) {
    if(!root) return NULL;
    
    if(key < root->key) {
        root->left = wb_delete(root->left, key);
    } else if(key > root->key) {
        root->right = wb_delete(root->right, key);
    } else {
        if(!root->left || !root->right) {
            WBNode *temp = root->left ? root->left : root->right;
            free(root);
            return temp;
        }
        
        // Find inorder predecessor
        WBNode *temp = root->left;
        while(temp->right) temp = temp->right;
        
        root->key = temp->key;
        root->value = temp->value;
        root->left = wb_delete(root->left, temp->key);
    }
    
    update_weight(root);
    
    if(needs_rebalance(root)) {
        return balance(root);
    }
    return root;
}

// Free tree memory
void free_wbtree(WBNode *root) {
    if(!root) return;
    free_wbtree(root->left);
    free_wbtree(root->right);
    free(root);
}

// Timing function
double wbt_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Test function
void test_wbtree(int num_ops) {
    WBTree tree = {NULL};
    srand(time(NULL));
    double start, end;
    
    // Insert test
    start = wbt_get_time();
    for(int i = 0; i < num_ops; i++) {
        int key = rand() % (num_ops * 10);
        tree.root = wb_insert(tree.root, key, NULL);
    }
    end = wbt_get_time();
    printf("WBTree Insert: %.6f sec\n", end - start);
    
    // Search test
    start = wbt_get_time();
    for(int i = 0; i < num_ops; i++) {
        int key = rand() % (num_ops * 10);
        wb_search(tree.root, key);
    }
    end = wbt_get_time();
    printf("WBTree Search: %.6f sec\n", end - start);
    
    // Delete test
    start = wbt_get_time();
    for(int i = 0; i < num_ops; i++) {
        int key = rand() % (num_ops * 10);
        tree.root = wb_delete(tree.root, key);
    }
    end = wbt_get_time();
    printf("WBTree Delete: %.6f sec\n", end - start);
    
    free_wbtree(tree.root);
}

// int main() {
//     printf("\n=== Weight-Balanced Tree Performance ===\n");
//     test_wbtree(1000);
//     return 0;
// }