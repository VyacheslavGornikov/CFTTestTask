#include "common.h"
#include "serializer.h"
#include "dataprinter.h"

int AddData (StatData** binData, int* size);
int DeleteData (StatData** binData, int* size, int index);

void PrintMenu (void)
{
    printf("Меню базы данных:\n");
    printf("1) Добавить данные\n");
    printf("2) Удалить данные\n");
    printf("3) Вывести данные\n");
    printf("4) Сериализовать данные\n");
    printf("5) Десериализовать данные\n");
    printf("6) Выход\n");
}

int main (void)
{
    StatData* binData = NULL;
    int size = 0;
    int dataNumber = 0;
    int isRunning = 1;
    char filePath[MAX_PATH_LEN] = "";
    int fileStrLen = 0;

    while (isRunning)
    {
        system("clear");
        PrintMenu();
        printf("Введите пункт меню: ");
        int choice;
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
            case 1:
                if (size < MAX_ARRAY_SIZE)
                {
                    if (AddData(&binData, &size) == EXIT_SUCCESS)
                    {
                        printf("Данные были успешно добавлены.");
                    }
                    else
                    {
                        printf("Данные не могут быть добавлены!");
                    }
                }
                else
                {
                    printf("Хранилище данных переполнено!!!");
                }
                getchar();
                break;
            case 2:
                if (size > 0)
                {
                    printf("Введите номер данных для удаления (1 - %d): ", size);
                    scanf ("%d", &dataNumber);
                    while (dataNumber < 1 || dataNumber > size)
                    {
                        printf ("Индекс должен принимать значения от 1 до %d!"
                                " Повторите ввод: ", size);
                        scanf ("%d", &dataNumber);
                    }

                    if (DeleteData (&binData, &size, dataNumber - 1) == EXIT_SUCCESS)
                    {
                        printf("Данные были успешно удалены.");
                    }
                    else
                    {
                        printf ("Данные не будут удалены!");
                    }
                    getchar();
                }
                else
                {
                    printf("Хранилище данных пусто! Удаление невозможно!");
                }
                break;
            case 3:
                if (size > 0)
                {
                    PrintData (binData, size);
                }
                else
                {
                    printf ("Хранилище данных пусто!");
                }
                break;
            case 4:
                printf ("Введите путь до файла, куда хотите сохранить данные: ");
                fgets (filePath, MAX_PATH_LEN, stdin);
                fileStrLen = strlen (filePath);
                filePath[fileStrLen - 1] = '\0';
                if (StoreDump (binData, size, filePath) == EXIT_SUCCESS)
                    printf ("Данные сохранены в файл по пути: %s\n", filePath);
                else
                    printf ("Сериализация не удалась!\n");
                break;
            case 5:
                printf ("Введите путь до файла, откуда хотите взять данные: ");
                fgets (filePath, MAX_PATH_LEN, stdin);
                fileStrLen = strlen (filePath);
                filePath[fileStrLen - 1] = '\0';
                if (LoadDump (&binData, &size, filePath) == EXIT_SUCCESS)
                    printf ("Данные успешно загружены из файла: %s", filePath);
                else
                    printf ("Десериализация не удалась!\n");
                break;
            case 6:
                isRunning = 0;
                printf("Завершение работы программы...");
                if (binData != NULL)
                {
                    free (binData);
                }
                break;

            default:
                printf("Неверный пункт меню! Повторите ввод!\n");
                break;
        }

        printf ("\nНажмите любую клавишу для продолжения (или завершения)"
                " работы программы...\n");
        getchar();
    }

    return EXIT_SUCCESS;
}

int AddData (StatData** binData, int* size)
{
    unsigned int bitPrimary = 0;
    unsigned int bitMode    = 0;

    *binData = (StatData*) realloc (*binData, (*size + 1) * sizeof(StatData));
    if (*binData == NULL)
    {
        printf("Ошибка выделения памяти!\n");
        return EXIT_FAILURE;
    }

    printf("Введите id: ");
    scanf ("%ld", &((*binData)[*size].id));
    while ((*binData)[*size].id < 0)
    {
        printf ("id не может быть меньше 0! Повторите ввод: ");
        scanf ("%ld", &((*binData)[*size].id));
    }

    printf("Введите count: ");
    scanf ("%d", &((*binData)[*size].count));
    while ((*binData)[*size].count < 0)
    {
        printf ("count не может быть меньше 0! Повторите ввод: ");
        scanf ("%d", &((*binData)[*size].count));
    }

    printf("Введите cost: ");
    scanf ("%f", &((*binData)[*size].cost));
    while ((*binData)[*size].cost < 0)
    {
        printf ("cost не может быть меньше 0! Повторите ввод: ");
        scanf ("%f", &((*binData)[*size].cost));
    }


    printf("Введите primary (0 или 1): ");
    scanf ("%u", &bitPrimary);
    while (bitPrimary < 0 || bitPrimary > 1)
    {
        printf ("primary должно рринимать значения от 0 до 1! Повторите ввод: ");
        scanf ("%u", &bitPrimary);
    }
    (*binData)[*size].primary = bitPrimary;

    printf("Введите mode (от 0 до 7): ");
    scanf ("%u", &bitMode);
    while (bitMode < 0 || bitMode > 7)
    {
        printf ("mode должно рринимать значения от 0 до 7! Повторите ввод: ");
        scanf ("%u", &bitMode);
    }
    (*binData)[*size].mode = bitMode;

    (*size)++;

    return EXIT_SUCCESS;
}

int DeleteData (StatData** binData, int* size, int index)
{
    char ch;
    printf("%-19s %-19s %-19s %-19s %-19s %-19s\n",
        "№", "id", "count", "cost", "primary", "mode");
    PrintDataByIndex (*binData, *size, index);
    printf("Вы действительно хотите удалить данные (y/n)? ");
    getchar();
    ch = getchar();
    while (ch != 'y' && ch != 'n')
    {
        printf("Неверный символ! Введите y или n: ");
        getchar();
        ch = getchar();
    }

    if (ch == 'y')
    {
        for (int i = index; i < *size - 1; i++)
        {
            memcpy(&(*binData)[i], &(*binData)[i + 1], sizeof(StatData));
        }

        *binData = (StatData*)realloc (*binData, (*size - 1) * sizeof(StatData));
        (*size)--;

        if (*binData == NULL && *size != 0)
        {
            printf("Ошибка повторного выделения памяти!\n");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
    else
    {
        printf("Пользователь отменил операцию удаления!\n");
        return EXIT_FAILURE;
    }
}
