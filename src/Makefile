##
## Makefile for ggrep/tests in
##
## Made by jean-daniel michaud
## Login   <jean.daniel.michaud@gmail.com>
##
##

CC  = colorgcc -ggdb3
AR  = ar
#CC  = clang++ -g
RM  = rm -fr
#
#
NAME = ggrep
SRCS = $(wildcard *.cc)
OBJS   = $(SRCS:.cc=.o)
#
CFLAGS = -Wall -Wextra -Wshadow -std=c++11 -ggdb3
IPATH  = -I../ -I../src -I../../mylib/src -I../../loglite
LPATH  = -L../lib
LIB    = -lcppunit -lpthread -lefence -lstdc++ -lmylib -lncurses -pthread
#
.PHONY = all clean fclean
#
all: $(OBJS)
	$(CC) $(IPATH) $(LPATH) $(CFLAGS) $(OBJS) -o $(NAME) $(LIB)
%.o : %.cc
	$(CC) -c $(IPATH) $(CFLAGS) $< -o $@
#
clean: fclean
	$(RM) *o *c~ *cc~ *hpp~ *~ Makefile~ \#* .depend
#
fclean:
	$(RM) $(NAME)
#
re  : clean all
#
depend: .depend
.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) $(IPATH) -MM $(SRCS) -MF  ./.depend;
include .depend
