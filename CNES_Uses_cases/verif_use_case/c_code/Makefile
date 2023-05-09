# Compiler used for the cpu compilation
CC = gcc
CFLAGS =
LIBS = -lm


# Directory structure
INCLUDEDIR = include
LIBDIR = lib
OBJDIR = obj
SRCDIR = src
BINDIR = bin


OBJS = $(addprefix $(OBJDIR)/, demosaicing.o)


all: $(BINDIR)/demosaicing


$(BINDIR)/demosaicing: $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -L$(LIBDIR) -o $@ $^ $(LIBS)


$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDEDIR) -c -o $@ $<
	

$(OBJDIR):
	mkdir -p $@


$(BINDIR):
	mkdir -p $@


clean:
	rm -rf $(OBJDIR)


fclean: clean
	rm -rf $(BINDIR)/*


re: fclean all
