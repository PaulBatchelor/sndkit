#ifndef WORGLE_PRIVATE_H
struct worgle_string {
    char *str;
    size_t size;
};
enum {
SEGTYPE_TEXT,
SEGTYPE_REFERENCE
};
struct worgle_segment {
    int type;
    worgle_string str;
    size_t linum;
    worgle_string *filename;
    worgle_segment *nxt;
    worgle_long id;
};
struct worgle_block {
    int nsegs;
    worgle_segment *head;
    worgle_segment *tail;
    worgle_string name;
    int am_i_used;
    worgle_block *nxt;
    int nblocks;
    int last_seg;
    worgle_long id;
};
struct worgle_blocklist {
    int nblocks;
    worgle_block *head;
    worgle_block *tail;
};
#define HASH_SIZE 256
struct worgle_hashmap {
    worgle_blocklist blk[HASH_SIZE];
    int nwords;
};
struct worgle_file {
    worgle_string filename;
    worgle_block *top;
    worgle_file *nxt;
    worgle_long id;
};
struct worgle_filelist {
    worgle_file *head;
    worgle_file *tail;
    int nfiles;
};
struct worgle_textbuf {
    char *buf;
    size_t size;
    worgle_string filename;
};
struct worgle_orglet_header {
    worgle_string str;
    int lvl;
};
struct worgle_orglet_content {
    worgle_string text;
};
struct worgle_orglet_blkref {
    worgle_block *blk;
    int pos;
    int prev_lastseg;
    int segoff;
};
struct worgle_orglet {
    int type;
    void *ud;
    worgle_orglet *next;
    size_t linum;
    worgle_long id;
};
struct worgle_orgfile {
    worgle_orglet *head;
    worgle_orglet *tail;
    worgle_orgfile *prev;
    int size;
};
struct worgle_d {
    worgle_string block;
    #ifndef WORGLITE
    worgle_string segblock;
    #endif
    long curline;
    int block_started;
    #ifndef WORGLITE
    int new_content;
    #endif
    worgle_hashmap dict;
    worgle_filelist flist;
    worgle_textbuf *curbuf;
    worgle_textbuf *buffers;
    int nbuffers;
    worgle_block *curblock;
    size_t linum;
    worgle_orgfile *orgs;
    worgle_orgfile *curorg;
    int prog;
    worgle_long UUID_count;
};
#define WORGLE_PRIVATE_H
#endif
