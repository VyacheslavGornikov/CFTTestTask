# Определяем переменные
TASK_PATH = .
SRC_DIR = $(TASK_PATH)/src
INC_DIR = $(TASK_PATH)/inc
BIN_DIR = $(TASK_PATH)/bin

# Находим все .c файлы в src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Определяем объектные файлы на основе исходников
OBJS = $(BIN_DIR)/*.o

# Объектные файлы для binarydb
BINARYDB_OBJS = \
    $(BIN_DIR)/binarydb.o

# Объектные файлы для dataprocessor
DATAPROC_OBJS = \
    $(BIN_DIR)/dataprocessor.o

# Цели сборки по умолчанию
TARGETS = binarydb dataprocessor

# Текстовые файлы для сериализации
FILES = $(wildcard $(BIN_DIR)/*.txt)

# Компилятор и флаги
CC = gcc
WARN_FLAGS = -Wall -Werror
CFLAGS = -I$(INC_DIR) $(WARN_FLAGS)

# Правило для цели сборки по умолчанию
all: $(TARGETS)

# Правила для промежуточных целей сборки по умолчанию
binarydb: $(BIN_DIR)/binarydb

dataprocessor: $(BIN_DIR)/dataprocessor

# Правило для создания бинарника binarydb
$(BIN_DIR)/binarydb: $(BINARYDB_OBJS)
	$(CC) -o $@ $^

# Правило для создания бинарника dataprocessor
$(BIN_DIR)/dataprocessor: $(DATAPROC_OBJS)
	$(CC) -o $@ $^

# Правило для создания объектных файлов
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Чистка объектов и исполняемого файла
clean:
	rm -rf $(OBJS)  $(addprefix $(BIN_DIR)/, $(TARGETS))

fclean:
	rm -rf $(FILES)

.PHONY: all clean fclean
