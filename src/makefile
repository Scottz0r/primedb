CC=gcc
CFLAGS=-I. -O2

OBJDIR=obj

all: clean code

code: main.o prmdb_assert.o prmdb_dbfile.o prmdb_log.o prmdb_sigs.o prmdb_verify.o
	$(CC) $(CFLAGS) -o prmdb main.o prmdb_assert.o prmdb_dbfile.o prmdb_log.o prmdb_sigs.o prmdb_verify.o
    
clean: 
	$(RM) *.o