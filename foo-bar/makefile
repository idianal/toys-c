# =========
# Variables
# =========

PROGRAM_NAME = foo-bar
CFLAGS = -Wall -Wextra

BINARY_DIR = binary
OBJECT_DIR = object
SOURCE_DIR = source

TEST_DIR = test
TEST_BINARY_DIR = $(TEST_DIR)/$(BINARY_DIR)
TEST_OBJECT_DIR = $(TEST_DIR)/$(OBJECT_DIR)
TEST_SOURCE_DIR = $(TEST_DIR)/$(SOURCE_DIR)

OBJECTS = foo.o bar.o
SOURCE_OBJECTS = $(addprefix $(OBJECT_DIR)/, $(OBJECTS))
TEST_OBJECTS = $(addprefix $(TEST_OBJECT_DIR)/test-, $(OBJECTS)) \
	$(addprefix $(TEST_OBJECT_DIR)/test-runners/test-runner-, $(OBJECTS))

UNITY_DIR = unity
UNITY_FLAGS = -DUNITY_FIXTURE_NO_EXTRAS
TEST_FILES = $(UNITY_DIR)/src/unity.c \
	$(UNITY_DIR)/extras/fixture/src/unity_fixture.c
TEST_SOURCES = $(UNITY_DIR)/src \
	$(UNITY_DIR)/extras/fixture/src \
	$(SOURCE_DIR)
TEST_INCLUDES = $(addprefix -I, $(TEST_SOURCES))


# =====
# Rules
# =====

run: build
	./$(BINARY_DIR)/$(PROGRAM_NAME)

build: $(OBJECT_DIR)/main.o $(SOURCE_OBJECTS) | $(BINARY_DIR)
	cc $(CFLAGS) -o $(BINARY_DIR)/$(PROGRAM_NAME) $^

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c | $(OBJECT_DIR)
	cc -c $(CFLAGS) -o $@ $<

test: $(TEST_OBJECT_DIR)/test-runners/run-tests.o $(TEST_OBJECTS) $(SOURCE_OBJECTS) | $(TEST_BINARY_DIR)
	cc $(CFLAGS) $(UNITY_FLAGS) -o $(TEST_BINARY_DIR)/test-$(PROGRAM_NAME) $^ $(TEST_FILES) $(TEST_INCLUDES)
	./$(TEST_BINARY_DIR)/test-$(PROGRAM_NAME)

$(TEST_OBJECT_DIR)/test-runners/%.o: $(TEST_SOURCE_DIR)/test-runners/%.c | $(TEST_OBJECT_DIR)
	cc -c $(CFLAGS) $(UNITY_FLAGS) -o $@ $< $(TEST_INCLUDES)

$(TEST_OBJECT_DIR)/test-%.o: $(TEST_SOURCE_DIR)/test-%.c | $(TEST_OBJECT_DIR)
	cc -c $(CFLAGS) $(UNITY_FLAGS) -o $@ $< $(TEST_INCLUDES)

$(BINARY_DIR):
	mkdir $(BINARY_DIR)

$(OBJECT_DIR):
	mkdir $(OBJECT_DIR)

$(TEST_BINARY_DIR):
	mkdir $(TEST_BINARY_DIR)

$(TEST_OBJECT_DIR):
	mkdir $(TEST_OBJECT_DIR)
	mkdir $(TEST_OBJECT_DIR)/test-runners

clean:
	rm -rf $(BINARY_DIR) $(OBJECT_DIR) $(TEST_BINARY_DIR) $(TEST_OBJECT_DIR)
