#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


#define MAX_KEYS 2
#define MAX_CHILDREN 3

typedef struct Node
{
    int keys[3]; 
    struct Node *children[4]; // 0 - left child, 1 - middle child, 2 - right child (4 need for split)
    int key_count;
    int child_count;
    int is_leaf;
} Node;

typedef struct
{
    Node *root;
} TwoThreeTree;

// Function prototypes
void split(TwoThreeTree *tree, Node *node);
Node *ttt_search(TwoThreeTree *tree, int key);
Node *searchPredecessor(Node *node, TwoThreeTree *tree);
void fillSuccessor(Node *successor, TwoThreeTree *tree);
void insertKey(Node *node, int key, TwoThreeTree *tree);
void ttt_insert(TwoThreeTree *tree, int key);
void deleteKey(Node *node, int key);
void ttt_delete(TwoThreeTree *tree, int key);
int isContains(Node *node, int key);
void replaseBySuccessor(Node *node, int key, TwoThreeTree *tree);
void printTree(TwoThreeTree *tree);
void join(Node *successor, Node *joinNode);
void share(Node *successor, Node *predecessor, Node *shareNode);

// Helper function to insert a key into a node
void insertKey(Node *node, int key, TwoThreeTree *tree) {
    if (node->key_count == 2) {
        int temp_keys[3] = {node->keys[0], node->keys[1], key};
        
        // sorting
        for (int i = 0; i < 2; i++) {
            for (int j = i+1; j < 3; j++) {
                if (temp_keys[i] > temp_keys[j]) {
                    int temp = temp_keys[i];
                    temp_keys[i] = temp_keys[j];
                    temp_keys[j] = temp;
                }
            }
        }
        
        // update keys
        node->keys[0] = temp_keys[0];
        node->keys[1] = temp_keys[1];
        node->keys[2] = temp_keys[2]; 
        
        node->key_count = 3;
        split(tree, node);
    } 
    else if (node->key_count == 1) {
        if (key < node->keys[0]) {
            node->keys[1] = node->keys[0];
            node->keys[0] = key;
        } else {
            node->keys[1] = key;
        }
        node->key_count = 2;
    } 
    else {
        node->keys[0] = key;
        node->key_count = 1;
    }
}

//The bigest split function ever (handle all cases)
void split(TwoThreeTree *tree, Node *node)
{
    Node *temp = tree->root;
    int key = node->keys[1];
    int keyForSplit = node->keys[0];
    int indexForFoundChild = 0;
    int isRoot;

    if (temp->is_leaf && key == temp->keys[1]) // if root is leaf
    {
        Node *newRoot = malloc(sizeof(Node));
        Node *newLeft = malloc(sizeof(Node));
        Node *newRight = malloc(sizeof(Node));

        newRoot->key_count = 1;
        newLeft->key_count = 1;
        newRight->key_count = 1;
        newRoot->is_leaf = 0;
        newLeft->is_leaf = 1;
        newRight->is_leaf = 1;
        newRoot->child_count = 2;

        newRoot->keys[0] = temp->keys[1];
        newRight->keys[0] = temp->keys[2];
        newLeft->keys[0] = temp->keys[0];
        newRoot->children[0] = newLeft;
        newRoot->children[2] = newRight;
        temp = newRoot;
        tree->root = temp;
        return;
    }
    if (key == temp->keys[1]) //spliting root when root is not leaf
    {
        Node *newRoot = malloc(sizeof(Node));
        Node *newLeft = malloc(sizeof(Node));
        Node *newRight = malloc(sizeof(Node));

        insertKey(newRoot, temp->keys[1], tree);
        insertKey(newLeft, temp->keys[0], tree);
        insertKey(newRight, temp->keys[2], tree);

        newLeft->children[0] = temp->children[0];
        newLeft->children[2] = temp->children[1];
        newLeft->child_count = 2;

        newRight->children[0] = temp->children[2];
        newRight->children[2] = temp->children[3];
        newRight->child_count = 2;

        newRoot->child_count = 2;

        newRoot->children[0] = newLeft;
        newRoot->children[2] = newRight;
        newRoot->is_leaf = 0;
        tree->root = newRoot;
        return;
    }

    int leftMiddleRight = 0; // 0 - left, 1 - middle, 2 - right
    // searching for predecessor
    while (1)
    {
        if (temp == NULL)
        {
            printTree(tree);
            exit(0);
        }
        if (keyForSplit < temp->keys[0])
        {
            if (temp->children[0]->keys[0] == keyForSplit)
            {
                leftMiddleRight = 0;
                indexForFoundChild = 0;
                break;
            }
            temp = temp->children[0];
            continue;
        }

        if (keyForSplit > temp->keys[0] && temp->keys[1] == 0)
        {
            if (temp->children[2]->keys[0] == keyForSplit)
            {
                leftMiddleRight = 2;
                indexForFoundChild = 2;
                break;
            }
            temp = temp->children[2];
            continue;
        }

        if (keyForSplit > temp->keys[0] && keyForSplit < temp->keys[1] && temp->keys[1] != 0)
        {
            if (temp->children[1]->keys[0] == keyForSplit)
            {
                leftMiddleRight = 1;
                indexForFoundChild = 1;
                break;
            }
            temp = temp->children[1];
            continue;
        }

        if (keyForSplit > temp->keys[1])
        {
            if (temp->children[2]->keys[0] == keyForSplit)
            {
                leftMiddleRight = 2;
                indexForFoundChild = 2;
                break;
            }
            temp = temp->children[2];
            continue;
        }

        temp->child_count += 2;
    }
    //simple case < 4 children
    if (temp->children[indexForFoundChild]->child_count <= 3)
    {
        // left
        switch (leftMiddleRight)
        {
        case 0:
        {
            Node *newLeftOne = malloc(sizeof(Node));
            Node *newLeftTwo = malloc(sizeof(Node));

            newLeftOne->key_count = 1;
            newLeftTwo->key_count = 1;

            newLeftOne->keys[0] = node->keys[0];
            newLeftTwo->keys[0] = node->keys[2];

            if (temp->children[indexForFoundChild]->is_leaf)
            {
                newLeftOne->is_leaf = 1;
                newLeftTwo->is_leaf = 1;
            }

            if (temp->child_count > 2)
            {
                temp->children[3] = temp->children[2];
                temp->children[2] = temp->children[1];
                temp->children[0] = newLeftOne;
                temp->children[1] = newLeftTwo;
            }

            else
            {
                temp->children[0] = newLeftOne;
                temp->children[1] = newLeftTwo;
            }

            if (temp->child_count >= 2)
                temp->child_count += 1;
            else
                temp->child_count += 2;

            insertKey(temp, key, tree);

            break;
        }
        // middle
        case 2:
        {
            Node *newRightOne = malloc(sizeof(Node));
            Node *newRightTwo = malloc(sizeof(Node));

            newRightOne->key_count = 1;
            newRightTwo->key_count = 1;

            newRightOne->keys[0] = node->keys[0];
            newRightTwo->keys[0] = node->keys[2];

            if (temp->children[indexForFoundChild]->is_leaf)
            {
                newRightOne->is_leaf = 1;
                newRightTwo->is_leaf = 1;
            }

            if (temp->child_count > 2)
            {
                temp->children[2] = newRightOne;
                temp->children[3] = newRightTwo;

                if (temp->child_count >= 2)
                    temp->child_count += 1;
                else
                    temp->child_count += 2;

                insertKey(temp, key, tree);

                break;
            }

            temp->children[1] = newRightOne;
            temp->children[2] = newRightTwo;

            if (temp->child_count >= 2)
                temp->child_count += 1;
            else
                temp->child_count += 2;

            insertKey(temp, key, tree);

            break;
        }
        // right
        case 1:
        {
            Node *middleOne = malloc(sizeof(Node));
            Node *middleTwo = malloc(sizeof(Node));

            middleOne->key_count = 1;
            middleTwo->key_count = 1;

            if (temp->children[indexForFoundChild]->is_leaf)
            {
                middleOne->is_leaf = 1;
                middleTwo->is_leaf = 1;
            }

            middleOne->keys[0] = node->keys[0];
            middleTwo->keys[0] = node->keys[2];

            if (temp->child_count > 2)
            {
                temp->children[3] = temp->children[2];
                temp->children[1] = middleOne;
                temp->children[2] = middleTwo;

                if (temp->child_count >= 2)
                    temp->child_count += 1;
                else
                    temp->child_count += 2;

                insertKey(temp, key, tree);

                break;
            }

            temp->children[1] = middleOne;
            temp->children[2] = middleTwo;

            if (temp->child_count >= 2)
                temp->child_count += 1;
            else
                temp->child_count += 2;

            insertKey(temp, key, tree);

            break;
        }
        }
    }
    // wrost case we had 4 children
    else
    {
        Node *newLeft = malloc(sizeof(Node));
        Node *newRight = malloc(sizeof(Node));

        newLeft->keys[0] = node->keys[0];
        newRight->keys[0] = node->keys[2];

        if (temp->children[indexForFoundChild]->is_leaf)
        {
            newLeft->is_leaf = 1;
            newRight->is_leaf = 1;
        }

        newLeft->children[0] = node->children[0];
        newLeft->children[2] = node->children[1];
        newRight->children[0] = node->children[2];
        newRight->children[2] = node->children[3];

        newLeft->child_count = 2;
        newRight->child_count = 2;

        newLeft->key_count = 1;
        newRight->key_count = 1;

        if (leftMiddleRight == 0)
        {
            temp->children[0] = newLeft;
            temp->children[1] = newRight;
        }
        else if (leftMiddleRight == 2 || leftMiddleRight == 1)
        {
            temp->children[1] = newLeft;
            temp->children[2] = newRight;
        }

        temp->child_count += 1;

        insertKey(temp, key, tree);
    }
}

Node *addNode(Node *parent, int index)
{
    Node *newNode = malloc(sizeof(Node));

    if (parent->is_leaf)
    {
        parent->is_leaf = 0;
        newNode->is_leaf = 1;
    }

    parent->children[index] = newNode;
    parent->child_count++;

    return newNode;
}

void printTree(TwoThreeTree *tree)
{
    if (tree->root == NULL)
    {
        printf("Tree is empty.\n");
        return;
    }

    Node *queue[1000];
    int front = 0, rear = 0;
    queue[rear++] = tree->root;

    int level = 0;

    while (front < rear)
    {
        int levelSize = rear - front;
        printf("Level %d: ", level++);

        for (int i = 0; i < levelSize; i++)
        {
            Node *current = queue[front++];

            printf("[");
            for (int j = 0; j < current->key_count; j++)
            {
                printf("%d", current->keys[j]);
                if (j < current->key_count - 1)
                    printf(", ");
            }
            printf("]");

            if (current->is_leaf)
            {
                printf("(L) "); // L = Leaf
            }
            else
            {
                printf("(I) "); // I = Internal
            }

            if (!current->is_leaf)
            {

                if (current->child_count == 2)
                {
                    if (current->children[0])
                        queue[rear++] = current->children[0];
                    if (current->children[2])
                        queue[rear++] = current->children[2];
                }

                else if (current->child_count == 3)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        if (current->children[j])
                            queue[rear++] = current->children[j];
                    }
                }
            }
        }
        printf("\n");
    }
}
// Function to insert a key(main one)
void ttt_insert(TwoThreeTree *tree, int key)
{
    Node *root = tree->root;
    Node *temp = root;

    if (root == NULL) // if tree is empty
    {
        root = malloc(sizeof(Node));
        root->keys[0] = key;
        root->is_leaf = 1;
        root->key_count++;
        tree->root = root;
        return;
    }

    if (root->children[0] == NULL) // if root has no children
    {
        insertKey(root, key, tree);
        return;
    }

    // find where to insert
    while (1)
    {

        if (temp->children[0] == NULL)
        {
            insertKey(temp, key, tree);
            break;
        }

        if (key < temp->keys[0])
        {
            if (temp->children[0] == NULL)
            {
                insertKey(temp, key, tree);
                break;
            }
            temp = temp->children[0];
            continue;
        }

        if (key > temp->keys[0] && temp->keys[1] == 0)
        {
            if (temp->children[2] == NULL)
            {
                insertKey(temp, key, tree);
                break;
            }
            temp = temp->children[2];
            continue;
        }

        if (key > temp->keys[0] && key < temp->keys[1] && temp->keys[1] != 0)
        {
            if (temp->children[1] == NULL)
            {
                insertKey(temp, key, tree);
                break;
            }
            temp = temp->children[1];
            continue;
        }

        if (key > temp->keys[1])
        {
            if (temp->children[2] == NULL)
            {
                insertKey(temp, key, tree);
                break;
            }
            temp = temp->children[2];
            continue;
        }
    }
}

void freeTree(Node *node)
{
    if (node == NULL)
        return;

    for (int i = 0; i < 4; i++)
    {
        if (node->children[i] != NULL)
        {
            freeTree(node->children[i]);
        }
    }

    free(node);
}

// Helper function to delete a key from a node
void deleteKey(Node *node, int key)
{
    if (node->keys[0] == key)
    {
        node->keys[0] = node->keys[1];
        node->keys[1] = node->keys[2];
        node->key_count--;
    }
    else if (node->keys[1] == key)
    {
        node->keys[1] = node->keys[2];
        node->keys[2] = 0;
        node->key_count--;
    }
}


// Helper function to replace a key with its successor
void replaseBySuccessor(Node *node, int key, TwoThreeTree *tree)
{
    Node *successor;

    if (node->key_count == 2) //-------------------------->
    {
        if (node->keys[0] == key)
            successor = node->children[1];

        else
            successor = node->children[2];
    }
    else
        successor = node->children[2];

    while (1)
    {
        if (successor->is_leaf)
        {
            break;
        }

        successor = successor->children[0];
    } //------------------<   finding successor

    if (node->keys[0] == key)
    {
        node->keys[0] = successor->keys[0];
        successor->keys[0] = successor->keys[1];
        successor->keys[1] = 0;
        successor->key_count--;
    }
    else
    {
        node->keys[1] = successor->keys[0];
        successor->keys[0] = successor->keys[1];
        successor->keys[1] = 0;
        successor->key_count--;
    }

    if (successor->key_count == 0)
    {
        fillSuccessor(successor, tree);
    }
}

// Helper function to check if a key is in a node
int isContains(Node *node, int key)
{
    if (node->keys[0] == key || node->keys[1] == key || node->keys[2] == key)
    {
        return 1;
    }
    return 0;
}

// Function to delete a key from the tree (main one)
void ttt_delete(TwoThreeTree *tree, int key)
{
    Node *temp = tree->root;

    if (temp == NULL)
    {
        printf("Tree is empty.\n");
        return;
    }

    if (isContains(temp, key) && temp->is_leaf) // if key is in root and root is leaf
    {
        deleteKey(temp, key);
        return;
    }

    if (isContains(temp, key) && !temp->is_leaf) // if key is in root and root is not leaf
    {
        replaseBySuccessor(temp, key, tree);
        return;
    }

    temp = ttt_search(tree, key);

    if (temp == NULL)
    {
        return;
    }

    if (temp->is_leaf && temp->key_count == 2) // if key is in leaf and leaf has 2 keys
    {
        deleteKey(temp, key);
    }
    else if (temp->is_leaf && temp->key_count == 1) // if key is in leaf and leaf has 1 key
    {
        deleteKey(temp, key);
        fillSuccessor(temp, tree);
    }
    else
    {
        replaseBySuccessor(temp, key, tree);
    }

    return;
}

// Helper function to join case
void join(Node *successor, Node *joinNode)
{
    int key;

    if (successor->keys[0] > joinNode->keys[0])
    {
        successor->keys[1] = successor->keys[0];
        successor->keys[0] = joinNode->keys[0];
    }
    else
    {
        successor->keys[1] = joinNode->keys[0];
    }
    successor->key_count++;

    if (successor->child_count == 2)
    {
        successor->children[1] = successor->children[0];
        successor->children[0] = joinNode->children[0];
    }
    else
    {
        successor->children[2] = successor->children[0];
        successor->children[0] = joinNode->children[0];
        successor->children[1] = joinNode->children[2];
    }

    successor->child_count = 0;

    for (int i = 0; i < 3; i++)
    {
        if (successor->children[i] != NULL)
        {
            successor->child_count++;
        }
    }

    free(joinNode);
}

// Helper function to share case
void share(Node *successor, Node *predecessor, Node *shareNode)
{
    successor->keys[0] = predecessor->keys[0];
    successor->key_count = 1;

    predecessor->keys[0] = shareNode->keys[1];
    shareNode->keys[1] = 0;
    shareNode->key_count = 1;
}

// Helper function to fill successor
void fillSuccessor(Node *successor, TwoThreeTree *tree)
{
    if (successor == tree->root) // if successor is root
    {
        tree->root = tree->root->children[0];
        return;
    }

    Node *predecessor = searchPredecessor(successor, tree); // finding predecessor

    if (predecessor->key_count == 2) // if predecessor has 2 keys working with case 2
    {
        if (predecessor->children[2] == successor) // if successor is right child
        {

            Node *joinNode = predecessor->children[1]; // using like link for node wich will join or share with successor

            if (predecessor->children[1]->key_count == 2)
                share(successor, predecessor, joinNode);

            else
            {
                successor->keys[0] = predecessor->keys[1];
                successor->key_count = 1;
                predecessor->keys[1] = predecessor->keys[2];
                predecessor->keys[2] = 0;
                predecessor->key_count--;
                predecessor->children[1] = NULL;
                predecessor->child_count--;
                join(successor, joinNode);
            }
        }

        else if (predecessor->children[1] == successor) // if successor is middle child
        {
            Node *joinNode = predecessor->children[0];

            if (predecessor->children[1]->key_count == 2)
                share(successor, predecessor, joinNode);

            else 
            {
                successor->keys[0] = predecessor->keys[0];
                successor->key_count = 1;
                predecessor->keys[0] = predecessor->keys[1];
                predecessor->keys[1] = 0;
                predecessor->key_count--;
                predecessor->children[1] = NULL;
                predecessor->children[0] = successor;
                predecessor->child_count--;
                join(successor, joinNode);
            }
        }

        else // if successor is left child
        {
            Node *joinNode = predecessor->children[1];

            if (predecessor->children[1]->key_count == 2)
                share(successor, predecessor, joinNode);

            else
            {
                successor->keys[0] = predecessor->keys[0];
                successor->key_count = 1;
                predecessor->keys[0] = predecessor->keys[1];
                predecessor->keys[1] = predecessor->keys[2];
                predecessor->keys[2] = 0;
                predecessor->key_count--;
                predecessor->children[1] = NULL;
                predecessor->children[0] = successor;
                predecessor->child_count--;
                join(successor, joinNode);
            }
        }


        if (predecessor->key_count == 0)
        {
            fillSuccessor(predecessor, tree);
        }

        return;
    }

    if (predecessor->key_count == 1) // if predecessor has 1 key taking keys from predecessor children
    {
        Node *joinNode;
        if (predecessor->children[0] == successor)
        {
            joinNode = predecessor->children[2];
            
        }
        else
        {
            joinNode = predecessor->children[0];
            
        }

        if (joinNode->key_count == 2)
        {
            share(successor, predecessor, joinNode);
        }

        else
        {

            if (predecessor->children[0] == successor)
            {
                predecessor->children[2] = NULL;
            } else
            {
                predecessor->children[0] = NULL;
            }
            successor->keys[0] = predecessor->keys[0];
            successor->key_count = 1;
            predecessor->keys[0] = predecessor->keys[1];
            predecessor->keys[1] = 0;
            predecessor->children[0] = successor;
            predecessor->child_count--;
            predecessor->key_count--;
            join(successor, joinNode);
        }

        if (predecessor->key_count == 0)
        {
            fillSuccessor(predecessor, tree);
        }
    }
}

// Search function returns node 
Node *ttt_search(TwoThreeTree *tree, int key)
{
    Node *temp = tree->root;

    if (isContains(temp, key))
        return temp;

    while (temp != NULL)
    {
        if (isContains(temp, key))
        {
            return temp;
        }
        if (key < temp->keys[0])
        {
            temp = temp->children[0];
            continue;
        }
        if (key > temp->keys[0] && key < temp->keys[1])
        {
            temp = temp->children[1];
            continue;
        }
        if (key > temp->keys[1])
        {
            temp = temp->children[2];
            continue;
        }

        if (temp->is_leaf )
            break;
        
    }
    return NULL;
}

// Helper function to find predecessor
Node *searchPredecessor(Node *node, TwoThreeTree *tree)
{
    Node *temp = tree->root;

    if (temp == node || temp->children[0] == node || temp->children[1] == node || temp->children[2] == node)
        return temp;

    if (temp->child_count == 2)
    {
        temp = temp->children[1];
        while (temp != NULL)
        {
            if (temp->children[0 == NULL])
                break;
            if (temp->children[0] == node || temp->children[1] == node || temp->children[2] == node)
            {
                return temp;
            }
            temp = temp->children[0];
        }
        temp = tree->root;
        temp = temp->children[2];
        while (temp != NULL)
        {
            if (temp->children[0 == NULL])
                break;
            if (temp->children[0] == node || temp->children[1] == node || temp->children[2] == node)
            {
                return temp;
            }
            temp = temp->children[0];
        }
    }
    else
    {
        temp = temp->children[2];
        while (temp != NULL)
        {
            if (temp->children[0 == NULL])
                break;
            if (temp->children[0] == node || temp->children[1] == node || temp->children[2] == node)
            {
                return temp;
            }
            temp = temp->children[0];
        }
    }

    return temp;
}

void freeNode(Node *node)
{
    free(node->children[0]);
    free(node->children[1]);
    free(node->children[2]);
    free(node);
}

double ttt_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void test_two_three_tree(int num_operations) {
    TwoThreeTree *tree = malloc(sizeof(TwoThreeTree));
    tree->root = NULL;
    
    srand(time(NULL));
    double start, end;
    
    start = ttt_get_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ttt_insert(tree, key);
    }
    end = ttt_get_time();
    printf("2-3 Tree Insert: %.6f sec\n", end - start);
    
    start = ttt_get_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ttt_search(tree, key);
    }
    end = ttt_get_time();
    printf("2-3 Tree Search: %.6f sec\n", end - start);
    
    start = ttt_get_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ttt_delete(tree, key);
    }
    end = ttt_get_time();
    printf("2-3 Tree Delete: %.6f sec\n", end - start);
    
    freeTree(tree->root);
    free(tree);
}

int main()
{
    int test_sizes[] = {1000};
    int num_tests = sizeof(test_sizes)/sizeof(test_sizes[0]);
    
    for(int i = 0; i < num_tests; i++) {
        int n = test_sizes[i];
        printf("\n=== Testing with %d operations ===\n", n);
        
        printf("\n-- 2-3 Tree --\n");
        test_two_three_tree(n);
    }

    return 0;
}