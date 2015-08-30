##
## Makefile for ggrep/tests in
##
## Made by jean-daniel michaud
## Login   <jean.daniel.michaud@gmail.com>
##
##

#CC  = colorgcc -rdynamic
CC  = clang++
#-Wl,--export-dynamic
AR  = ar
RM  = rm -fr
#
HOST = $(shell hostname)
NAME = ggrep
SRCS = $(wildcard *.cc) $(wildcard curses/*.cc)
OBJS = $(SRCS:.cc=.o)
DPDS = $(SRCS:.cc=.d)
#
UNAME_S := $(shell uname -s)
CFLAGS = -Wall -Wextra -Wshadow -std=c++11 -stdlib=libc++ -Wfatal-errors -Wno-unused-parameter -DS_LOGERR
OPTFLGS= -O3 -Os
IPATH  = -I../ -I../src -I../../loglite
LPATH  =
LIB    = -lpthread -lstdc++ -lncursesw -stdlib=libc++
DBGLIBS=
#
.PHONY = all dbg clean fclean
#
all: $(OBJS)
	$(CC) $(IPATH) $(LPATH) $(OBJS) -o $(NAME) $(LIB) $(DBGLIBS)
%.o : %.cc
	$(CC) -c $(IPATH) $(CFLAGS) $(OPTFLGS) $< -o $@
#
dbg: CFLAGS += -DDEBUG -ggdb3 -DS_LOGINF -DS_LOGDBG
dbg: DBGLIBS= -lcppunit -lefence -lSegFault
dbg: OPTFLGS=
dbg: all
#
clean: fclean
	$(RM) *o curses/*o *c~ *cc~ *hpp~ *~ Makefile~ \#* *.d
#
fclean:
	$(RM) $(NAME)
#
re  : clean all
#
red : clean dbg
#
depend: $(DPDS)
%.d : %.cc
	rm -fr $@
	$(CC) $(CFLAGS) $(IPATH) -MM $< -MF $@
#
include $(DPDS)
