I=/usr/lib/jvm/java-6-openjdk/include
L=/usr/lib/jvm/java-6-openjdk/jre/lib/i386/server

SOURCES=main.c cmdqueue.c game.c gui.c level.c player.c highscores.c entity.c enemy.c collision.c
OBJECTS=$(SOURCES:.c=.o)

all: blastit

blastit: $(OBJECTS)
	gcc -lm -L $L $(OBJECTS) -ljvm -lpthread -o blastit

%.o: %.c
	gcc -Wall -I $I -I $I/linux -c $< -o $@

clean:
	rm *.o blastit

windows:
	rm -r ../windows
	mkdir ../windows
	cp *.h *.c ../windows
	cp ../original/gui.c ../original/main.c ../original/cmdqueue.* ../original/blast_it.h ../windows
