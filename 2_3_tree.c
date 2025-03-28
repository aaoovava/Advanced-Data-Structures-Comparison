#include <stdio.h>
#include <stdlib.h>

#define MAX_KEYS 2
#define MAX_CHILDREN 3

typedef struct Node
{
    int keys[3];
    struct Node *children[4];
    int key_count;
    int child_count;
    int is_leaf;
} Node;

typedef struct
{
    Node *root;
} TwoThreeTree;

void split(TwoThreeTree *tree, Node *node);

Node *search(TwoThreeTree *tree, int key);

Node *searchPredecessor(Node *node, TwoThreeTree *tree);

void fillSuccessor(Node *successor, TwoThreeTree *tree);

void insertKey(Node *node, int key, TwoThreeTree *tree)
{
    if (node->key_count == 2)
    {
        if (key < node->keys[0])
        {
            node->keys[2] = node->keys[1];
            node->keys[1] = node->keys[0];
            node->keys[0] = key;
        }

        if (key > node->keys[0] && key < node->keys[1])
        {
            node->keys[2] = node->keys[1];
            node->keys[1] = key;
        }

        if (key > node->keys[1])
        {
            node->keys[2] = key;
        }
    }

    else if (node->key_count == 1)
    {
        if (key > node->keys[0])
        {
            node->keys[1] = key;
        }
        else
        {
            node->keys[1] = node->keys[0];
            node->keys[0] = key;
        }
    }
    else
    {
        node->keys[0] = key;
    }

    node->key_count++;

    if (node->key_count == 3)
    {
        split(tree, node);
    }
}

void split(TwoThreeTree *tree, Node *node)
{
    Node *temp = tree->root;
    int key = node->keys[1];
    int indexForFoundChild = 0;

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
    if (key == temp->keys[1])
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
    while (1)
    {
        if (key < temp->keys[0])
        {
            if (temp->children[0]->keys[1] == key)
            {
                leftMiddleRight = 0;
                indexForFoundChild = 0;
                break;
            }
            temp = temp->children[0];
            continue;
        }

        if (key > temp->keys[0] && temp->keys[1] == 0)
        {
            if (temp->children[2]->keys[1] == key)
            {
                leftMiddleRight = 2;
                indexForFoundChild = 2;
                break;
            }
            temp = temp->children[2];
            continue;
        }

        if (key > temp->keys[0] && key < temp->keys[1] && temp->keys[1] != 0)
        {
            if (temp->children[1]->keys[1] == key)
            {
                leftMiddleRight = 1;
                indexForFoundChild = 1;
                break;
            }
            temp = temp->children[1];
            continue;
        }

        temp->child_count += 2;
    }

    if (temp->children[indexForFoundChild]->child_count <= 3)
    {
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
                temp->children[1] = temp->children[2];
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

void insert(TwoThreeTree *tree, int key)
{
    Node *root = tree->root;
    Node *temp = root;

    if (root == NULL)
    {
        root = malloc(sizeof(Node));
        root->keys[0] = key;
        root->is_leaf = 1;
        root->key_count++;
        tree->root = root;
        return;
    }

    if (root->children[0] == NULL)
    {
        insertKey(root, key, tree);
        return;
    }

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
    }
}

void deleteKey(Node *node, int key)
{
    if (node->keys[0] == key)
    {
        node->keys[0] = node->keys[1];
        node->key_count--;
    }
    else if (node->keys[1] == key)
    {
        node->keys[1] = node->keys[2];
        node->key_count--;
    }
}

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

int isContains(Node *node, int key)
{
    if (node->keys[0] == key || node->keys[1] == key || node->keys[2] == key)
    {
        return 1;
    }
    return 0;
}

void delete(TwoThreeTree *tree, int key)
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

    temp = search(tree, key);

    if (temp->is_leaf && temp->key_count == 2) // if key is in leaf and leaf has 2 keys
    {
        deleteKey(temp, key);
    }
    else
    {
        replaseBySuccessor(temp, key, tree);
    }

    return;
}

void join(Node *successor, Node *joinNode) {

    
    if (successor->keys[0] > joinNode->keys[0]) {
        successor->keys[1] = successor->keys[0];
        successor->keys[0] = joinNode->keys[0];
    } else {
        successor->keys[1] = joinNode->keys[0];
    }
    successor->key_count++;
    
    if (successor->child_count == 2) {
        successor->children[1] = successor->children[0];
        successor->children[0] = joinNode->children[0];
    } else {
        successor->children[2] = successor->children[0];
        successor->children[0] = joinNode->children[0];
        successor->children[1] = joinNode->children[2];
    }

    successor->child_count = 0;

    for (int i = 0; i < 4; i++)
    {
        if (successor->children[i] != NULL)
        {
            successor->child_count++;
        }
    }
    
    

    free(joinNode);
}

void share(Node *successor, Node *predecessor, Node *shareNode)
{
    successor->keys[0] = predecessor->keys[0];
    successor->key_count = 1;

    predecessor->keys[0] = shareNode->keys[1];
    shareNode->keys[1] = 0;
    shareNode->key_count = 1;
}

void fillSuccessor(Node *successor, TwoThreeTree *tree)
{
    if (successor == tree->root)
    {
        tree->root = tree->root->children[0];
        return;
    }

    Node *predecessor = searchPredecessor(successor, tree);

    if (predecessor->key_count == 2) // if predecessor has 2 keys working with case 2
    {
        if (predecessor->children[2] == successor)
        {
            successor->keys[0] = predecessor->keys[1];
            successor->key_count = 1;
            predecessor->keys[1] = 0;
            predecessor->key_count--;
            Node *joinNode = predecessor->children[1]; // using like link for node wich will join or share with successor

            if (predecessor->children[1]->key_count == 2)
                share(successor, predecessor, joinNode);

            else
            {
                predecessor->children[1] = NULL;
                predecessor->child_count--;
                join(successor, joinNode);
            }
        }

        else if (predecessor->children[1] == successor)
        {
            successor->keys[0] = predecessor->keys[0];
            successor->key_count = 1;
            predecessor->keys[0] = 0;
            predecessor->key_count--;
            Node *joinNode = predecessor->children[0];

            if (predecessor->children[1]->key_count == 2)
                share(successor, predecessor, joinNode);

            else
            {
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

    if (predecessor->key_count == 1) // if predecessor has 1 key working with case 1
    {
        successor->keys[0] = predecessor->keys[0];
        successor->key_count = 1;
        predecessor->keys[0] = 0;
        predecessor->key_count--;
        Node *joinNode;
        if (predecessor->children[0] == successor)
        {
            joinNode = predecessor->children[2];
        } else
        {
            joinNode = predecessor->children[0]; 
        }


        if (joinNode->key_count == 2)
        {
            share(successor, predecessor, joinNode);
        }

        else
        {
            predecessor->children[0] = successor;
            predecessor->child_count--;
            join(successor, joinNode);
        }

        if (predecessor->key_count == 0)
        {
            fillSuccessor(predecessor, tree);
        }
    }
}

Node *search(TwoThreeTree *tree, int key)
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
    }
    return NULL;
}

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
int main()
{
    TwoThreeTree *tree = malloc(sizeof(TwoThreeTree));

    insert(tree, 10);
    insert(tree, 20);
    insert(tree, 30);
    insert(tree, 15);
    insert(tree, 8);
    insert(tree, 5);
    insert(tree, 1);

    printTree(tree);

    delete (tree, 10);

    printf("\n");

    printTree(tree);

    return 0;
}
