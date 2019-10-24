CC = g++ $(CFLAGS)
CFLAGS = -g -O1 -I. -I.. -I/usr/X11R6/include -L/usr/X11R6/lib $(pkg-config --cflags x11-xcb) -Wall -Werror -pedantic

all: textedit keysym

textedit: TextEdit.o Text.o ../GWindow/gwindow.o
	$(CC) -o textedit TextEdit.o Text.o ../GWindow/gwindow.o -lX11

keysym: KeySym.o ../GWindow/gwindow.o
	$(CC) -o keysym KeySym.o ../GWindow/gwindow.o -lX11

KeySym.o: KeySym.cpp ../GWindow/gwindow.h
	$(CC) -c KeySym.cpp

textTst: textTst.cpp Text.o Text.h L2List.h
	$(CC) -o textTst textTst.cpp Text.o

listTst: listTst.cpp L2List.h
	$(CC) -o listTst listTst.cpp

Text.o: Text.cpp Text.h L2List.h
	$(CC) -c Text.cpp

TextEdit.o: TextEdit.cpp TextEdit.h Text.h ../GWindow/gwindow.h
	$(CC) -c TextEdit.cpp

../GWindow/gwindow.o: ../GWindow/gwindow.cpp ../GWindow/gwindow.h
	cd ../GWindow; make gwindow.o

clean:
	rm -f *.o textedit textTst listTst keysym leak.out noname.txt *\~
	cd ../GWindow; make clean
