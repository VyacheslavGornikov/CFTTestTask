# Определяем переменные
TASK_PATH = .
SRC_DIR = $(TASK_PATH)/src
INC_DIR = $(TASK_PATH)/inc
BIN_DIR = $(TASK_PATH)/bin
OBJ_DIR = $(BIN_DIR)/obj
TEST_FILES_DIR = $(BIN_DIR)/test_files

# Находим все .c файлы в src
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Объектные файлы для dataprocessor
DATAPROC_OBJS = \
    $(OBJ_DIR)/dataprocessor.o \
	$(OBJ_DIR)/serializer.o \
	$(OBJ_DIR)/dataprinter.o

# Объектные файлы для testdata
TESTDATA_OBJS = \
    $(OBJ_DIR)/testdata.o \
	$(OBJ_DIR)/serializer.o

# Цели сборки по умолчанию
TARGETS = dataprocessor testdata

# Компилятор и флаги
CC = gcc
WARN_FLAGS = -Wall -Werror
CFLAGS = -I$(INC_DIR) $(WARN_FLAGS)

# Правило для цели сборки по умолчанию
all: $(TARGETS)

# Правило для создания папки bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR) \
	$(TEST_FILES_DIR) \
	$(OBJ_DIR)

# Правила для промежуточных целей сборки по умолчанию

dataprocessor: $(BIN_DIR)/dataprocessor

testdata: $(BIN_DIR)/testdata

# Правило для создания бинарника dataprocessor
$(BIN_DIR)/dataprocessor: $(DATAPROC_OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

# Правило для создания бинарника testdata
$(BIN_DIR)/testdata: $(TESTDATA_OBJS) | $(BIN_DIR)
	$(CC) -o $@ $^

# Правило для создания объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Чистка объектов и исполняемого файла
clean:
	rm -rf $(OBJ_DIR)/*.o $(TEST_FILES_DIR)/*.bin $(addprefix $(BIN_DIR)/, $(TARGETS))

.PHONY: all clean fclean
