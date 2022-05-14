IDIR =../include


CC=gcc
CFLAGS=-I$(IDIR)

# rdisc server (-r option) support [no|yes]
#ENABLE_RDISC_SERVER=yes

ODIR=obj
LDIR =../lib

# LIBS=-lm

_DEPS = rdisc.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = rdisc.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -DRDISC_SERVER -c -o $@ $< $(CFLAGS)

rdisc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 
      


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
