sources = $(wildcard *.c)
objects = $(addsuffix .o, $(basename $(sources)))
flags = -g -W -Wall
target = psuedo-shell


$(target) : $(objects)
	gcc -o $(target) $(objects)

%.o : %.c
	gcc -c $(flags) $< -o $@

clean:
	rm $(target) $(objects)
