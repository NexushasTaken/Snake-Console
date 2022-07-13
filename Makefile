# I rename my x86_64-mingw32-gcc.exe compiler to w-gcc.exe
# Runs in powershell
CC = w-gcc 
OUT = Snake.exe

all: Snake.exe run

${OUT}: main.c
	cls
	${CC} $^ -o $@

run:
	.\{OUT}
	cls

clean:
	rm ${OUT}