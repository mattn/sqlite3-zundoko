select load_extension("zundoko.dll");
create virtual table zundoko using zundoko(val);
select * from zundoko;

