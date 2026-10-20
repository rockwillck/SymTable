all: testsymtablelist testsymtablehash

testsymtable.o: testsymtable.c symtable.h
	gcc217 -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
	gcc217 -c symtablelist.c

symtablehash.o: symtablehash.c symtable.h
	gcc217 -c symtablehash.c

testsymtablelist: symtablelist.o testsymtable.o
	gcc217 symtablelist.o testsymtable.o -o testsymtablelist

testsymtablehash: symbtablehash.o testsymtable.o
	gcc217 symbtablehash.o testsymtable.o -o testsymtablehash

