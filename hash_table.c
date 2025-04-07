#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define DEFAULT_CAPACITY 128 
#define LOAD_FACTOR_UP 0.75
#define LOAD_FACTOR_DOWN 0.15

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
    int size;     // current table size
    int capacity; // owerall table capacity
    int deletedCount;
} HashTable;

int hash1(int key, int size) // first hash function (simple division)
{
    return key % size;
}

int hash2(int key, int size) // second hash function (linear probing)
{
    return (key % (size - 1)) | 1;
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

void resize(HashTable *hashTable, int newCapacity) {
    HashCell *oldTable = hashTable->table;
    int oldCapacity = hashTable->capacity;

    // new hash table
    hashTable->capacity = newCapacity;
    hashTable->table = (HashCell*)calloc(newCapacity, sizeof(HashCell));
    hashTable->size = 0;
    hashTable->deletedCount = 0;

    // set all cells to EMPTY
    for (int i = 0; i < newCapacity; i++) {
        hashTable->table[i].status = EMPTY;
    }

    // rehash old table
    for (int i = 0; i < oldCapacity; i++) {
        if (oldTable[i].status == OCCUPIED) {
            int key = oldTable[i].key;
            Data data = oldTable[i].data;
            int index = hash1(key, newCapacity);
            int step = hash2(key, newCapacity);

            // serch for free cell
            while (hashTable->table[index].status == OCCUPIED) {
                index = (index + step) % newCapacity;
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
Data* ht_search(HashTable* hashTable, int key) {
    int index = hash1(key, hashTable->capacity);
    int step = hash2(key, hashTable->capacity);
    int initialIndex = index;
    int firstIteration = 1;

    while (hashTable->table[index].status != EMPTY) {
        if (hashTable->table[index].status == OCCUPIED && 
            hashTable->table[index].key == key) {
            return &hashTable->table[index].data;
        }
        index = (index + step) % hashTable->capacity;

        // Защита от бесконечного цикла при полной таблице
        if (index == initialIndex && !firstIteration) break;
        firstIteration = 0;
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
    

    // Тест вставки
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        Data d = {"Test", "User", 25};
        ht_insert(ht, key, d);
    }
    end = ht_get_current_time();
    printf("Insert: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    // Тест пошуку
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ht_search(ht, key);
    }
    end = ht_get_current_time();
    printf("Search: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    // Тест видалення
    start = ht_get_current_time();
    for(int i = 0; i < num_operations; i++) {
        int key = rand() % (num_operations * 10);
        ht_delete(ht, key);
    }
    end = ht_get_current_time();
    printf("Delete: %.6f sec\tAVG: %.9f sec/op\n", 
          end - start, (end - start)/num_operations);

    // Розрахунок пам'яті
    size_t memory_used = sizeof(HashTable) + (ht->capacity * sizeof(HashCell));
    size_t memory_used_kb = memory_used / 1024;
    printf("Memory used: %zu KB\n", memory_used_kb);

    deleteHashTable(ht);
}
// int main()
// {
//     printf("\n=== Testing with 1000 operations ===\n");
//     test_hash_table(1000);

//     return 0;
// }
