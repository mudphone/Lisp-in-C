all: compile-prompt

compile-prompt: prompt
cc -std=c99 -Wall prompt.c -o prompt

compile-readline: prompt_with_readline
cc -std=c99 -Wall prompt_with_readline.c -o prompt_with_readline -lreadline
