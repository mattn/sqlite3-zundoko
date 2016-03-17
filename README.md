# sqlite3-zundoko

SQLite3 でズンドコキヨシ

## Installation

Compile DLL
```
gcc -I. -g -o zundoko.dll -shared zundoko.c
```

Load DLL
```
SQLite version 3.7.14 2012-09-03 15:42:36
Enter ".help" for instructions
Enter SQL statements terminated with a ";"

sqlite> select load_extension("zundoko.dll");
load_extension("zundoko.dll")
```

ZUNDOKO
```
sqlite> SELECT * FROM ZUNDOKO;
ドコ
ドコ
ズン
ズン
ズン
ズン
ドコ
キ・ヨ・シ！
```

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a mattn)
