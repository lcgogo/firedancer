// git clone https://github.com/facebook/rocksdb.git
// cd rocksdb
// make static_lib -j10
//
// back to this directory
// make

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <endian.h>

#include <rocksdb/c.h>

int main()
{
    const char *db_name = "/home/jsiegel/repos/solana/test-ledger/rocksdb";
    rocksdb_options_t *opts = rocksdb_options_create();
    char *err = NULL;

//    size_t lencf = 0;
//    char **cf = rocksdb_list_column_families(opts, db_name, &lencf, &err);
//    for (int i = 0; i < lencf; i++) {
//      printf("%s\n", cf[i]);
//    }

    const char *cfs[] = {"default", "meta", "root", "data_shred", "code_shred"};
    const rocksdb_options_t *cf_options[] = {opts, opts, opts, opts, opts}; // One per cfs

    rocksdb_column_family_handle_t* column_family_handles[5] = {NULL, NULL, NULL, NULL, NULL};

    rocksdb_t *db = rocksdb_open_for_read_only_column_families(
      opts, db_name, 5,
        (const char * const *) &cfs, 
        (const rocksdb_options_t * const*) cf_options,
        column_family_handles, 
        false, &err);

    if (err != NULL) {
        fprintf(stderr, "database open %s\n", err);
        free(err);
        return -1;
    }

    rocksdb_readoptions_t *ro = rocksdb_readoptions_create();

    rocksdb_iterator_t* iter = rocksdb_create_iterator_cf(db, ro, column_family_handles[2]);
    rocksdb_iter_seek_to_last(iter);    
    if (!rocksdb_iter_valid(iter)) {
      fprintf(stderr, "Odd, no slots?");
      rocksdb_close(db);
      return -1;
    }

    size_t klen = 0;
    const char *key = rocksdb_iter_key(iter, &klen); // There is no need to free kee
    unsigned long slot = be64toh(*((unsigned long *) key));
    printf("Last slot in the db: %ld\n", slot);
    rocksdb_iter_destroy(iter);

    rocksdb_readoptions_destroy(ro);

//    rocksdb_writeoptions_t *wo = rocksdb_writeoptions_create();
//    char *key = "name";
//    char *value = "foo";
//    rocksdb_put(db, wo, key, strlen(key), value, strlen(value), &err);
//    if (err != NULL) {
//        fprintf(stderr, "put key %s\n", err);
//        free(err);
//        rocksdb_close(db);
//        return -1;
//    }
//    free(err);
//    err = NULL;
//
//    size_t rlen;
//    value = rocksdb_get(db, ro, key, strlen(key), &rlen, &err);
//    if (err != NULL) {
//        fprintf(stderr, "get key %s\n", err);
//        free(err);
//        rocksdb_close(db);
//        return -1;
//    }
//    free(err);
//    err = NULL;
//    printf("get key len: %lu, value: %s\n", rlen, value);

    rocksdb_close(db);
    rocksdb_options_destroy(opts);

    return 0;
}
