all: Snake.exe run

Snake.exe: main.c
	cls	
	w-gcc $^ -o $@

run:
	.\main.exe
	cls
