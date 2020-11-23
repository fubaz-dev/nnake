# nnake Makefile
# (2020) fubaz <fubaz@protonmail.com>
#
# make all        builds the package
# make nnake      builds the package
# make run        builds and runs the package
# make install    install this package onto your system
# make uninstall  uninstall this package from your system
# make clean      cleans all result of building
# make dist       creates tar.gz
# make distclean  deletes everything from directory except original files
#

PACKAGE = nnake
VERSION = 0.0.1

CC       = gcc
CFLAGS   = -g -Wall
LIBS     = -lncurses -lncursesw
SRC      = main.c
ALLFILES = .git Makefile main.c README.md
TARGET   = nnake
EXEDIR   = /usr/bin/
TARNAME  = $(TARGET)-$(VERSION)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LIBS) -o $(TARGET) $(SRC)

$(TARNAME).tar.gz:
	touch $(TARNAME).tar.gz
	tar --exclude-vcs --exclude=$(TARNAME).tar.gz --xform='s|^|$(TARNAME)/|S' -czf $(TARNAME).tar.gz .

install:
	sudo mv $(TARGET) $(EXEDIR)

uninstall:
	sudo ${RM} "$(EXEDIR)$(TARGET)"

distclean:
	${RM} -r $(shell ls -Q -a -I main.c -I README.md -I Makefile -I .git -I .. -I .)

dist: distclean $(TARNAME).tar.gz

run: $(TARGET)
	./$(TARGET)

clean:
	${RM} $(TARGET)
