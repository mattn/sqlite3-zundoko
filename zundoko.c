#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <sqlite3ext.h>

#ifdef _WIN32
# define EXPORT __declspec(dllexport)
#else
# define EXPORT
#endif

SQLITE_EXTENSION_INIT1;

static int
zundoko_connect(sqlite3 *db, void *pAux, int argc, const char * const *argv, sqlite3_vtab **ppVTab, char **c) {
  char buf[256];
  snprintf(buf, sizeof(buf)-1, "CREATE TABLE %s(val text)", argv[0]);
  int rc = sqlite3_declare_vtab(db, buf);
  *ppVTab = (sqlite3_vtab *) sqlite3_malloc(sizeof(sqlite3_vtab));
  memset(*ppVTab, 0, sizeof(sqlite3_vtab));
  return rc;
}

static int
zundoko_create(sqlite3 *db, void *pAux, int argc, const char * const * argv, sqlite3_vtab **ppVTab, char **c) {
  return zundoko_connect(db, pAux, argc, argv, ppVTab, c);
}

static int zundoko_disconnect(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

static int
zundoko_destroy(sqlite3_vtab *pVTab) {
  sqlite3_free(pVTab);
  return SQLITE_OK;
}

typedef struct {
  sqlite3_vtab_cursor base;
  int rowid;
  int kiyoshi;
  char zundoko[6];
} cursor;

static int
zundoko_open(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor) {
  cursor *c = (cursor *)sqlite3_malloc(sizeof(cursor));
  *ppCursor = &c->base;
  return SQLITE_OK;
}

static int
zundoko_close(cursor *c) {
  sqlite3_free(c);
  return SQLITE_OK;
}

static int
zundoko_next(cursor *c) {
  int i, l = sizeof(c->zundoko)/sizeof(c->zundoko[0]);
  for (i = 0; i < l-1; i++)
    c->zundoko[i] = c->zundoko[i+1];
  c->zundoko[5] = c->kiyoshi ? 'K' : (rand() % 2 == 0 ? 'Z' : 'D');
  c->rowid++;
  return SQLITE_OK;
}

static int
zundoko_filter(cursor *c, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
  srand((unsigned)time(NULL));
  c->rowid = 0;
  c->kiyoshi = 0;
  memset(c->zundoko, 0, sizeof(c->zundoko));
  zundoko_next(c);
  return SQLITE_OK;
}

static int
zundoko_eof(cursor *c) {
  c->kiyoshi = memcmp(c->zundoko+1, "ZZZZD", 5) == 0;
  return c->zundoko[4] == 'K';
}

static int
zundoko_column(cursor *c, sqlite3_context *ctxt, int i) {
  char *p = "？";
  switch (c->zundoko[5]) {
	case 'Z': p = "ズン"; break;
	case 'D': p = "ドコ"; break;
	case 'K': p = "キ・ヨ・シ！"; break;
  }
  sqlite3_result_text(ctxt, strdup(p), strlen(p), NULL);
  return SQLITE_OK;
}

static int
zundoko_rowid(cursor *c, sqlite3_int64 *pRowid) {
  *pRowid = c->rowid;
  return SQLITE_OK;
}

static int
zundoko_bestindex(sqlite3_vtab *tab, sqlite3_index_info *pIdxInfo) {
  return SQLITE_OK;
}

static const sqlite3_module module = {
  0,
  zundoko_create,
  zundoko_connect,
  zundoko_bestindex,
  zundoko_disconnect,
  zundoko_destroy,
  zundoko_open,
  (int (*)(sqlite3_vtab_cursor *)) zundoko_close,
  (int (*)(sqlite3_vtab_cursor *, int, char const *, int, sqlite3_value **)) zundoko_filter,
  (int (*)(sqlite3_vtab_cursor *)) zundoko_next,
  (int (*)(sqlite3_vtab_cursor *)) zundoko_eof,
  (int (*)(sqlite3_vtab_cursor *, sqlite3_context *, int)) zundoko_column,
  (int (*)(sqlite3_vtab_cursor *, sqlite3_int64 *)) zundoko_rowid,
  NULL, // zundoko_update
  NULL, // zundoko_begin
  NULL, // zundoko_sync
  NULL, // zundoko_commit
  NULL, // zundoko_rollback
  NULL, // zundoko_findfunction
  NULL, // zundoko_rename
};

static void
destructor(void *arg) {
  return;
}


EXPORT int
sqlite3_extension_init(sqlite3 *db, char **errmsg, const sqlite3_api_routines *api) {
  SQLITE_EXTENSION_INIT2(api);
  sqlite3_create_module_v2(db, "zundoko", &module, NULL, destructor);
  return 0;
}
