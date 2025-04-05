#include <stdio.h>

void test_wbtree(int);
void test_red_black_tree(int);
void test_hash_table(int);

int main() {
    int test_sizes[] = {1000, 10000, 50000};
    int num_tests = sizeof(test_sizes)/sizeof(test_sizes[0]);
    
    for(int i = 0; i < num_tests; i++) {
        int n = test_sizes[i];
        printf("\n=== Testing with %d operations ===\n", n);
        
        printf("\n-- Weight-Balanced Tree --\n");
        test_wbtree(n);
        
        printf("\n-- Red-Black Tree --\n");
        test_red_black_tree(n);
        
        printf("\n-- Hash Table --\n");
        test_hash_table(n);
    }
    
    return 0;
}