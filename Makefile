all : zundoko.dll

zundoko.dll : zundoko.c
	gcc -I. -g -o zundoko.dll -shared zundoko.c

test : zundoko.dll
	sqlite3 < zundoko.sql
