CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic
NAME=sheet
$(NAME): $(NAME).o

clean:
	rm -f sheet sheet.o

load_input:
	./$(NAME) < testing_files/input.txt > testing_files/output.txt

load_test:
	./$(NAME) < testing_files/test.txt > testing_files/output.txt
