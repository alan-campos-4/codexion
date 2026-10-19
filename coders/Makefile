NAME	= codexion

SRC_DIR	= src  
INC_DIR	= include

CC		= cc
CFLAGS	= -Wall -Werror -Wextra -I$(INC_DIR)
RM		= rm -rf

GREEN	= "\033[0;32m"
RED		= "\033[0;31m"
END		= "\033[0m"

SOURCES	:= $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(SOURCES:.c=.o)

%.o: %.c
	@$(CC) $(CFLAGS) -I. -c $< -o $@

all: $(NAME)
$(NAME): $(OBJECTS)
	@$(CC) -g $(CFLAGS) $(SOURCES) -o $(NAME)
	@echo $(GREEN)"Program created."$(END)

clean:
	@$(RM) $(OBJECTS)
	@echo $(RED)"Object files removed."$(END)

fclean:
	@$(RM) $(OBJECTS) && $(RM) $(NAME)
	@echo $(RED)"Library and object files removed."$(END)

re: fclean all clean

test1:
	./codexion 5 600 200 100 100 10 2 fifo


.PHONY: all clean fclean test1 test_m
