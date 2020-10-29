CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror
NAME=sheet
DELIM_ARG=-d "|:]['"
$(NAME): $(NAME).o

clean:
	rm -f sheet sheet.o

load_input:
	./$(NAME) $(DELIM_ARG) < testing_files/input.txt > testing_files/output.txt

load_test: 
	./$(NAME) $(DELIM_ARG) < testing_files/test.txt > testing_files/output.txt
