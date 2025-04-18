#include "common.h"
#include "serializer.h"

int StoreDump (StatData* binData, int size, const char* filePath)
{
    int writtenData = 0;

    if (binData == NULL)
    {
        printf ("Хранилище данных пусто!\n");
        return EXIT_FAILURE;
    }

    FILE* file = fopen (filePath, "wb");
    if (file == NULL)
    {
        printf ("Не удалось создать файл по пути: %s\n", filePath);
        return EXIT_FAILURE;
    }

    writtenData = GetValidDataCount (binData, size);

    printf ("Количество записанных данных = %d\n", writtenData);
    if (fwrite (&writtenData, sizeof(int), 1, file) != 1)
    {
        printf ("Не уадалось записать количество данных!\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < size; i++)
    {
        if (!IsValidData (&binData[i]))
            continue;
        if (fwrite (&binData[i], sizeof(StatData), 1, file) != 1)
        {
            printf ("Ошибка записи в файл: %s\n", filePath);
            fclose (file);
            return EXIT_FAILURE;
        }
    }

    fclose (file);
    return EXIT_SUCCESS;
}

int LoadDump (StatData** binData, int* size, const char* filePath)
{
    int       dataCount = 0;
    StatData* tempData  = NULL;

    FILE* file = fopen (filePath, "rb");
    if (file == NULL)
    {
        printf ("Не удалось открыть файл по пути: %s\n", filePath);
        return EXIT_FAILURE;
    }

    if (fread (&dataCount, sizeof(int), 1, file) != 1)
    {
        printf ("Не удалось прочитать количество записей в файле %s!\n",
                filePath);
        fclose (file);
        return EXIT_FAILURE;
    }
    printf ("Количество загруженных записей = %d\n", dataCount);

    tempData = (StatData*)malloc (dataCount * sizeof(StatData));
    if (tempData == NULL)
    {
        printf ("Ошибка выделения памяти!\n");
        fclose (file);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < dataCount; i++)
    {
        if (fread (&tempData[i], sizeof(StatData), 1, file) != 1)
        {
            printf ("Ошибка чтения записи %d из файла: %s\n", i + 1,filePath);
            free (tempData);
            fclose (file);
            return EXIT_FAILURE;
        }
    }

    if (*binData != NULL)
        free (*binData);

    *binData = tempData;
    *size = dataCount;

    return EXIT_SUCCESS;
}

int GetValidDataCount (StatData* binData, int size)
{
    int validDataCount = 0;

    for (int i = 0; i < size; i++)
    {
        if (IsValidData(&binData[i]))
            validDataCount++;
    }

    return validDataCount;
}

int IsValidData(StatData *binData)
{
    if (binData->id < 0 || binData->count < 0 ||
        binData->cost < 0 || binData->primary < 0 ||
        binData->primary > 1 || binData->mode < 0 ||
        binData->mode > 7)
        return 0;

    return 1;
}
