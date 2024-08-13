all:
	gcc -Ofast bitboardchess.c -o bitboardchess
	x86_64-w64-mingw32-gcc -Ofast bitboardchess.c -o bitboardchess.exe

debug:
	gcc bitboardchess.c -o bitboardchess
	x86_64-w64-mingw32-gcc bitboardchess.c -o bitboardchess.exe