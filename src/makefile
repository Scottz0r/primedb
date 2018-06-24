CC=gcc
CFLAGS=-I.

OBJDIR=obj

all: main.o prmdb_assert.o prmdb_dbfile.o prmdb_log.o prmdb_sigs.o prmdb_verify.o
	$(CC) $(CFLAGS) -o prmdb main.o prmdb_assert.o prmdb_dbfile.o prmdb_log.o prmdb_sigs.o prmdb_verify.o

