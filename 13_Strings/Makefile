TARGET = lispy

all: compile

clean:
	test -f $(TARGET) && rm $(TARGET)

compile: lispy.c
	cc -std=c99 -Wall $(TARGET).c mpc.c -lreadline -o $(TARGET)
