
all: vending

vending: vending-machine.o consumer.o consumer.h supplier.o supplier.h monitor.h
	gcc -Wall vending-machine.o consumer.o supplier.o -o vending -pthread

vending-machine.o: vending-machine.c consumer.h supplier.h monitor.h 
	gcc -g -c vending-machine.c 

consumer.o: consumer.c consumer.h
	gcc -g -c consumer.c

supplier.o: supplier.c supplier.h
	gcc -g -c supplier.c

clean: 
	rm -f vending-machine.o consumer.o supplier.o vending
