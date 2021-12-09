#ifndef GRAFORGE_H
#define GRAFORGE_H
#include <stdio.h>
#include <stdarg.h>

#ifndef GFFLT
#define GFFLT float
#endif

enum {
    GF_OK,
    GF_NOT_OK,
    GF_INVALID_CABLE,
    GF_INVALID_NODE,
    GF_INVALID_BUFFER,
    GF_ALREADY_ALLOCATED,
    GF_CONNECTION_MISMATCH,
    GF_INVALID_ENTRY,
    GF_NOT_ENOUGH_NODES,
    GF_NULL_VALUE,
    GF_I_DONT_KNOW,
    GF_POOL_FULL,
    GF_STACK_OVERFLOW
};


typedef struct gf_node gf_node;
typedef struct gf_pointer gf_pointer;
typedef void(*gf_function)(gf_node*);
typedef void(*gf_nodefun)(gf_node*,void*);
typedef struct gf_cable gf_cable;
typedef void(*gf_pointer_function)(gf_pointer*p);
typedef struct gf_buffer gf_buffer;
typedef struct gf_bufferpool gf_bufferpool;
typedef struct gf_stack gf_stack;
typedef struct gf_patch gf_patch;
typedef int(*gf_mallocfun)(gf_patch*,size_t,void**);
typedef int(*gf_freefun)(gf_patch*,void**);


typedef struct {
    gf_pointer*root;
    gf_pointer*last;
    unsigned int size;
} gf_pointerlist;


typedef struct {
    gf_node*nodes;
    gf_node*last;
    int nnodes;
    gf_cable*out;
    gf_pointerlist plist;
} gf_subpatch;

struct gf_cable {
    gf_node*node;
    GFFLT*val;
    GFFLT ival;
    GFFLT*blk;
    int blksize;
    unsigned char type;
    gf_cable*pcable;
    gf_buffer*buf;
};

size_t gf_node_size();
void gf_node_init(gf_node*node,int blksize);
int gf_node_get_id(gf_node*node);

void gf_node_set_id(gf_node*node,int id);
void gf_node_set_setup(gf_node*node,gf_function fun);
void gf_node_set_compute(gf_node*node,gf_function fun);
void gf_node_set_destroy(gf_node*node,gf_function fun);
void gf_node_setup(gf_node*node);
void gf_node_compute(gf_node*node);
void gf_node_destroy(gf_node*node);
void gf_node_set_data(gf_node*node,void*data);
void*gf_node_get_data(gf_node*node);
int gf_node_cables_alloc(gf_node*node,int ncables);
int gf_node_cables_free(gf_node*node);
int gf_node_get_cable(gf_node*node,int id,gf_cable**cable);
int gf_node_blksize(gf_node*node);
int gf_node_set_block(gf_node*node,int id);
int gf_node_get_ncables(gf_node*node);
int gf_node_get_type(gf_node*node);
void gf_node_set_type(gf_node*node,int type);
void gf_node_set_patch(gf_node*node,gf_patch*patch);
int gf_node_get_patch(gf_node*node,gf_patch**patch);
gf_node*gf_node_get_next(gf_node*node);
void gf_node_set_next(gf_node*node,gf_node*next);

void gf_cable_init(gf_node*node,gf_cable*cable);
void gf_cable_free(gf_cable*cable);
void gf_cable_set_block(gf_cable*cable,GFFLT*blk,int blksize);
void gf_cable_set_constant(gf_cable*cable,GFFLT val);
void gf_cable_set_value(gf_cable*c,GFFLT val);
GFFLT gf_cable_get(gf_cable*cable,int pos);
void gf_cable_set(gf_cable*cable,int pos,GFFLT val);
int gf_cable_connect(gf_cable*c1,gf_cable*c2);
void gf_cable_connect_nocheck(gf_cable*c1,gf_cable*c2);
int gf_cable_pop(gf_cable*cab);
void gf_cable_push(gf_cable*cab);
int gf_cable_is_block(gf_cable*cab);
int gf_cable_is_constant(gf_cable*cab);
gf_buffer*gf_cable_get_buffer(gf_cable*cab);
void gf_cable_set_buffer(gf_cable*cab,gf_buffer*buf);
int gf_cable_make_block(gf_cable*cable,gf_stack*stack,int blksize);
int gf_cable_clear(gf_cable*cab);
int gf_cable_mix(gf_cable*in,gf_cable*sum,GFFLT mix);
int gf_cable_blksize(gf_cable*cable);
void gf_cable_override(gf_cable*c1,gf_cable*c2);
void gf_cable_copy(gf_cable*c1,gf_cable*c2);

const char*gf_error(int rc);

#define GF_ERROR_CHECK(rc) if(rc != GF_OK) return rc

int gf_pointer_create(gf_patch*patch,
                      gf_pointer**pointer,
                      gf_pointer_function free,
                      void*ud);

void gf_pointer_destroy(gf_pointer**pointer);
void*gf_pointer_data(gf_pointer*pointer);
void gf_pointer_set_type(gf_pointer*pointer,int id);
int gf_pointer_get_type(gf_pointer*pointer);
gf_pointer*gf_pointer_get_next(gf_pointer*p);
void gf_pointer_set_next(gf_pointer*p,gf_pointer*next);
void gf_pointerlist_init(gf_pointerlist*plist);
gf_pointer*gf_pointerlist_top(gf_pointerlist*plist);
void gf_pointerlist_append(gf_pointerlist*plist,gf_pointer*p);
void gf_pointerlist_free(gf_pointerlist*plist);

size_t gf_buffer_size(void);
void gf_buffer_alloc(gf_patch*patch,gf_buffer*buf,int size);
void gf_buffer_free(gf_patch*patch,gf_buffer*buf);
void gf_buffer_init(gf_buffer*buf);
void gf_buffer_reinit(gf_buffer*buf);
void gf_buffer_mark(gf_buffer*buf);
int gf_buffer_unmark(gf_buffer*buf);
GFFLT*gf_buffer_data(gf_buffer*buf);
void gf_buffer_hold(gf_buffer*buf);
void gf_buffer_holdu(gf_buffer*buf);
int gf_buffer_unhold(gf_buffer*buf);
int gf_buffer_id(gf_buffer*buf);

void gf_bufferpool_init(gf_bufferpool*pool);
void gf_bufferpool_create(gf_patch*patch,
gf_bufferpool*pool,int nbuf,int blksize);
void gf_bufferpool_reset(gf_bufferpool*pool);
void gf_bufferpool_destroy(gf_patch*patch,gf_bufferpool*pool);
int gf_bufferpool_nactive(gf_bufferpool*pool);
int gf_bufferpool_unhold(gf_bufferpool*pool,gf_buffer*buf);
int gf_bufferpool_nextfree(gf_bufferpool*pool,gf_buffer**buf);
int gf_bufferpool_holdu(gf_bufferpool*pool,gf_buffer*buf);
int gf_bufferpool_unholdu(gf_bufferpool*pool,gf_buffer*buf);
int gf_bufferpool_unholdu_all(gf_bufferpool*pool);
int gf_bufferpool_uactive(gf_bufferpool*pool);
void gf_bufferpool_clear_last_free(gf_bufferpool*pool);

void gf_stack_init(gf_stack*stack,gf_bufferpool*pool);
int gf_stack_alloc(gf_patch*patch,gf_stack*stack,int size);
int gf_stack_free(gf_patch*patch,gf_stack*stack);
int gf_stack_push(gf_stack*stack,gf_buffer**buf);
int gf_stack_push_buffer(gf_stack*stack,gf_buffer*buf);
int gf_stack_pop(gf_stack*stack,gf_buffer**buf);
int gf_stack_dup(gf_stack*stack);
int gf_stack_drop(gf_stack*stack);
int gf_stack_swap(gf_stack*stack);
int gf_stack_hold(gf_stack*stack,gf_buffer**buf);
int gf_stack_size(gf_stack*stack);
int gf_stack_pos(gf_stack*stack);
void gf_stack_reset(gf_stack*stack);

void gf_patch_init(gf_patch*patch,int blksize);
void gf_patch_alloc(gf_patch*patch,int nbuffers,int stack_size);
void gf_patch_realloc(gf_patch*patch,
                      int nbuffers,
                      int stack_size,
                      int blksize);
void gf_patch_reinit(gf_patch*patch);
void gf_patch_clear(gf_patch*patch);
void gf_patch_free_nodes(gf_patch*patch);
void gf_patch_setup(gf_patch*patch);
void gf_patch_destroy(gf_patch*patch);
void gf_patch_compute(gf_patch*patch);
void gf_patch_set_out(gf_patch*patch,gf_cable*cable);
gf_cable*gf_patch_get_out(gf_patch*patch);
size_t gf_patch_size(void);
GFFLT gf_patch_tick(gf_patch*patch);
int gf_patch_new_node(gf_patch*patch,gf_node**node);
int gf_patch_new_cable(gf_patch*patch,gf_cable**cable);
int gf_patch_append_userdata(gf_patch*patch,
                             gf_pointer_function dfun,
                             void*ud);
gf_stack*gf_patch_stack(gf_patch*patch);
int gf_patch_blksize(gf_patch*patch);
int gf_patch_blksize_set(gf_patch *patch, int blksize);
gf_bufferpool*gf_patch_pool(gf_patch*patch);
void gf_patch_srate_set(gf_patch*patch,int sr);
int gf_patch_srate_get(gf_patch*patch);
void gf_patch_data_set(gf_patch*patch,void*ud);
void*gf_patch_data_get(gf_patch*patch);
void gf_patch_holdbuf(gf_patch*patch,gf_cable*c);
void gf_patch_unholdbuf(gf_patch*patch,gf_cable*c);
int gf_patch_bhold(gf_patch*patch,gf_buffer**b);
int gf_patch_bunhold(gf_patch*patch,gf_buffer*b);
void gf_patch_err(gf_patch*patch,int rc);
gf_node*gf_patch_last_node(gf_patch*patch);

void gf_subpatch_init(gf_subpatch*subpatch);
void gf_subpatch_save(gf_patch*patch,gf_subpatch*subpatch);
void gf_subpatch_restore(gf_patch*patch,gf_subpatch*subpatch);
void gf_subpatch_compute(gf_subpatch*subpatch);
void gf_subpatch_destroy(gf_subpatch*subpatch);
void gf_subpatch_free(gf_subpatch*subpatch);
gf_cable*gf_subpatch_out(gf_subpatch*subpatch);

int gf_memory_alloc(gf_patch*p,size_t size,void**ud);
int gf_memory_free(gf_patch*p,void**ud);
void gf_memory_override(gf_patch*p,gf_mallocfun m,gf_freefun f);
void gf_memory_defaults(gf_patch*p);
void gf_print(gf_patch*p,const char*fmt,...);
void gf_print_init(gf_patch*p);
void gf_print_set(gf_patch*p,void(*print)(gf_patch*,const char*,va_list));

#ifdef __plan9__
#pragma incomplete gf_buffer
#pragma incomplete gf_patch
#pragma incomplete gf_node
#pragma incomplete gf_pointer
#pragma incomplete gf_stack
#endif

#endif
