# **************************************************************************** #
#                                                                              #
#                                                          :::      ::::::::   #
#   Makefile                                             :+:      :+:    :+:   #
#                                                      +:+ +:+         +:+     #
#   By: mschuck <mschuck@student.42.fr>              +#+  +:+       +#+        #
#                                                  +#+#+#+#+#+   +#+           #
#   Created: 2013/11/28 00:07:04 by mschuck             #+#    #+#             #
#   Updated: 2015/03/19 11:08:43 by mschuck            ###   ########.fr       #
#                                                                              #
# **************************************************************************** #

NAME		= libfl.a


CC			= gcc
CFLAGS		= -ggdb -DUNIX -Wall -Wextra -I includes -fPIC 
#CFLAGS		= -Wall -Wextra -Werror -I includes -fPIC 
#-Wno-implicit-function-declaration

FILES		= 	fl_fuzzyfy_methods \
			fl_rule_token \
			fl_variable_token \
			fl_fuzzylogic \
      			fl_variable_obj\
			expr_methods \
			expr \
			list \
			utils \

SRC			= $(addsuffix .c, $(FILES))
OBJ			= $(addsuffix .o, $(FILES))

.PHONY: clean fclean re

.SILENT:

all: $(NAME)


$(NAME): $(OBJ)
	ar rc $(NAME) $(OBJ)
	ranlib $(NAME)
	printf "\xF0\x9F\x8D\xBA  $@ is ok\n  "
	printf ""

%.o: %.c

	printf "\xF0\x9F\x92\xBF  $@\n"
	printf ""
	gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJ)
	printf "\xE2\x8C\xA6  objects files \n"

fclean: clean
	rm -f $(NAME)
	printf "\xE2\x8C\xA6  $(NAME) file \n"

re: fclean all
