CFLAGS= -g -O1 -Wall -I/usr/X11R6/include -L/usr/X11R6/lib -I.. -I. -Werror -pedantic
CC= g++ $(CFLAGS)

all: func gclock mondrian bezier cursTst react

func: func.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o func func.o gwindow.o R2Graph/R2Graph.o -lX11

gclock: clock.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o gclock clock.o gwindow.o R2Graph/R2Graph.o -lX11

mondrian: mondrian.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o mondrian mondrian.o gwindow.o R2Graph/R2Graph.o -lX11

bezier: bezier.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o bezier bezier.o gwindow.o R2Graph/R2Graph.o -lX11

cursTst: cursTst.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o cursTst cursTst.o gwindow.o R2Graph/R2Graph.o -lX11

react: react.o gwindow.o R2Graph/R2Graph.o
	$(CC) -o react react.o gwindow.o R2Graph/R2Graph.o -lX11 -lrt

gwindow.o: gwindow.cpp gwindow.h
	$(CC) -c gwindow.cpp

func.o: func.cpp gwindow.h
	$(CC) -c func.cpp

clock.o: clock.cpp gwindow.h
	$(CC) -c clock.cpp

mondrian.o: mondrian.cpp gwindow.h
	$(CC) -c mondrian.cpp

besier.o: besier.cpp gwindow.h
	$(CC) -c besier.cpp

cursTst.o: cursTst.cpp gwindow.h
	$(CC) -c cursTst.cpp

react.o: react.cpp gwindow.h
	$(CC) -c react.cpp

R2Graph/R2Graph.o:
	cd R2Graph; make R2Graph.o; cd ..

gwindow.h: R2Graph/R2Graph.h

grtst: grtst.cpp gwindow.o
	$(CC) -o grtst grtst.cpp gwindow.o -lX11

clean:
	rm -f *.o func gclock mondrian bezier grtst cursTst react *\~
	cd R2Graph; make clean; cd ..
