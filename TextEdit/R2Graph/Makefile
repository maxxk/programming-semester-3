CC = g++ $(CFLAGS)
CFLAGS = -g -O1 -Wall -Werror -pedantic

graphtst: graphtst.cpp R2Graph.o R2Graph.h
	$(CC) -o graphtst graphtst.cpp R2Graph.o

R2Graph.o: R2Graph.cpp R2Graph.h
	$(CC) -c R2Graph.cpp

clean:
	rm -f *.o graphtst core*
