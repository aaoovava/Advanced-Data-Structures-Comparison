#include <stdio.h>
#include <stdlib.h>

#define LOAD_FACTOR_UP 0.8
#define LOAD_FACTOR_DOWN 0.2
#define DEFAULT_CAPACITY 10

typedef struct Data
{
    char name[50];
    char surname[50];
    int age;
} Data;

typedef enum CellStatus
{
    EMPTY,
    OCCUPIED,
    DELETED
} CellStatus;

typedef struct HashCell
{
    int key;
    Data data;
    CellStatus status;
} HashCell;

typedef struct HashTable
{
    HashCell *table;
    int size;     // Current table size
    int capacity; // Owerall table capacity
    int deletedCount;
} HashTable;

int hash1(int key, int size) // first hash function (simple division)
{
    return key % size;
}

int hash2(int key, int size) // second hash function (linear probing)
{
    return (key % (size - 1)) + 1;
}

HashTable *createHashTable()
{
    HashTable *hashTable = malloc(sizeof(HashTable));
    hashTable->size = 0;
    hashTable->capacity = DEFAULT_CAPACITY;
    hashTable->deletedCount = 0;
    hashTable->table = (HashCell *)calloc(DEFAULT_CAPACITY, sizeof(HashCell));
    for (int i = 0; i < DEFAULT_CAPACITY; i++)
    {
        hashTable->table[i].status = EMPTY;
    }

    return hashTable;
}

void deleteHashTable(HashTable *hashTable)
{
    free(hashTable->table);
    free(hashTable);
}

void resize(HashTable *hashTable, int newCapacity)
{
    HashCell *oldTable = hashTable->table;
    int oldCapacity = hashTable->capacity;

    // new table initialization
    hashTable->capacity = newCapacity;
    hashTable->table = (HashCell *)calloc(newCapacity, sizeof(HashCell));
    hashTable->size = 0;
    hashTable->deletedCount = 0;

    for (int i = 0; i < oldCapacity; i++)
    {
        hashTable->table[i].status = EMPTY;
    }

    for (size_t i = 0; i < oldCapacity; i++)
    {
        // bringing back all the data from the old table
        if (oldTable[i].status == OCCUPIED)
        {
            int key = oldTable[i].key;
            Data data = oldTable[i].data;
            int index = hash1(key, newCapacity);
            int step = hash2(key, newCapacity);

            while (hashTable->table[index].status == OCCUPIED)
            {
                index = (index + step) % hashTable->capacity;
            }

            hashTable->table[index].key = key;
            hashTable->table[index].data = data;
            hashTable->table[index].status = OCCUPIED;
            hashTable->size++;
        }
    }
    free(oldTable);
}

void insert(HashTable *hashTable, int key, Data data)
{
    if ((double)(hashTable->size + hashTable->deletedCount) / hashTable->capacity > LOAD_FACTOR_UP)
    {
        resize(hashTable, hashTable->capacity * 2);
    }

    int index = hash1(key, hashTable->capacity);
    int step = hash2(key, hashTable->capacity);

    while (hashTable->table[index].status == OCCUPIED)
    {
        // update data for existing key
        if (hashTable->table[index].key == key)
        {
            hashTable->table[index].data = data;
            return;
        }

        index = (index + step) % hashTable->capacity;
    }

    hashTable->table[index].key = key;
    hashTable->table[index].data = data;
    hashTable->table[index].status = OCCUPIED;
    hashTable->size++;
}

// returns NULL if not found (simple search)
Data *search(HashTable *hashTable, int key)
{
    int index = hash1(key, hashTable->capacity);
    int step = hash2(key, hashTable->capacity);

    for (int i = 0; i < hashTable->capacity; i++)
    {
        if (hashTable->table[index].status == EMPTY)
        {
            return NULL;
        }
        if (hashTable->table[index].status == OCCUPIED && hashTable->table[index].key == key)
        {
            return &hashTable->table[index].data;
        }
        index = (index + step) % hashTable->capacity;
    }

    return NULL;
}

int delete(HashTable *hashTable, int key)
{
    int index = hash1(key, hashTable->capacity);
    int step = hash2(key, hashTable->capacity);

    for (int i = 0; i < hashTable->capacity; i++)
    {
        if (hashTable->table[index].status == EMPTY)
        {
            return 0;
        }
        if (hashTable->table[index].status == OCCUPIED && hashTable->table[index].key == key)
        {
            hashTable->table[index].status = DELETED;
            hashTable->size--;
            hashTable->deletedCount++;

            if ((double)(hashTable->size + hashTable->deletedCount) / hashTable->capacity < LOAD_FACTOR_DOWN && hashTable->capacity > DEFAULT_CAPACITY)
            {
                resize(hashTable, hashTable->capacity / 2);
            }
            return 1;
        }
        index = (index + step) % hashTable->capacity;
    }

    return 0;
}

void printTable(HashTable *hashTable)
{
    for (int i = 0; i < hashTable->capacity; i++)
    {
        printf("Index %d: ", i);
        if (hashTable->table[i].status == OCCUPIED)
        {
            printf("Key=%d, Name=%s\n", hashTable->table[i].key, hashTable->table[i].data.name);
        }
        else
        {
            printf("Empty/Deleted\n");
        }
    }
}

int main()
{

    HashTable *ht = createHashTable();

    Data p1 = {"John", "Doe", 30};
    Data p2 = {"Alice", "Smith", 25};

    insert(ht, 1, p1);
    insert(ht, 1002, p2);

    printTable(ht);

    Data *found = search(ht, 1);
    if (found)
    {
        printf("Found: %s %s, %d\n", found->name, found->surname, found->age);
    }

    delete (ht, 1001);

    printTable(ht);

    found = search(ht, 1001);
    if (!found)
    {
        printf("Element not found\n");
    }

    deleteHashTable(ht);

    return 0;
}
