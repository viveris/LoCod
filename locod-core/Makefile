ifeq ($(TARGET), $(filter $(TARGET), ultra96 enclustra))
# Directory structure
INCLUDEDIR = include
LIBDIR = lib
OBJDIR = obj
SRCDIR = src
BINDIR = bin

# Varibles overloading
CC +=
CFLAGS += -DTARGET_${TARGET}
CPPFLAGS +=
LDFLAGS +=
LDLIBS += -lm

OBJS = $(addprefix $(OBJDIR)/, main.o locod.o)

all: locod-cpu

locod-cpu: $(BINDIR)/locod-cpu

$(BINDIR)/locod-cpu: $(OBJS) | $(BINDIR)
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
endif


ifeq ($(TARGET), ngultra)
APP_NAME := locod-cpu
NGULTRA_COMMON := y
NGULTRA_INIT := y

TARGET_MEM := TARGET_MEM_TCM_A

include ../../build/common/Makefile.common

SRCFILES += src/main.c src/locod.c
CFLAGS += -DTARGET_${TARGET}

include ../../build/common/Makefile.build
endif
