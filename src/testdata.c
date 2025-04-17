#include "common.h"
#include <sys/time.h>
#include <math.h>

#define TEST_FILES_DIR "test_files"
#define PASS_STR "[УСПЕХ]"
#define FAIL_STR "[НЕУДАЧА]"
#define MAX_CMD_SIZE 1024
#define ERR_STR_LEN 256

typedef struct TestCase
{
    const char* name;
    StatData* inputA;
    int sizeA;
    StatData* inputB;
    int sizeB;
    StatData* expectedOut;
    int expectedSize;
} TestCase;



int StoreDump (StatData* binData, int size, const char* filePath);
int LoadDump (StatData** binData, int* size, const char* filePath);
void RunTest (TestCase* test, int* passed, double* totalTime);
int CompareStatData (const StatData* output, const StatData* expected, int size,
                     char* errStr);
int CompareCostFloat (float cost1, float cost2);


/* Case 1 */
/* Содержимое для исходных файлов */
const StatData case_1_in_a[2] =
{{.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode=3 },
{.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode=0 }};
const StatData case_1_in_b[2] =
{{.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode=2 },
{.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode=2}};
/* Ожидаемый результат обработки */
const StatData case_1_out[3] =
{{.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2 },
{.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3 },
{.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2 }};


TestCase testCases[] = {
    {
        .name = "case_1",
        .inputA = (StatData*)case_1_in_a,
        .sizeA = 2,
        .inputB = (StatData*)case_1_in_b,
        .sizeB = 2,
        .expectedOut = (StatData*)case_1_out,
        .expectedSize = 3
    }
};

const int TEST_COUNT = sizeof(testCases) / sizeof(TestCase);

int main (void)
{
    int passed = 0;
    double totalTime = 0.0;

    printf ("Выполнение %d тестов... \n", TEST_COUNT);
    for (int i = 0; i < TEST_COUNT; i++)
    {
        RunTest (&testCases[i], &passed, &totalTime);
    }

    printf("\nРезультаты: %d/%d тестов завершились успешно "
           "(Время выполнения: %.3f сек)\n", passed, TEST_COUNT, totalTime);

    return passed == TEST_COUNT ? EXIT_SUCCESS : EXIT_FAILURE;
}

void RunTest (TestCase* test, int* passed, double* totalTime)
{
    struct timeval start, end;
    StatData*      outData = NULL;
    int            outDataSize = 0;
    char           in1Path[MAX_PATH_LEN] = "";
    char           in2Path[MAX_PATH_LEN] = "";
    char           outPath[MAX_PATH_LEN] = "";
    char           cmd[MAX_CMD_SIZE] = "";
    int            retVal = 0;
    int            isSuccessful = 1;
    long           seconds = 0;
    long           micros = 0;
    double         testTime = 0;
    char           compareErr[ERR_STR_LEN];



    snprintf (in1Path, MAX_PATH_LEN, "%s/%s_in_a.bin",
              TEST_FILES_DIR, test->name);
    snprintf (in2Path, MAX_PATH_LEN, "%s/%s_in_b.bin",
              TEST_FILES_DIR, test->name);
    snprintf (outPath, MAX_PATH_LEN, "%s/%s_out.bin",
              TEST_FILES_DIR, test->name);

    if (StoreDump (test->inputA, test->sizeA, in1Path) != EXIT_SUCCESS)
    {
        fprintf (stderr, "%s -> %s: Не удалось открыть файл A!\n",
                 FAIL_STR, test->name);
        return;
    }

    if (StoreDump (test->inputB, test->sizeB, in2Path) != EXIT_SUCCESS)
    {
        fprintf (stderr, "%s -> %s: Не удалось открыть файл B!\n",
                 FAIL_STR, test->name);
        return;
    }

    /* Запуск утилиты обработки данных */
    gettimeofday (&start, NULL);
    snprintf(cmd, MAX_CMD_SIZE, "./dataprocessor %s %s %s",
             in1Path, in2Path, outPath);
    retVal = system (cmd);
    gettimeofday (&end, NULL);

    if (retVal != 0)
    {
        fprintf (stderr, "%s -> %s: dataprocessor завершилась с кодом %d!\n",
                 FAIL_STR, test->name, retVal);
        return;
    }

    /* Проверка результата */

    if (LoadDump(&outData, &outDataSize, outPath) != EXIT_SUCCESS)
    {
        fprintf (stderr, "%s -> %s: Не удалось загрузить выходные данные!\n",
                 FAIL_STR, test->name);
        return;
    }

    if (outDataSize != test->expectedSize)
    {
        fprintf (stderr, "%s -> %s: Размеры массивов не совпали "
                 "(полученный %d, ожидаемый %d)!\n",
                 FAIL_STR, test->name, outDataSize, test->expectedSize);
        isSuccessful = 0;
    }
    else if (CompareStatData (outData, test->expectedOut, outDataSize,
                              compareErr) != EXIT_SUCCESS)
    {
        fprintf (stderr, "%s -> %s: %s!\n", FAIL_STR, test->name, compareErr);
        isSuccessful = 0;
    }

    free(outData);

    /* Расчет времени */
    seconds = end.tv_sec - start.tv_sec;
    micros = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;
    testTime = seconds + micros / 1e6;
    *totalTime += testTime;

    if (isSuccessful)
    {
        printf("%s -> %s (%.3f сек)\n", PASS_STR, test->name, testTime);
        (*passed)++;
    }
}

int StoreDump (StatData* binData, int size, const char* filePath)
{
    int writtenData = size;

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

    FilterInvalidData(binData, size);

    printf ("Количество записанных данных = %d\n", writtenData);
    if (fwrite (&writtenData, sizeof(int), 1, file) != 1)
    {
        printf ("Не уадалось записать количество данных!\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < size; i++)
    {
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

int CompareStatData (const StatData* output, const StatData* expected, int size,
                     char* errStr)
{
    for (int i = 0; i < size; i++)
    {
        if (output[i].id != expected[i].id)
        {
            snprintf (errStr, ERR_STR_LEN, "По индексу [%d] у записей не равны "
                      "id (полученное %ld, ожидаемое %ld)",
                      i, output[i].id, expected[i].id);
            return EXIT_FAILURE;
        }

        if (output[i].count != expected[i].count)
        {
            snprintf (errStr, ERR_STR_LEN, "По индексу [%d] у записей не равны "
                      "count (полученное %d, ожидаемое %d)",
                      i, output[i].count, expected[i].count);
            return EXIT_FAILURE;
        }

        if (!CompareCostFloat (output[i].cost, expected[i].cost))
        {
            snprintf (errStr, ERR_STR_LEN, "По индексу [%d] у записей не равны "
                      "cost (полученное %.3e, ожидаемое %.3e)",
                      i, output[i].cost, expected[i].cost);
            return EXIT_FAILURE;
        }

        if (output[i].primary != expected[i].primary)
        {
            snprintf (errStr, ERR_STR_LEN, "По индексу [%d] у записей не равны "
                      "primary (полученное %d, ожидаемое %d)",
                      i, output[i].primary, expected[i].primary);
            return EXIT_FAILURE;
        }

        if (output[i].mode != expected[i].mode)
        {
            snprintf (errStr, ERR_STR_LEN, "По индексу [%d] у записей не равны "
                      "mode (полученное %d, ожидаемое %d)",
                      i, output[i].primary, expected[i].primary);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int CompareCostFloat (float cost1, float cost2)
{
    return (fabs(cost1 - cost2) < 1e-5); /* Допустимая погрешность */
}