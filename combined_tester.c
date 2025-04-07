#include <stdio.h>

void test_treap(int);
void test_red_black_tree(int);
void test_hash_table(int);


// Help for build command: gcc -o tester combined_tester.c red_black_tree.c hash_table.c treap.c  -lm; )
//need to be in the same directory
int main() {
    int test_sizes[] = { 10000, 100000,1000000, 5000000};
    int num_tests = sizeof(test_sizes)/sizeof(test_sizes[0]);
    
    for(int i = 0; i < num_tests; i++) {
        int n = test_sizes[i];
        printf("\n=== Testing with %d operations ===\n", n);
        
        printf("\n-- Treap Tree --\n");
        test_treap(n);
        
        printf("\n-- Red-Black Tree --\n");
        test_red_black_tree(n);
        
        printf("\n-- Hash Table --\n");
        test_hash_table(n);
    }
    
    return 0;
}