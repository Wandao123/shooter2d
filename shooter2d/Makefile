#CXX = g++
CPPFLAGS = -Wall -I/usr/include/SDL2 -I/usr/include/lua5.3 -I../packages/sol2 #-DLUA_32BITS
CXXFLAGS = -std=c++17
LDFLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf -llua5.3

TARGET = main
OBJS_DIR = lib
SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS:.cpp=.o)))
DEPS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS:.cpp=.d)))

DEBUG_FLAGS = -g3 -O0
RELEASE_FLAGS = -s -O2

.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all
.PHONY: release
release: CXXFLAGS += $(RELEASE_FLAGS)
release: all

.PHONY: all
all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJS_DIR)/%.o: %.cpp
	@[ -d $(OBJS_DIR) ]
	$(CXX) $(CXXFLAGS) -o $@ -c -MMD -MP $< $(CPPFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(TARGET)
