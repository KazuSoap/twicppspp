# ------------------------------------
# Makefile
# ------------------------------------

# target
TARGET = $(shell basename `pwd`)

# directory
OUT_DIR = build
SRC_DIR = src
HEADER_DIR = include
PROGRAM_DIR = $(OUT_DIR)/bin
LIB_OUT_DIR = $(OUT_DIR)/lib
OBJ_DIR = $(OUT_DIR)/obj

# input files
SRCS = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(addprefix $(OBJ_DIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJ_DIR)/,$(SRCS:.cpp=.d))

# compiler
CXX = g++ -std=c++14
CXXFLAGS = -I./$(HEADER_DIR)
LDFLAGS  = `pkg-config --libs libssl` `pkg-config --libs oauth` -liconv

# dummy target
.PHONY : release debug all clean

# release
release: CXX += -O2
# release: STATICFLAG = -static
release: CXXFLAGS = -I./$(HEADER_DIR) `pkg-config --cflags opencv_static`
release: LDFLAGS  = `pkg-config --libs opencv_static` `pkg-config --libs libssl` `pkg-config --libs oauth` `pkg-config --libs jsoncpp` -liconv
release: all

# debug
debug: CXX += -g -O0
debug: all

# all
all: $(TARGET)
$(TARGET): $(OBJS)
	@mkdir -p $(LIB_OUT_DIR)
#	@mkdir -p $(PROGRAM_DIR)
	ar r $(LIB_OUT_DIR)/lib$@.a $^
#	$(CXX) $(STATICFLAG) $^ $(LDFLAGS) -o $(PROGRAM_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -MMD -MP -MF $(@:%.o=%.d) $< -o $@

# コマンドラインから与えた最終ターゲットが "clean" 以外
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif

# clean
clean:
	$(RM) -r $(OUT_DIR)
