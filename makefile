CC = gcc
CFLAGS = -ansi -Wall -pedantic -Iheaders

assembler: core/assembler.o core/first_pass.o core/second_pass.o core/parser.o core/sym_table.o core/helpers.o core/preproc.o core/macro_list.o
	$(CC) $(CFLAGS) core/assembler.o core/first_pass.o core/second_pass.o core/parser.o core/sym_table.o core/helpers.o core/preproc.o core/macro_list.o -o assembler

core/assembler.o: core/assembler.c headers/globals.h headers/first_pass.h headers/second_pass.h headers/helpers.h headers/preproc.h headers/parser.h headers/sym_table.h headers/context_asm.h
	$(CC) $(CFLAGS) -c core/assembler.c -o core/assembler.o

core/first_pass.o: core/first_pass.c headers/globals.h headers/first_pass.h headers/helpers.h headers/parser.h headers/sym_table.h headers/context_asm.h
	$(CC) $(CFLAGS) -c core/first_pass.c -o core/first_pass.o

core/second_pass.o: core/second_pass.c headers/globals.h headers/second_pass.h headers/helpers.h headers/parser.h headers/sym_table.h headers/context_asm.h
	$(CC) $(CFLAGS) -c core/second_pass.c -o core/second_pass.o

core/parser.o: core/parser.c headers/globals.h headers/helpers.h headers/parser.h
	$(CC) $(CFLAGS) -c core/parser.c -o core/parser.o

core/sym_table.o: core/sym_table.c headers/globals.h headers/sym_table.h
	$(CC) $(CFLAGS) -c core/sym_table.c -o core/sym_table.o

core/helpers.o: core/helpers.c headers/globals.h headers/helpers.h
	$(CC) $(CFLAGS) -c core/helpers.c -o core/helpers.o

core/preproc.o: core/preproc.c headers/globals.h headers/helpers.h headers/preproc.h
	$(CC) $(CFLAGS) -c core/preproc.c -o core/preproc.o

core/macro_list.o: core/macro_list.c headers/globals.h headers/preproc.h
	$(CC) $(CFLAGS) -c core/macro_list.c -o core/macro_list.o

clean:
	rm -f core/*.o assembler

