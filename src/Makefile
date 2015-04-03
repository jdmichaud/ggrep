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
#SRCS = model.cc terminal_view.cc main.cc
SRCS = $(wildcard *.cc)
OBJS   = $(SRCS:.cs=.o)
#
CFLAGS = -Wall -Wextra -Wshadow -std=c++98
IPATH  = -I../src -I../../mylib/src
LPATH  = -L../lib
LIB    = -lcppunit -lpthread -lefence -lstdc++ -lmylib
#
.PHONY = all clean fclean
#
all: $(OBJS)
	$(CC) $(IPATH) $(LPATH) $(CFLAGS) $(OBJS) -o $(NAME) $(LIB)
.cc .o:
	$(CC) $(IPATH) $(CFLAGS) -c $<
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
