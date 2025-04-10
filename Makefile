# Определяем переменные
TASK_PATH = .
SRC_DIR = $(TASK_PATH)/src
INC_DIR = $(TASK_PATH)/inc
BIN_DIR = $(TASK_PATH)/bin

# Находим все .c файлы в src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Определяем объектные файлы на основе исходников
OBJS = $(SRCS:.c=.o)

# Имя исполняемого файла
TARGET = $(BIN_DIR)/my_program

# Текстовые файлы для сериализации
FILES = $(wildcard $(BIN_DIR)/*.txt)

# Компилятор и флаги
CC = gcc
CFLAGS = -I$(INC_DIR)

# Правило по умолчанию
all: $(TARGET)

# Правило для создания исполняемого файла
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# Правило для компиляции .c файлов в .o файлы
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Чистка объектов и исполняемого файла
clean:
	rm -rf $(OBJS) $(TARGET)

fclean:
	rm -rf $(FILES)

.PHONY: all clean fclean
