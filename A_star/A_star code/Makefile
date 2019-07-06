#For compiling astar

CFLAGS =-Ofast

OFILES_CREATE = createbin.o
OFILES_SOLVE = readbin.o astar.o a_star_distance.o

CC = gcc
LIBS = -lm
ALL = create solve

all: $(ALL)

create: $(OFILES_CREATE)
	$(CC) $(OFILES_CREATE) -o $@
 
solve: $(OFILES_SOLVE)
	$(CC) $(OFILES_SOLVE) $(LIBS) -o $@ 

clean:
	rm -f $(ALL) $(OFILES_CREATE) $(OFILES_SOLVE)

createbin.o: createbin.c astar.h

readbin.o: readbin.c astar.h astar.c a_star_distance.c

astar.o: astar.c astar.h

a_star_distance.o: a_star_distance.c astar.h

