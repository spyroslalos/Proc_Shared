MAINOBJ		= main.o list.o
SOURCE		= main.c list.c
MYEXE1		= exe
CC		= gcc
FLAGS		= -g -c
# -g option enables debugging mode
# -c flag generates object code for separate files


all:	$(MAINOBJ) 
	$(CC) -g $(MAINOBJ) -o $(MYEXE1) 

# create/compile the individual files >>separately<<
main.o: main.c
	$(CC) $(FLAGS) main.c
