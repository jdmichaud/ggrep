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
#CC  = clang++ -g
RM  = rm -fr
#
#
HOST = $(shell hostname)
NAME = ggrep
SRCS = $(wildcard *.cc) $(wildcard curses/*.cc)
OBJS = $(SRCS:.cc=.o)
DPDS = $(SRCS:.cc=.d)
#
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS = -ggdb3 -Wall -Wextra -Wshadow -std=c++11 -Wfatal-errors -Wno-unused-parameter -DS_LOGERR -DS_LOGINF -DS_LOGDBG
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS = -ggdb3 -Wall -Wextra -Wshadow -std=c++11 -stdlib=libc++ -Wfatal-errors -Wno-unused-parameter -DS_LOGERR -DS_LOGINF -DS_LOGDBG
endif
IPATH  = -I../ -I../src -I../../mylib/src -I../../loglite
LPATH  = -L../lib
LIB    = -lcppunit -lpthread -lstdc++ -lmylib_$(HOST) -lncursesw
DEBUGLIB = -lefence -lSegFault
#
.PHONY = all clean fclean
#
all: $(OBJS)
	$(CC) $(IPATH) $(LPATH) $(CFLAGS) $(OBJS) -o $(NAME) $(LIB)
%.o : %.cc
	$(CC) -c $(IPATH) $(CFLAGS) $< -o $@
#
clean: fclean
	$(RM) *o curses/*o *c~ *cc~ *hpp~ *~ Makefile~ \#* *.d
#
fclean:
	$(RM) $(NAME)
#
re  : clean all
#
depend: $(DPDS)
%.d : %.cc
	rm -fr $@
	$(CC) $(CFLAGS) $(IPATH) -MM $< -MF $@
include $(DPDS)
