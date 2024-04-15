# Directory structure
INCLUDEDIR = include
LIBDIR = lib
OBJDIR = obj
SRCDIR = src
BINDIR = bin

# Varibles
CC := gcc
CFLAGS := 
CPPFLAGS :=
LDFLAGS :=
LDLIBS := -lm

OBJS = $(addprefix $(OBJDIR)/, demosaicing.o)

all: use_case_ccode

use_case_ccode: $(BINDIR)/use_case_ccode

$(BINDIR)/use_case_ccode: $(OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -L$(LIBDIR) $^ $(LOADLIBES) $(LDLIBS) -o $@


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