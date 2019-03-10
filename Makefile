build:
	gcc -o war war.c -pthread

run: build
	./war

.PHONY: run