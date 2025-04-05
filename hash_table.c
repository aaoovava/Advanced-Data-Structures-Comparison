#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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

void ht_insert(HashTable *hashTable, int key, Data data)
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
Data *ht_search(HashTable *hashTable, int key)
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

int ht_delete(HashTable *hashTable, int key)
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

double ht_get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void test_hash_table(int num_operations) {
    HashTable *ht = createHashTable();
    
    srand(time(NULL));
    double start, end;
    
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        Data d = {"Test", "User", 25};
        ht_insert(ht, key, d);
    }
    end = ht_get_current_time();
    printf("Hash Table Insert: %.6f sec\n", end - start);
    
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ht_search(ht, key);
    }
    end = ht_get_current_time();
    printf("Hash Table Search: %.6f sec\n", end - start);
    
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ht_delete(ht, key);
    }
    end = ht_get_current_time();
    printf("Hash Table Delete: %.6f sec\n", end - start);
    
    deleteHashTable(ht);
}

int main()
{

    int test_sizes[] = {1000, 10000, 50000};
    int num_tests = sizeof(test_sizes)/sizeof(test_sizes[0]);
    
    for(int i = 0; i < num_tests; i++) {
        int n = test_sizes[i];
        printf("\n=== Testing with %d operations ===\n", n);
        
        printf("\n-- Hash Table --\n");
        test_hash_table(n);
    }

    return 0;
}
