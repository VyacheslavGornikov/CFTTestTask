# Определяем переменные
TASK_PATH = .
SRC_DIR = $(TASK_PATH)/src
INC_DIR = $(TASK_PATH)/inc
BIN_DIR = $(TASK_PATH)/bin
TEST_FILES_DIR = $(BIN_DIR)/test_files

# Находим все .c файлы в src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Определяем объектные файлы на основе исходников
OBJS = $(BIN_DIR)/*.o

# Объектные файлы для binarydb
BINARYDB_OBJS = \
    $(BIN_DIR)/binarydb.o \
	$(BIN_DIR)/serializer.o \
	$(BIN_DIR)/dataprinter.o

# Объектные файлы для dataprocessor
DATAPROC_OBJS = \
    $(BIN_DIR)/dataprocessor.o \
	$(BIN_DIR)/serializer.o \
	$(BIN_DIR)/dataprinter.o

# Объектные файлы для testdata
TESTDATA_OBJS = \
    $(BIN_DIR)/testdata.o \
	$(BIN_DIR)/serializer.o

# Цели сборки по умолчанию
TARGETS = binarydb dataprocessor testdata

# Текстовые файлы для сериализации
FILES = $(wildcard $(BIN_DIR)/*.bin)

# Компилятор и флаги
CC = gcc
WARN_FLAGS = -Wall -Werror
CFLAGS = -I$(INC_DIR) $(WARN_FLAGS)

# Правило для цели сборки по умолчанию
all: $(TARGETS)

# Правило для создания папки bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR) \
	mkdir -p $(TEST_FILES_DIR)

# Правила для промежуточных целей сборки по умолчанию
binarydb: $(BIN_DIR)/binarydb

dataprocessor: $(BIN_DIR)/dataprocessor

testdata: $(BIN_DIR)/testdata

# Правило для создания бинарника binarydb
$(BIN_DIR)/binarydb: $(BINARYDB_OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

# Правило для создания бинарника dataprocessor
$(BIN_DIR)/dataprocessor: $(DATAPROC_OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

# Правило для создания бинарника testdata
$(BIN_DIR)/testdata: $(TESTDATA_OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

# Правило для создания объектных файлов
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Чистка объектов и исполняемого файла
clean:
	rm -rf $(OBJS) $(TEST_FILES_DIR)/*.bin $(addprefix $(BIN_DIR)/, $(TARGETS))

fclean:
	rm -rf $(FILES)

.PHONY: all clean fclean
