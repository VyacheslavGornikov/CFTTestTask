#include "common.h"
#include "dataprinter.h"

void PrintDataByIndex (StatData* binData, int size, int index)
{
    if (index < 0 || index > size - 1)
    {
        printf ("Данных с таким индексом не существует!\n");
    }
    else
    {
        printf("%-17d 0x%-17lX %-19d %-19.3e %-19c ", index + 1, binData[index].id,
                binData[index].count, binData[index].cost,
                binData[index].primary ? 'y' : 'n');
        PrintModeBinary (binData[index].mode);
        putchar ('\n');
    }
}

void PrintModeBinary (unsigned int mode)
{
    for (int i = MAX_MODE_BITS - 1; i >= 0; i--)
    {
        putchar (mode & (1 << i) ? '1' : '0');
    }
}

void PrintData (StatData* binData, int size)
{
    printf("\n%-19s %-19s %-19s %-19s %-19s %-19s\n",
            "№", "id", "count", "cost", "primary", "mode");
    for (int i = 0; i < size; i++)
    {
        PrintDataByIndex (binData, size, i);
    }
    putchar ('\n');
}
