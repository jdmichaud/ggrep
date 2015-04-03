##
## Makefile for router/tests in
##
## Made by jean-daniel michaud
## Login   <jean.daniel.michaud@gmail.com>
##

NAME    = ggrep
SRCDIR  = src/
TESTDIR = tests/
DATE    = `date '+%b%d%y%Hh%M'`
#
all		: $(NAME) $(TEST)
#
$(NAME)	:
		cd $(SRCDIR) && make && cd ..
		cp $(SRCDIR)/$(NAME)
#
$(TEST) :
		cd $(TESTDIR) && make && cd ..
#
clean	:
		cd $(SRCDIR) && make clean && cd ..
		cd $(TESTDIR) && make clean && cd ..
#
re		: clean all
