library: msocket.c 
	@echo "\n****Building library****"
	gcc -c -Wall msocket.c -o msocket.o -I. 
	ar rcs libmsocket.a msocket.o
	@echo "****Library built****\n"

user1: user1.c
	@echo "\n****Building user1****"
	gcc -c -Wall user1.c -o user1.o -I. 
	gcc user1.o -o user1 -L. -lmsocket
	@echo "****User1 built****\n"

user2: user2.c
	@echo "\n****Building user2****"
	gcc -c -Wall user2.c -o user2.o -I.
	gcc user2.o -o user2 -L. -lmsocket
	@echo "****User2 built****\n"

user3: user3.c
	@echo "\n****Building user3****"
	gcc -c -Wall user3.c -o user3.o -I.
	gcc user3.o -o user3 -L. -lmsocket
	@echo "****User3 built****\n"

user4: user4.c
	@echo "\n****Building user4****"
	gcc -c -Wall user4.c -o user4.o -I.
	gcc user4.o -o user4 -L. -lmsocket
	@echo "****User4 built****\n"

user5: user5.c
	@echo "\n****Running user5****"
	gcc -c -Wall user5.c -o user5.o -I.
	gcc user5.o -o user5 -L. -lmsocket
	@echo "****User5 built****\n"

user6: user6.c
	@echo "\n****Running user6****"
	gcc -c -Wall user6.c -o user6.o -I.
	gcc user6.o -o user6 -L. -lmsocket
	@echo "****User6 built****\n"

init: initmsocket.c
	@echo "\n****Initializing****"
	gcc initmsocket.c -o initmsocket -L. -lmsocket -I.
	@echo "****Initialized****\n"

clear:
	@echo "\n****Clearing****"
	gcc clear.c -o clear1 -I.
	@echo "****Cleared****\n"

clean:
	@echo "\n****Cleaning up****"
	rm -f *.o  *.a user1 user2 user3 user4 user5 user6 initmsocket clear1
	@echo "****Cleaned****\n"

all: clean library user1 user2 user3 user4 user5 user6 init clear
	@echo "\n****Building all****"
	@echo "****All built****\n"
