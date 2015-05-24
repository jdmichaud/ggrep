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
CFLAGS = -Wall -Wextra -Wshadow -std=c++11 -ggdb3
IPATH  = -I.. -I../src -I../../mylib/src -I../../ -I/usr/src/gmock/gtest/include
LPATH  = -L../src -L../lib -L/usr/src/gmock
LIB    = -lcppunit -lpthread -lgmock -lefence -lstdc++ -lmylib
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
