CC = g++
COMPILE_OPTIONS = -std=c++14 -Wall -I/usr/include/SDL2 -I/usr/include/lua5.3 #-DLUA_32BITS
LINK_OPTIONS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf -llua5.3
TARGET = main
OBJS_DIR = lib
SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS:.cpp=.o)))
DEPS = $(addprefix $(OBJS_DIR)/, $(notdir $(SRCS:.cpp=.d)))

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LINK_OPTIONS)

#.cpp.o:
$(OBJS_DIR)/%.o: %.cpp
	@[ -d $(OBJS_DIR) ]
	$(CC) -o $@ -c -MMD -MP $< $(COMPILE_OPTIONS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(TARGET)
