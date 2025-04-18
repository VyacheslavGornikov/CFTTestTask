#include "common.h"
#include "serializer.h"
#include "dataprinter.h"

#define HASH_TABLE_SIZE 200003

typedef struct HashEntry
{
    StatData data;
    struct HashEntry* next;
} HashEntry;

int JoinDump (StatData* binData1, int size1,
              StatData* binData2, int size2,
              StatData** resultData, int* resultSize);
long Hash (long id);
int ProcessElement (StatData* element, HashEntry** hashTable);
int SortDump (StatData* data, int size);
int CompareCost (const void* p1, const void* p2);

int main (int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Используйте: %s <Путь к файлу 1> <Путь к файлу 2> "
                "<Путь к итоговому файлу>\n", argv[0]);
        return EXIT_FAILURE;
    }

    StatData* binData1 = NULL;
    StatData* binData2 = NULL;
    StatData* resultData = NULL;
    int       size1 = 0;
    int       size2 = 0;
    int       resultSize = 0;
    char      filePath[MAX_PATH_LEN] = "";
    int       fileStrLen = 0;

    fileStrLen = strlen (argv[1]) + 1;
    strncpy (filePath, argv[1], fileStrLen);
    if (LoadDump (&binData1, &size1, filePath) == EXIT_SUCCESS)
    {
        printf ("Данные успешно загружены из файла #1: %s\n", filePath);
    }
    else
    {
        fprintf (stderr, "Десериализация из файла #1 не удалась!\n");
        return EXIT_FAILURE;
    }

    fileStrLen = strlen (argv[2]) + 1;
    strncpy (filePath, argv[2], fileStrLen);
    if (LoadDump (&binData2, &size2, filePath) == EXIT_SUCCESS)
    {
        printf ("Данные успешно загружены из файла #2: %s\n", filePath);
    }
    else
    {
        fprintf (stderr, "Десериализация из файла #2 не удалась!\n");
        return EXIT_FAILURE;
    }


    if (JoinDump (binData1, size1, binData2, size2, &resultData, &resultSize) !=
        EXIT_SUCCESS)
    {
        fprintf (stderr, "Ошибка в работе функции объединения двух массивов!\n");
        return EXIT_FAILURE;
    }

    if (SortDump (resultData, resultSize) != EXIT_SUCCESS)
    {
        fprintf (stderr, "Не удалось отсортировать массив!\n");
        return EXIT_FAILURE;
    }

    /*
     * Если записей меньше 10, то печатает size записей,
     * иначе печатает первые 10
     */
    if (resultSize < 10)
        PrintData (resultData, resultSize);
    else
        PrintData (resultData, 10);

    fileStrLen = strlen (argv[3]) + 1;
    strncpy (filePath, argv[3], fileStrLen);
    if (StoreDump (resultData, resultSize, filePath) == EXIT_SUCCESS)
    {
        printf ("Данные успешно сохранены в итоговый файл: %s\n", filePath);
    }
    else
    {
        fprintf (stderr, "Не удалось сериализовать данные в итоговый файл: "
                 "%s!\n", filePath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int JoinDump (StatData* binData1, int size1,
              StatData* binData2, int size2,
              StatData** resultData, int* resultSize)
{
    HashEntry** hashTable = NULL;
    StatData*   result = NULL;
    int         uniqueIdCount = 0;
    int         index = 0;

    hashTable = calloc (HASH_TABLE_SIZE, sizeof (HashEntry*));
    if (hashTable == NULL)
    {
        fprintf (stderr, "Не удалось выделить память под хеш-таблицу!\n");
        return EXIT_FAILURE;
    }

    /* Maybe cover to ProcessDataArray */
    for (int i = 0; i < size1; i++)
    {
        if (ProcessElement (&binData1[i], hashTable) != EXIT_SUCCESS)
        {
            fprintf (stderr, "Не удалось обработать массив из файла #1"
                     " на элементе %d!\n", i);
            free (hashTable);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < size2; i++)
    {
        if (ProcessElement (&binData2[i], hashTable) != EXIT_SUCCESS)
        {
            fprintf (stderr, "Не удалось обработать массив из файла #2"
                     " на элементе %d!\n", i);
            free (hashTable);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        HashEntry* entry = hashTable[i];
        while (entry != NULL)
        {
            uniqueIdCount++;
            entry = entry->next;
        }
    }

    result = (StatData*)malloc (uniqueIdCount * sizeof(StatData));
    if (result == NULL)
    {
        fprintf (stderr, "Не удалось выделить память для итогового массива!\n");
        free (hashTable);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        HashEntry* entry = hashTable[i];
        while (entry != NULL)
        {
            result[index++] = entry->data;
            HashEntry* temp = entry;
            entry = entry->next;
            free (temp);
        }
    }

    free (hashTable);
    *resultData = result;
    *resultSize = uniqueIdCount;

    return EXIT_SUCCESS;
}

long Hash (long id)
{
    return id % HASH_TABLE_SIZE;
}

int ProcessElement (StatData* element, HashEntry** hashTable)
{
    long hash = -1;

    hash = Hash (element->id);
    if (hash < 0)
    {
        fprintf (stderr, "Хеш-функция вернула отрицательный хеш %ld\n", hash);
        return EXIT_FAILURE;
    }

    HashEntry* curEntry = hashTable[hash];
    HashEntry* prevEntry = NULL;

    while (curEntry != NULL)
    {
        if (curEntry->data.id == element->id)
        {
            curEntry->data.count += element->count;
            curEntry->data.cost += element->cost;
            curEntry->data.primary &= element->primary;
            if (element->mode > curEntry->data.mode)
                curEntry->data.mode = element->mode;

            return EXIT_SUCCESS;
        }

        prevEntry = curEntry;
        curEntry = curEntry->next;
    }

    HashEntry* newEntry = (HashEntry*)malloc (sizeof(HashEntry));
    if (newEntry == NULL)
    {
        fprintf (stderr, "Не удалось выделить память для новой записи в"
                 " хеш-таблице!\n");
        return EXIT_FAILURE;
    }

    newEntry->data = *element;
    newEntry->next = NULL;

    if (prevEntry == NULL)
        hashTable[hash] = newEntry;
    else
        prevEntry->next = newEntry;

    return EXIT_SUCCESS;
}

int SortDump (StatData* data, int size)
{
    if (data == NULL || size == 0)
    {
        printf ("Невозможно отсортировать пустой массив!\n");
        return EXIT_FAILURE;
    }

    qsort (data, size, sizeof(StatData), CompareCost);

    return EXIT_SUCCESS;
}

int CompareCost (const void* p1, const void* p2)
{
    const StatData* data1 = (const StatData*) p1;
    const StatData* data2 = (const StatData*) p2;

    if (data1->cost < data2->cost)
        return -1;
    if (data1->cost > data2->cost)
        return 1;
    return 0;
}