CC = g++
CFLAGS = -c -g -Wshadow -Winit-self -Wredundant-decls -Wcast-align\
-Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations\
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra\
-Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy\
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op\
-Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith\
-Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel\
-Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJDIR = obj
OBJ = $(OBJDIR)/main.o $(OBJDIR)/tree.o $(OBJDIR)/dump.o $(OBJDIR)/latex.o $(OBJDIR)/graph.o $(OBJDIR)/diff.o $(OBJDIR)/read.o

all: diff.exe

diff.exe: $(OBJ)
	@$(CC) $^ -o $@

$(OBJDIR)/%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean all

clean:
	del $(OBJ) diff.exe
