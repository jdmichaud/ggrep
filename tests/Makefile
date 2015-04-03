##
## Makefile for ggrep/tests in
##
## Made by jean-daniel michaud
## Login   <jean.daniel.michaud@gmail.com>
##

CC  = colorgcc -ggdb3
#CC  = clang++ -g
RM  = rm -fr
#
#
NAME = ggrepunittest
SRC  = main.cpp
#
OBJS = $(SRC:.cpp=.o)
#
CFLAGS = -Wall -Wextra -Wshadow -std=c++98
IPATH  = -I../../src/base -I../../src/plugin
#LPATH  = -L. -L../src
LIB    = -lcppunit -lpthread -lefence
#
all: $(OBJS)
	$(CC) $(IPATH) $(LPATH) $(CFLAGS) $(OBJS) -o $(NAME) $(LIB)
.cpp.o:
	$(CC) $(IPATH) $(CFLAGS) -c $<
#
clean: fclean
	$(RM) *o *c~ *cpp~ *hpp~ *~ Makefile~ \#* .depend
#
fclean:
	$(RM) $(NAME)
#
re  : clean all
#
depend: .depend
.depend: $(SRC)
	rm -f ./.depend
	$(CC) $(CFLAGS) $(IPATH) -MM $(SRC) -MF  ./.depend;
include .depend
