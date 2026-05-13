PROGRAM_NAME = foo-bar
CFLAGS = -Wall -Wextra -std=c23 -MMD -MP

BIN_DIR = binary
OBJ_DIR = object
SRC_DIR = source

TEST_DIR = test
TEST_BIN_DIR = $(TEST_DIR)/$(BIN_DIR)
TEST_OBJ_DIR = $(TEST_DIR)/$(OBJ_DIR)
TEST_SRC_DIR = $(TEST_DIR)/$(SRC_DIR)

OBJS = foo.o bar.o
SRC_OBJS = $(addprefix $(OBJ_DIR)/, $(OBJS))
TEST_OBJS = $(addprefix $(TEST_OBJ_DIR)/test-, $(OBJS)) \
    $(addprefix $(TEST_OBJ_DIR)/test-runners/test-runner-, $(OBJS))

UNITY_DIR = unity
UNITY_FLAGS = -DUNITY_FIXTURE_NO_EXTRAS
UNITY_CFLAGS = -Wall -Wextra -std=c23 $(UNITY_FLAGS)
UNITY_OBJS = $(UNITY_DIR)/src/unity.o \
    $(UNITY_DIR)/extras/fixture/src/unity_fixture.o
TEST_SRCS = $(UNITY_DIR)/src \
    $(UNITY_DIR)/extras/fixture/src \
    $(SRC_DIR)
TEST_INCS = $(addprefix -I, $(TEST_SRCS))
TEST_DEPS = $(TEST_OBJ_DIR)/test-runners/run-tests.o \
    $(TEST_OBJS) \
    $(SRC_OBJS) \
    $(UNITY_OBJS)

-include $(wildcard $(OBJ_DIR)/*.d \
    $(TEST_OBJ_DIR)/*.d \
    $(TEST_OBJ_DIR)/test-runners/*.d)

.PHONY: run build test clean

run: build
	./$(BIN_DIR)/$(PROGRAM_NAME)

build: $(OBJ_DIR)/main.o $(SRC_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(PROGRAM_NAME) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

test: $(TEST_DEPS) | $(TEST_BIN_DIR)
	$(CC) $(CFLAGS) $(UNITY_FLAGS) -o $(TEST_BIN_DIR)/test-$(PROGRAM_NAME) $^
	./$(TEST_BIN_DIR)/test-$(PROGRAM_NAME)

$(TEST_OBJ_DIR)/test-runners/%.o: \
    $(TEST_SRC_DIR)/test-runners/%.c | \
    $(TEST_OBJ_DIR)
	$(CC) -c $(CFLAGS) $(UNITY_FLAGS) -o $@ $< $(TEST_INCS)

$(TEST_OBJ_DIR)/test-%.o: $(TEST_SRC_DIR)/test-%.c | $(TEST_OBJ_DIR)
	$(CC) -c $(CFLAGS) $(UNITY_FLAGS) -o $@ $< $(TEST_INCS)

$(UNITY_DIR)/%.o: $(UNITY_DIR)/%.c
	$(CC) -c $(UNITY_CFLAGS) -o $@ $< $(TEST_INCS)

$(BIN_DIR) $(OBJ_DIR) $(TEST_BIN_DIR):
	mkdir -p $@

$(TEST_OBJ_DIR):
	mkdir -p $@ $@/test-runners

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR)
	rm -f $(UNITY_OBJS)
