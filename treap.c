#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Структура данных для хранения в узле
typedef struct Data {
    char firstName[20];
    char lastName[20];
    int age;
} Data;

// Структура узла Treap
typedef struct TreapNode {
    int key;
    int priority;
    Data data;
    struct TreapNode *left;
    struct TreapNode *right;
} TreapNode;

// Создание нового узла
TreapNode* createNode(int key, Data d) {
    TreapNode* node = (TreapNode*)malloc(sizeof(TreapNode));
    node->key = key;
    node->priority = rand();
    node->data = d;
    node->left = node->right = NULL;
    return node;
}

// Разделение Treap на два по ключу
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

// Объединение двух Treap
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

// Вставка элемента
TreapNode* tt_insert(TreapNode* root, int key, Data d) {
    TreapNode* node = createNode(key, d);
    TreapNode *left = NULL, *right = NULL;
    split(root, key, &left, &right);
    return merge(merge(left, node), right);
}

// Поиск элемента
TreapNode* tt_search(TreapNode* root, int key) {
    if (!root) return NULL;
    if (key == root->key) return root;
    return key < root->key ? 
        tt_search(root->left, key) : 
        tt_search(root->right, key);
}

// Удаление элемента
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

// Удаление всего Treap
void freeTreap(TreapNode* root) {
    if (root) {
        freeTreap(root->left);
        freeTreap(root->right);
        free(root);
    }
}

// Функция для получения текущего времени
double tt_get_current_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// Тестер производительности
void test_treap(int num_operations) {
    TreapNode *root = NULL;
    Data d = {"Test", "User", 25};
    double start, end;
    
    // Тест вставки
    srand(time(NULL));
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        root = tt_insert(root, key, d);
    }
    end = tt_get_current_time();
    printf("Treap Insert: %.6f sec\n", end - start);
    
    // Тест поиска
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        tt_search(root, key);
    }
    end = tt_get_current_time();
    printf("Treap Search: %.6f sec\n", end - start);
    
    // Тест удаления
    start = tt_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        root = tt_delete(root, key);
    }
    end = tt_get_current_time();
    printf("Treap Delete: %.6f sec\n", end - start);
    
    freeTreap(root);
}

// int main() {
//     test_treap(1000000); // Тест на 10,000 операций
//     return 0;
// }