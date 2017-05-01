TARGET = parsing

all: compile

clean:
	rm $(TARGET) 

compile: parsing.c
	cc -std=c99 -Wall $(TARGET).c mpc.c -lreadline -o $(TARGET)
