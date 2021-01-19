#ifndef WORGLE_H
#define WORGLE_H
typedef struct worgle_d worgle_d;
typedef struct worgle_string worgle_string;
typedef struct worgle_segment worgle_segment;
typedef struct worgle_block worgle_block;
typedef struct worgle_blocklist worgle_blocklist;
typedef struct worgle_hashmap worgle_hashmap;
typedef struct worgle_file worgle_file;
typedef struct worgle_filelist worgle_filelist;
typedef struct worgle_textbuf worgle_textbuf;
typedef unsigned long worgle_long;
typedef struct worgle_orgfile worgle_orgfile;
typedef struct worgle_orglet worgle_orglet;
typedef struct worgle_orglet_header worgle_orglet_header;
typedef struct worgle_orglet_content worgle_orglet_content;
typedef struct worgle_orglet_blkref worgle_orglet_blkref;
int parse_file(worgle_d *worg, int file);
void worgle_begin_block(worgle_d *worg, worgle_string *name);
void worgle_append_file(worgle_d *worg, worgle_string *filename);
void worgle_append_string(worgle_d *worg);
void worgle_append_reference(worgle_d *worg, worgle_string *ref);
int worgle_generate(worgle_d *worg);
void worgle_init(worgle_d *worg);
void worgle_free(worgle_d *worg);
void worgle_string_reset(worgle_string *str);
void worgle_string_init(worgle_string *str);
int worgle_string_write(FILE *fp, worgle_string *str);
void worgle_segment_init(worgle_segment *s,
                        int type,
                        worgle_string *str,
                        worgle_string *filename,
                        size_t linum);
int worgle_segment_write(worgle_segment *s, worgle_hashmap *h, FILE *fp);
int worgle_segment_is_text(worgle_segment *s);
int worgle_segment_is_reference(worgle_segment *s);
void worgle_block_init(worgle_block *b);
void worgle_block_free(worgle_block *lst);
worgle_segment* worgle_block_append_segment(worgle_block *b,
                                            worgle_string *str,
                                            int type,
                                            size_t linum,
                                            worgle_string *filename);
worgle_segment* worgle_block_append_string(worgle_block *b,
                               worgle_string *str,
                               size_t linum,
                               worgle_string *filename);
worgle_segment* worgle_block_append_reference(worgle_block *b,
                                              worgle_string *str,
                                              size_t linum,
                                              worgle_string *filename);
int worgle_block_write(worgle_block *b, worgle_hashmap *h, FILE *fp);
void worgle_blocklist_init(worgle_blocklist *lst);
void worgle_blocklist_free(worgle_blocklist *lst);
void worgle_blocklist_append(worgle_blocklist *lst, worgle_block *b);
void worgle_hashmap_init(worgle_hashmap *h);
void worgle_hashmap_free(worgle_hashmap *h);
int worgle_hashmap_find(worgle_hashmap *h, worgle_string *name, worgle_block **b);
worgle_block * worgle_hashmap_get(worgle_hashmap *h, worgle_string *name);
int worgle_file_write(worgle_file *f, worgle_hashmap *h);
void worgle_filelist_init(worgle_filelist *flist);
void worgle_filelist_free(worgle_filelist *flist);
worgle_file* worgle_filelist_append(worgle_filelist *flist,
                           worgle_string *name,
                           worgle_block *top);
int worgle_filelist_write(worgle_filelist *flist, worgle_hashmap *h);
void worgle_textbuf_zero(worgle_textbuf *txt);
void worgle_textbuf_init(worgle_textbuf *txt,
                         char *buf,
                         size_t bufsize);
void worgle_textbuf_free(worgle_textbuf *txt);
worgle_long worgle_uuid_new(worgle_d *worg);
worgle_long worgle_segment_id_get(worgle_segment *s);
void worgle_segment_id_set(worgle_segment *s, worgle_long id);
worgle_long worgle_block_id_get(worgle_block *b);
void worgle_block_id_set(worgle_block *b, worgle_long id);
void worgle_file_id_set(worgle_file *f, worgle_long id);
void worgle_orglet_id_set(worgle_orglet *orglet,
                           worgle_long id);
void worgle_orgfile_init(worgle_orgfile *org);
void worgle_orgfile_free(worgle_orgfile *org);
void worgle_orgfile_append(worgle_d *worg,
                           worgle_orgfile *file,
                           worgle_orglet *orglet);
void worgle_orgfile_append_header(worgle_d *worg,
                                  worgle_string *header,
                                  int lvl);
void worgle_orgfile_append_content(worgle_d *worg,
                                   worgle_string *text);
void worgle_orgfile_append_reference(worgle_d *worg,
                                     worgle_block *blk);
void worgle_orglet_init(worgle_orglet *orglet);
void worgle_orglet_free(worgle_orglet *orglet);
int worgle_warn_unused(worgle_d *worg);
void worgle_map(worgle_d *worg, worgle_block *b, int lvl, FILE *out);
void worgle_map_files(worgle_d *worg, char *filename);
void worgle_map_a_file(worgle_d *worg, worgle_file *file, FILE *out);
#endif
