
TARGET	=	c

all: $(TARGET)

$(TARGET): calc.c
	gcc -Wall -Wextra -Wpedantic -g -o $(TARGET) calc.c -lm

clean:
	$(RM) $(TARGET)
