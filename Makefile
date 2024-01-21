
TARGET	=	calc
CARGS	=	-Wall -Wextra -Wpedantic
DEBUG	=	-g
LIBS	=	-lm -lreadline
OBJS	=	calc.o scanner.o main.o

all: $(TARGET)

%.o: %.c
	gcc $(DEBUG) $(CARGS) -c -o $@ $<

$(TARGET): $(OBJS)
	gcc $(DEBUG) -o $(TARGET) $(OBJS) $(LIBS)

calc.o: calc.c calc.h scanner.h
scanner.o: scanner.c scanner.h
main.o: main.c calc.h

clean:
	$(RM) $(TARGET) $(OBJS)
