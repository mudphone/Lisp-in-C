TARGET = lispy

all: compile

clean:
	test -f $(TARGET) && rm $(TARGET)

compile: parsing.c
	cc -std=c99 -Wall $(TARGET).c mpc.c -lreadline -o $(TARGET)
