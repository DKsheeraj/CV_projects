all: Master sched process MMU 
	
Master: Master.c
	gcc -o Master Master.c -lm

sched: sched.c
	gcc -o sched sched.c 

process: process.c
	gcc -o process process.c

MMU: MMU.c
	gcc -o MMU MMU.c

result:
	cat frequency.txt pageFault.txt globalOrdering.txt invalidPageReference.txt > result.txt

clean: 
	rm Master sched process MMU frequency.txt pageFault.txt globalOrdering.txt invalidPageReference.txt result.txt