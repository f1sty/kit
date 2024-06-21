CFLAGS = -Wall -Wextra -pedantic
OBJDIR = build
SRCDIR = src
OBJS = $(OBJDIR)/main.o
EXECNAME = kit
TARGET = $(OBJDIR)/$(EXECNAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $<

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

.PHONE: all clean
