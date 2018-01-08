CFLAGS := -Wall -Wextra -O2
LIB := -lSDL -lGL
CC := c++
TARGET := openborne
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

%.o: %.cpp
	$(CC) $(LIB) $(CFLAGS) -I/usr/include/SDL -Isrc/ -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(LIB) -o $@ $^ $(CFLAGS)

install:
	cp $(TARGET) /usr/bin

clean:
	rm -f $(TARGET) $(OBJ)
