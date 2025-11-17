##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## Makefile
##

NAME = bs_rtype

OK = \033[0;32m
KO = \033[0;31m
RESET = \033[0m

# Directories
SRC_DIR = src
LIB_DIR = lib
MATH_DIR = $(SRC_DIR)/common/math
UTILS_DIR = $(SRC_DIR)/common/utils
#TESTS_DIR = tests

# Include directories
INC_DIRS = -I$(SRC_DIR) -I$(MATH_DIR) -I$(UTILS_DIR)

# Compiler options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I. $(INC_DIRS) -g
LDFLAGS = -ldl -fno-gnu-unique -lconfig++

# Common files needed for all libraries
#COMMON_SRC = $(SRC_DIR)/

#COMMON_OBJ = $(COMMON_SRC:.cpp=.o)

# Core sources and objects
CORE_SRC = $(SRC_DIR)/main.cpp

CORE_OBJ = $(CORE_SRC:.cpp=.o) $(COMMON_OBJ)

# Tests
#TESTS_SRC = $(wildcard $(TESTS_DIR)/*.cpp)
#TESTS_OBJ = $(TESTS_SRC:.cpp=.o)
#TESTS_BIN = unit_tests

# Main rules
all: core materials primitives

# Core rule - builds the main executable
core: $(NAME)

$(NAME): $(CORE_OBJ)
	@echo -e "$(OK)[RULE] Building $(NAME) executable$(RESET)"
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(CORE_OBJ) $(LDFLAGS) \
	&& echo -e "$(OK)[RULE] $(NAME) executable created$(RESET)" \
	|| echo -e "$(KO)[RULE] Failed to create $(NAME) executable$(RESET)"


# Tests rules
#tests_run: $(COMMON_OBJ) $(TESTS_OBJ)
#	@echo -e "$(OK)[RULE] Building tests$(RESET)"
#	@$(CXX) $(CXXFLAGS) -o $(TESTS_BIN) $(COMMON_OBJ) $(TESTS_OBJ) --coverage -lcriterion $(LDFLAGS) \
#	&& echo -e "$(OK)[RULE] Tests built successfully$(RESET)" \
#	|| echo -e "$(KO)[RULE] Failed to build tests$(RESET)"
#	@echo -e "$(OK)[RULE] Running tests$(RESET)"
#	@./$(TESTS_BIN) \
#	&& echo -e "$(OK)[RULE] Tests passed successfully$(RESET)" \
#	|| echo -e "$(KO)[RULE] Tests failed$(RESET)"

# Compile object files
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@ \
	&& echo -e "$(OK)[OK] $<$(RESET)" \
	|| echo -e "$(KO)[KO] $<$(RESET)"

clean:
	@echo -e "$(OK)[RULE] Cleaning object files$(RESET)"
	@find $(SRC_DIR) -name "*.o" -type f -delete
#	@find $(TESTS_DIR) -name "*.o" -type f -delete

fclean: clean
	@echo -e "$(OK)[RULE] Full cleaning (executable and libraries)$(RESET)"
	@$(RM) $(NAME)
#	@$(RM) $(TESTS_BIN)
	@echo -e "$(OK)[RULE] Removing library files$(RESET)"
#	@$(RM) -f $(LIB_DIR)/*.so

re: fclean all

# Display help
help:
	@echo -e "$(OK)Available targets:$(RESET)"
	@echo -e "  all        : Build everything (default)"
	@echo -e "  core       : Build only the main raytracer executable"
	@echo -e "  primitives : Build only the primitive libraries"
	@echo -e "  materials  : Build only the material libraries"
	@echo -e "  tests_run  : Build and run unit tests with criterion"
	@echo -e "  clean      : Remove object files"
	@echo -e "  fclean     : Remove object files and executable (not lib dir)"
	@echo -e "  re         : Full rebuild"
	@echo -e "  help       : Display this help message"

.PHONY: all core primitives materials tests_run clean fclean re help