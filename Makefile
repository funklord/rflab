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
HEADER = helper.h manchester.h 
#SRC = bytecoder.c  helper.c manchester.c  pin.c  test.c
SRC = helper.c manchester.c test.c
OBJ = $(SRC:.c=.o)
LIB = -lm
#LIBFILES = flog/libflog.a

##Rules
.PHONY : all clean distclean valgrind_test

all: test

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(HEADER) $(OBJ) $(LIBFILES)
	$(CC) $(LDFLAGS) $(OBJ) $(LIB) -o $@

doxygen: Doxyfile $(SRC) $(HEADER)
	$(DOXYGEN)

valgrind_test: test
	$(VALGRIND) ./$<

clean:
	$(RM) $(OBJ) test

distclean: clean
	$(RM) -r doxygen
