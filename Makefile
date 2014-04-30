##Config
CFLAGS   = -W -Wall -Os
LDFLAGS  = -W -Wall -Os
ifdef DEBUG
CFLAGS  += -g -DDEBUG
LDFLAGS += -g -DDEBUG
endif
ifdef PROFILE
CFLAGS  += -pg
LDFLAGS += -pg
endif
DOXYGEN  = doxygen
VALGRIND = valgrind -v --leak-check=full

##Files
#HEADER = bytecoder.h helper.h manchester.h  pin.h
HEADER = helper.h manchester.h manchester_lookup.h config.h
#SRC = bytecoder.c  helper.c manchester.c  pin.c  test.c
SRC = helper.c manchester.c manchester_lookup.c test.c
OBJ = $(SRC:.c=.o)
LIB = -lm
#LIBFILES = flog/libflog.a

##Rules
.PHONY : all clean distclean valgrind_test

all: test

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

manchester_lookup.h: manchester_lookup.c

manchester_lookup.c: manchester_lookup_create.c
	mv config.h config_backup.h
	cp manchester_lookup_create.config config.h
	cc -W -Wall -Os manchester_lookup_create.c manchester.c -o manchester_lookup_create
	./manchester_lookup_create
	rm manchester_lookup_create
	mv config_backup.h config.h

test: $(HEADER) $(OBJ) $(LIBFILES)
	$(CC) $(LDFLAGS) $(OBJ) $(LIB) -o $@

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) test manchester_lookup.c manchester_lookup.h

distclean: clean
	$(RM) -r doxygen
