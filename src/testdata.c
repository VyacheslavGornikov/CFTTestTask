#include "common.h"
#include "serializer.h"
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

void RunTest (TestCase* test, int* passed, double* totalTime);
int CompareStatData (const StatData* output, const StatData* expected, int size,
                     char* errStr);
int CompareCostFloat (float cost1, float cost2);


/* Case 1 */
/* Содержимое для исходных файлов */
const StatData case_1_in_a[2] =
{
    {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode =3  },
    {.id = 90089, .count = 1, .cost = 88.90, .primary = 1, .mode = 0 }
};
const StatData case_1_in_b[2] =
{
    {.id = 90089, .count = 13, .cost = 0.011, .primary = 0, .mode = 2 },
    {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2}
};
/* Ожидаемый результат обработки */
const StatData case_1_out[3] =
{
    {.id = 90189, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2 },
    {.id = 90889, .count = 13, .cost = 3.567, .primary = 0, .mode = 3 },
    {.id = 90089, .count = 14, .cost = 88.911, .primary = 0, .mode = 2 }
};

/* Case 2 */
/* Содержимое для исходных файлов */
const StatData case_2_in_a[4] =
{
    {.id = -1, .count = 13, .cost = 3.567, .primary = 0, .mode=3 },
    {.id = 90089, .count = -1, .cost = 88.90, .primary = 1, .mode = 0 },
    {.id = 90089, .count = 1, .cost = -88.90, .primary = 1, .mode = 0 },
    {.id = 200000, .count = 1, .cost = 88.90, .primary = 1, .mode = 0 }
};
const StatData case_2_in_b[2] =
{
    {.id = 100000, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2},
    {.id = 200000, .count = 1000, .cost = 21.12003, .primary = 0, .mode = 3}
};
/* Ожидаемый результат обработки */
const StatData case_2_out[2] =
{
    {.id = 100000, .count = 1000, .cost = 1.00003, .primary = 1, .mode = 2},
    {.id = 200000, .count = 1001, .cost = 110.02003, .primary = 0, .mode = 3}
};


TestCase testCases[] =
{
    {
        .name = "case_1",
        .inputA = (StatData*)case_1_in_a,
        .sizeA = 2,
        .inputB = (StatData*)case_1_in_b,
        .sizeB = 2,
        .expectedOut = (StatData*)case_1_out,
        .expectedSize = 3
    },
    {
        .name = "case_2",
        .inputA = (StatData*)case_2_in_a,
        .sizeA = 4,
        .inputB = (StatData*)case_2_in_b,
        .sizeB = 2,
        .expectedOut = (StatData*)case_2_out,
        .expectedSize = 2
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
    char           compareErr[ERR_STR_LEN] = "";



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
        printf("\n%s -> %s (%.3f сек)\n", PASS_STR, test->name, testTime);
        (*passed)++;
    }
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