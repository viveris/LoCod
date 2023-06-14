# Compiler flags used for the cpu compilation
CFLAGS =
LIBS = -lm

# Directory structure
OBJDIR = obj
SRCDIR = src
BINDIR = bin

OBJS = $(addprefix $(OBJDIR)/, main.o map_memory.o)

all: locod-cpu

locod-cpu: $(BINDIR)/locod-cpu

$(BINDIR)/locod-cpu: $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<


$(OBJDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@


clean:
	rm -rf $(OBJDIR)


fclean: clean
	rm -rf $(BINDIR)/*


re: fclean all
