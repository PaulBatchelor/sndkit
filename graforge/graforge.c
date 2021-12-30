#include <stdlib.h>
#include <stdio.h>
#include "graforge.h"

struct gf_pointer {
    gf_patch *patch;
    int type;
    void *ud;
    gf_pointer_function free;
    gf_pointer *next;
};

struct gf_buffer {
    int id;
    int read;
    GFFLT *buf;
};

struct gf_node {
    gf_patch *patch;
    int id;
    gf_function destroy;
    gf_function compute;
    void *ud;
    gf_cable *cables;
    int ncables;
    int blksize;
    int type;
    int group;
    gf_node *next;
};

struct gf_bufferpool {
    gf_buffer *buffers;
    int size;
    int nactive;
    int last_free;
    int usrnactive;
};

struct gf_stack {
    int pos;
    int size;
    gf_bufferpool *pool;
    gf_buffer **buffers;
};

struct gf_patch {
    gf_node *nodes;
    gf_node *last;
    int nnodes;
    int blksize;
    int counter;
    int nodepos;
    gf_pointerlist plist;
    gf_bufferpool pool;
    gf_stack stack;
    int sr;
    void *ud;
    int err;
    gf_mallocfun malloc;
    gf_freefun free;
    void (*print)(gf_patch *, const char *fmt, va_list);
};

size_t gf_node_size()
{
    return sizeof(gf_node);
}

static void empty(gf_node *node)
{
}

static void free_cables(gf_node *node)
{
    gf_node_cables_free(node);
}

void gf_node_init(gf_node *node, int blksize)
{
    node->id = -1;
    node->compute = empty;
    node->destroy = free_cables;
    node->ncables = 0;
    node->blksize = blksize;
    node->type = -1;
    node->group = -1;
    node->next = NULL;
}

int gf_node_get_id(gf_node *node)
{
    return node->id;
}

void gf_node_set_id(gf_node *node, int id)
{
    node->id = id;
}

void gf_node_set_compute(gf_node *node, gf_function fun)
{
    node->compute = fun;
}

void gf_node_set_destroy(gf_node *node, gf_function fun)
{
    node->destroy = fun;
}


void gf_node_compute(gf_node *node)
{
    node->compute(node);
}

void gf_node_destroy(gf_node *node)
{
    node->destroy(node);
}

void gf_node_set_data(gf_node *node, void *data)
{
    node->ud = data;
}

void *gf_node_get_data(gf_node *node)
{
    return node->ud;
}

int gf_node_cables_alloc(gf_node *node, int ncables)
{
    int n;
    int rc;
    if (node->patch == NULL)
	return GF_NOT_OK;
    rc = gf_memory_alloc(node->patch,
			 sizeof(gf_cable) *ncables,
			 (void **) &node->cables);
    if (rc != GF_OK)
	return rc;

    node->ncables = ncables;
    for (n = 0; n < ncables; n++) {
	gf_cable_init(node, &node->cables[n]);
    }

    return GF_OK;
}

int gf_node_cables_free(gf_node *node)
{
    int n;
    if (node->patch == NULL)
	return GF_NOT_OK;
    if (node->ncables > 0) {
	for (n = 0; n < node->ncables; n++) {
	    gf_cable_free(&node->cables[n]);
	}
	return gf_memory_free(node->patch, (void **) &node->cables);
    }

    return GF_NOT_OK;
}

int gf_node_get_cable(gf_node *node, int id, gf_cable ** cable)
{
    if (id >= node->ncables) {
	return GF_INVALID_CABLE;
    }

    *cable = &node->cables[id];

    return GF_OK;
}

int gf_node_blksize(gf_node *node)
{
    return node->blksize;
}

int gf_node_set_block(gf_node *node, int id)
{
    int rc;
    if (id > node->ncables) {
	return GF_INVALID_CABLE;
    }

    rc = gf_cable_make_block(&node->cables[id],
			     gf_patch_stack(node->patch), node->blksize);

    if (rc != GF_OK) {
	gf_patch_err(node->patch, GF_POOL_FULL);
    }

    return rc;
}

int gf_node_get_ncables(gf_node *node)
{
    return node->ncables;
}

int gf_node_get_type(gf_node *node)
{
    return node->type;
}

void gf_node_set_type(gf_node *node, int type)
{
    node->type = type;
}

void gf_node_set_patch(gf_node *node, gf_patch *patch)
{
    node->patch = patch;
}

int gf_node_get_patch(gf_node *node, gf_patch ** patch)
{
    *patch = node->patch;
    return GF_OK;
}

gf_node *gf_node_get_next(gf_node *node)
{
    return node->next;
}

void gf_node_set_next(gf_node *node, gf_node *next)
{
    node->next = next;
}


enum {
    CABLE_IVAL,
    CABLE_BLOCK
};

void gf_cable_init(gf_node *node, gf_cable *cable)
{
    cable->ival = 0;
    cable->blksize = 0;
    cable->blk = NULL;
    cable->type = CABLE_IVAL;
    cable->val = &cable->ival;
    cable->node = node;
    cable->pcable = cable;
    cable->buf = NULL;
}

void gf_cable_free(gf_cable *cable)
{
    if (cable->blksize != 0) {

    }
}

void gf_cable_set_block(gf_cable *cable, GFFLT *blk, int blksize)
{
    cable->blk = blk;
    cable->blksize = blksize;
    cable->val = cable->blk;
    cable->type = CABLE_BLOCK;
}

void gf_cable_set_constant(gf_cable *cable, GFFLT val)
{
    cable->val = &cable->ival;
    cable->ival = val;
    cable->type = CABLE_IVAL;
}

void gf_cable_set_value(gf_cable *c, GFFLT val)
{
    *c->val = val;
}

GFFLT gf_cable_get(gf_cable *cable, int pos)
{
    if (cable->type == CABLE_IVAL) {
	return *cable->val;
    } else {
	return cable->val[pos];
    }
}

void gf_cable_set(gf_cable *cable, int pos, GFFLT val)
{
    if (cable->type == CABLE_IVAL) {
	*cable->val = val;
    } else {
	cable->val[pos] = val;
    }
}

int gf_cable_connect(gf_cable *c1, gf_cable *c2)
{
    int id1, id2;

    id1 = gf_node_get_id(c1->node);
    id2 = gf_node_get_id(c2->node);

    if (id1 > id2)
	return GF_CONNECTION_MISMATCH;

    gf_cable_connect_nocheck(c1, c2);

    return GF_OK;
}

void gf_cable_connect_nocheck(gf_cable *c1, gf_cable *c2)
{
    c2->type = c1->type;
    gf_cable_override(c1, c2);
}

int gf_cable_pop(gf_cable *cab)
{
    gf_stack *stack;
    gf_node *node;
    gf_buffer *tmp;

    if (!gf_cable_is_block(cab))
	return GF_NOT_OK;

    node = cab->node;
    stack = gf_patch_stack(node->patch);
    gf_stack_pop(stack, &tmp);

    return GF_OK;
}

void gf_cable_push(gf_cable *cab)
{
    gf_stack *stack;
    gf_node *node;
    gf_buffer *tmp;
    tmp = NULL;

    node = cab->node;
    stack = gf_patch_stack(node->patch);
    gf_stack_push(stack, &tmp);
}

int gf_cable_is_block(gf_cable *cab)
{
    return cab->type == CABLE_BLOCK;
}

int gf_cable_is_constant(gf_cable *cab)
{
    return cab->type == CABLE_IVAL;
}

gf_buffer *gf_cable_get_buffer(gf_cable *cab)
{
    return cab->buf;
}

void gf_cable_set_buffer(gf_cable *cab, gf_buffer *buf)
{
    cab->buf = buf;
}

int gf_cable_make_block(gf_cable *cable, gf_stack *stack, int blksize)
{
    gf_buffer *buf;
    GFFLT *blk;

    buf = NULL;

    if (gf_stack_push(stack, &buf) != GF_OK) {
	return GF_NOT_OK;
    }

    blk = gf_buffer_data(buf);
    gf_cable_set_block(cable, blk, blksize);
    gf_cable_set_buffer(cable, buf);

    return GF_OK;
}

int gf_cable_clear(gf_cable *cab)
{
    int i;
    if (!gf_cable_is_block(cab))
	return GF_NOT_OK;

    for (i = 0; i < cab->blksize; i++) {
	cab->val[i] = 0;
    }

    return GF_OK;
}

int gf_cable_mix(gf_cable *in, gf_cable *sum, GFFLT mix)
{
    int i;
    if (!gf_cable_is_block(in) || !gf_cable_is_block(sum))
	return GF_NOT_OK;

    for (i = 0; i < sum->blksize; i++) {
	sum->val[i] += in->val[i] *mix;
    }

    return GF_OK;
}

int gf_cable_blksize(gf_cable *cable)
{
    return cable->blksize;
}

void gf_cable_override(gf_cable *c1, gf_cable *c2)
{
    c2->val = c1->val;
    c2->pcable = c1;
    c2->blksize = c1->blksize;
}

void gf_cable_copy(gf_cable *c1, gf_cable *c2)
{
    int blksize;
    int n;
    GFFLT tmp;

    blksize = c1->blksize;

    for (n = 0; n < blksize; n++) {
	tmp = gf_cable_get(c1, n);
	gf_cable_set(c2, n, tmp);
    }
}


static const char *errmsg[] = {
    "Everything is okay!",
    "Oops! Something went wrong.",
    "Invalid cable.",
    "Invalid node.",
    "Invalid buffer.",
    "This thing has already been allocated.",
    "Cables can only connect to nodes with a higher id.",
    "Position out of range in ugen list.",
    "Patch is out of nodes. Consider allocating more nodes.",
    "Attempted to read a NULL value.",
    "I'm not actually sure what happened.",
    "Buffer Pool is Full.",
    "Stack Overflow.",
};


const char *gf_error(int rc)
{
    if (rc >= (sizeof(errmsg) / sizeof(*errmsg)) || rc < 0) {
	return errmsg[GF_I_DONT_KNOW];
    } else {
	return errmsg[rc];
    }
}

int gf_pointer_create(gf_patch *patch,
		      gf_pointer ** pointer,
		      gf_pointer_function free, void *ud)
{
    gf_pointer *pptr;
    int rc;

    rc = gf_memory_alloc(patch, sizeof(gf_pointer), (void **) &pptr);

    if (rc != GF_OK)
	return rc;

    *pointer = pptr;
    pptr->patch = patch;
    pptr->next = NULL;
    pptr->type = 0;
    pptr->ud = ud;
    pptr->free = free;
    return GF_OK;
}

void gf_pointer_destroy(gf_pointer ** pointer)
{
    (*pointer)->free(*pointer);
    gf_memory_free((*pointer)->patch, (void **) pointer);
}

void *gf_pointer_data(gf_pointer *pointer)
{
    return pointer->ud;
}

void gf_pointer_set_type(gf_pointer *pointer, int id)
{
    pointer->type = id;
}

int gf_pointer_get_id(gf_pointer *pointer)
{
    return pointer->type;
}

gf_pointer *gf_pointer_get_next(gf_pointer *p)
{
    return p->next;
}

void gf_pointer_set_next(gf_pointer *p, gf_pointer *next)
{
    p->next = next;
}

void gf_pointerlist_init(gf_pointerlist *plist)
{
    plist->root = NULL;
    plist->last = NULL;
    plist->size = 0;
}

gf_pointer *gf_pointerlist_top(gf_pointerlist *plist)
{
    return plist->root;
}

void gf_pointerlist_append(gf_pointerlist *plist, gf_pointer *p)
{
    if (plist->size == 0) {
	plist->root = p;
    } else {
	gf_pointer_set_next(plist->last, p);
    }

    plist->last = p;
    plist->size++;
}

void gf_pointerlist_free(gf_pointerlist *plist)
{
    unsigned int i;
    gf_pointer *next;
    gf_pointer *val;

    val = gf_pointerlist_top(plist);
    for (i = 0; i < plist->size; i++) {
	next = gf_pointer_get_next(val);
	gf_pointer_destroy(&val);
	val = next;
    }
}

size_t gf_buffer_size(void)
{
    return sizeof(gf_buffer);
}

void gf_buffer_alloc(gf_patch *patch, gf_buffer *buf, int size)
{
    gf_memory_alloc(patch, sizeof(GFFLT) *size, (void **) &buf->buf);
}

void gf_buffer_free(gf_patch *patch, gf_buffer *buf)
{
    gf_memory_free(patch, (void **) &buf->buf);
}

void gf_buffer_init(gf_buffer *buf)
{
    buf->id = -1;
    gf_buffer_reinit(buf);
}

void gf_buffer_reinit(gf_buffer *buf)
{
    buf->read = 0;
}

void gf_buffer_mark(gf_buffer *buf)
{
    if (buf->read >= 0)
	buf->read++;
}

int gf_buffer_unmark(gf_buffer *buf)
{
    if (buf->read < 0)
	return -3;
    if (buf->read == 0)
	return -2;
    buf->read--;
    if (buf->read == 0)
	return buf->id;
    return -1;
}

GFFLT *gf_buffer_data(gf_buffer *buf)
{
    return buf->buf;
}

void gf_buffer_hold(gf_buffer *buf)
{
    buf->read = -1;
}

void gf_buffer_holdu(gf_buffer *buf)
{
    buf->read = -2;
}

int gf_buffer_unhold(gf_buffer *buf)
{
    if (buf->read < 0) {
	buf->read = 0;
	return 1;
    } else {
	return 0;
    }
}

int gf_buffer_id(gf_buffer *buf)
{
    return buf->id;
}

void gf_bufferpool_init(gf_bufferpool *pool)
{
    pool->size = 0;
    pool->nactive = 0;
    pool->usrnactive = 0;
}

void gf_bufferpool_create(gf_patch *patch,
			  gf_bufferpool *pool, int nbuf, int blksize)
{
    int i;
    pool->size = nbuf;
    pool->nactive = 0;
    gf_memory_alloc(patch,
		    sizeof(gf_buffer) *nbuf, (void **) &pool->buffers);

    for (i = 0; i < nbuf; i++) {
	gf_buffer_alloc(patch, &pool->buffers[i], blksize);
	gf_buffer_init(&pool->buffers[i]);
	pool->buffers[i].id = i;
    }
}

void gf_bufferpool_reset(gf_bufferpool *pool)
{
    int i;
    pool->last_free = -1;
    pool->nactive = 0;
    for (i = 0; i < pool->size; i++) {
        if (pool->buffers[i].read >= 0) {
            gf_buffer_reinit(&pool->buffers[i]);
        } else {
            pool->nactive++;
        }
    }
}

void gf_bufferpool_destroy(gf_patch *patch, gf_bufferpool *pool)
{
    int i;

    for (i = 0; i < pool->size; i++) {
        gf_buffer_free(patch, &pool->buffers[i]);
    }

    if (pool->size > 0) {
        gf_memory_free(patch, (void **) &pool->buffers);
    }
}

int gf_bufferpool_nactive(gf_bufferpool *pool)
{
    return pool->nactive;
}

int gf_bufferpool_unhold(gf_bufferpool *pool, gf_buffer *buf)
{
    if (buf->id < 0) return 0;
    if (gf_buffer_unhold(buf)) {
        pool->nactive--;
        return 1;
    } else {
        return 0;
    }
}

int gf_bufferpool_nextfree(gf_bufferpool *pool, gf_buffer ** buf)
{
    int i;
    if (pool->nactive >= pool->size) {
        return GF_POOL_FULL;
    } else if (pool->last_free >= 0) {
        *buf = &pool->buffers[pool->last_free];
        pool->last_free = -1;
    } else {
        for (i = 0; i < pool->size; i++) {
            if (pool->buffers[i].read == 0) {
                *buf = &pool->buffers[i];
                break;
            }
        }
    }

    pool->nactive++;
    gf_buffer_mark(*buf);

    return GF_OK;
}

int gf_bufferpool_holdu(gf_bufferpool *pool, gf_buffer *buf)
{
    if (buf == NULL) return GF_NULL_VALUE;
    if (buf->read >= 0) {
        gf_buffer_holdu(buf);
        pool->usrnactive++;
        pool->nactive++;
        return GF_OK;
    }
    return GF_INVALID_BUFFER;
}

int gf_bufferpool_unholdu(gf_bufferpool *pool, gf_buffer *buf)
{
    if (buf == NULL) return GF_NULL_VALUE;
    if (buf->id == -1) return GF_OK;
    if (buf->read != -2) return GF_INVALID_BUFFER;
    if (!gf_buffer_unhold(buf)) return GF_NOT_OK;
    pool->nactive--;
    pool->usrnactive--;
    return GF_OK;
}

int gf_bufferpool_unholdu_all(gf_bufferpool *pool)
{
    int i;
    if (pool->usrnactive == 0) return GF_NOT_OK;
    for (i = 0; i < pool->size; i++) {
        gf_bufferpool_unholdu(pool, &pool->buffers[i]);
    }
    return GF_OK;
}

int gf_bufferpool_uactive(gf_bufferpool *pool)
{
    return pool->usrnactive;
}

void gf_bufferpool_clear_last_free(gf_bufferpool *pool)
{
    pool->last_free = -1;
}

void gf_stack_init(gf_stack *stack, gf_bufferpool *pool)
{
    stack->pool = pool;
    stack->pos = 0;
    stack->size = 0;
}

int gf_stack_alloc(gf_patch *patch, gf_stack *stack, int size)
{
    stack->size = size;
    return gf_memory_alloc(patch,
			   sizeof(gf_buffer *) *size,
			   (void **) &stack->buffers);
}

int gf_stack_free(gf_patch *patch, gf_stack *stack)
{
    return gf_memory_free(patch, (void **) &stack->buffers);
}

int gf_stack_push(gf_stack *stack, gf_buffer ** buf)
{
    gf_buffer *pbuf;
    int rc;

    pbuf = NULL;

    if (stack->pos >= stack->size) {
	return GF_STACK_OVERFLOW;
    }

    rc = gf_bufferpool_nextfree(stack->pool, &pbuf);

    if (rc != GF_OK)
	return rc;

    stack->buffers[stack->pos] = pbuf;
    stack->pos++;
    if (buf != NULL)
	*buf = pbuf;
    return GF_OK;
}

int gf_stack_push_buffer(gf_stack *stack, gf_buffer *buf)
{
    if (stack->pos >= stack->size) {
	return GF_STACK_OVERFLOW;
    }
    stack->buffers[stack->pos] = buf;
    stack->pos++;
    return GF_OK;
}

static int pop_from_stack(gf_stack *stack, gf_buffer ** buf)
{
    if (stack->pos == 0)
	return GF_NOT_OK;
    *buf = stack->buffers[stack->pos - 1];
    stack->pos--;
    return GF_OK;
}

int gf_stack_pop(gf_stack *stack, gf_buffer ** buf)
{
    int rc;
    gf_buffer *tmp;

    rc = pop_from_stack(stack, &tmp);
    if (rc != GF_OK)
	return GF_NOT_OK;

    rc = gf_buffer_unmark(tmp);
    if (rc >= 0) {
	stack->pool->last_free = rc;
	stack->pool->nactive--;
    }
    if (buf != NULL)
	*buf = tmp;
    return GF_OK;
}

int gf_stack_dup(gf_stack *stack)
{
    gf_buffer *buf;
    if (stack->pos == 0) {
	return GF_NOT_OK;
    }

    if (stack->pos >= stack->size - 1) {
	return GF_NOT_OK;
    }

    stack->buffers[stack->pos] = stack->buffers[stack->pos - 1];
    buf = stack->buffers[stack->pos];
    gf_buffer_mark(buf);
    stack->pos++;
    return GF_OK;
}

int gf_stack_drop(gf_stack *stack)
{
    if (stack->pos == 0)
	return GF_NOT_OK;
    stack->pos--;
    return GF_OK;
}

int gf_stack_swap(gf_stack *stack)
{
    gf_buffer *tmp;
    if (stack->pos < 2)
	return GF_NOT_OK;
    tmp = stack->buffers[stack->pos - 1];
    stack->buffers[stack->pos - 1] = stack->buffers[stack->pos - 2];
    stack->buffers[stack->pos - 2] = tmp;

    return GF_OK;
}

int gf_stack_hold(gf_stack *stack, gf_buffer ** buf)
{
    if (pop_from_stack(stack, buf) != GF_OK) {
	return GF_NOT_OK;
    }
    gf_buffer_hold(*buf);
    return GF_OK;
}


int gf_stack_size(gf_stack *stack)
{
    return stack->size;
}

int gf_stack_pos(gf_stack *stack)
{
    return stack->pos;
}

void gf_stack_reset(gf_stack *stack)
{
    stack->pos = 0;
}

void gf_patch_init(gf_patch *patch, int blksize)
{
    patch->blksize = blksize;
    gf_bufferpool_init(&patch->pool);
    gf_patch_srate_set(patch, 44100);
    gf_memory_defaults(patch);
    gf_print_init(patch);
    gf_patch_reinit(patch);
}

void gf_patch_alloc(gf_patch *patch, int nbuffers, int stack_size)
{
    gf_bufferpool_create(patch, &patch->pool, nbuffers, patch->blksize);
    gf_bufferpool_reset(&patch->pool);
    gf_stack_init(&patch->stack, &patch->pool);
    gf_stack_alloc(patch, &patch->stack, stack_size);
}

void gf_patch_realloc(gf_patch *patch,
		      int nbuffers, int stack_size, int blksize)
{
    gf_bufferpool_destroy(patch, &patch->pool);
    gf_stack_free(patch, &patch->stack);
    patch->blksize = blksize;
    gf_patch_alloc(patch, nbuffers, stack_size);
}

void gf_patch_reinit(gf_patch *patch)
{
    patch->counter = 0;
    patch->nodepos = 0;

    gf_patch_clear(patch);

    gf_stack_reset(&patch->stack);
    gf_bufferpool_reset(&patch->pool);

    patch->err = GF_OK;
}

void gf_patch_clear(gf_patch *patch)
{
    patch->nodes = NULL;
    patch->last = NULL;
    patch->nnodes = 0;
    gf_pointerlist_init(&patch->plist);
}

void gf_patch_free_nodes(gf_patch *patch)
{
    int i;
    gf_node *node;
    gf_node *next;
    node = patch->nodes;
    for (i = 0; i < patch->nnodes; i++) {
	next = gf_node_get_next(node);
	free(node);
	node = next;
    }

    patch->nnodes = 0;

}

void gf_patch_destroy(gf_patch *patch)
{
    int n;
    gf_node *node;
    gf_node *next;
    node = patch->nodes;
    for (n = 0; n < patch->nnodes; n++) {
	next = gf_node_get_next(node);
	gf_node_destroy(node);
	node = next;
    }
    gf_pointerlist_free(&patch->plist);
    gf_bufferpool_destroy(patch, &patch->pool);
    gf_stack_free(patch, &patch->stack);
}

void gf_patch_compute(gf_patch *patch)
{
    int n;
    gf_node *node;
    gf_node *next;
    node = patch->nodes;
    for (n = 0; n < patch->nnodes; n++) {
	next = gf_node_get_next(node);
	gf_node_compute(node);
	node = next;
    }
}

size_t gf_patch_size(void)
{
    return sizeof(gf_patch);
}

int gf_patch_new_node(gf_patch *patch, gf_node **node)
{
    gf_node *tmp;
    int rc;

    if (patch == NULL)
	return GF_NULL_VALUE;

    if (patch->err != GF_OK) return patch->err;

    rc = gf_memory_alloc(patch, sizeof(gf_node), (void **) &tmp);

    if (rc != GF_OK)
	return rc;

    gf_node_init(tmp, patch->blksize);
    gf_node_set_id(tmp, patch->nnodes);
    gf_node_set_patch(tmp, patch);

    if (patch->nnodes == 0) {
        patch->nodes = tmp;
    } else {
        gf_node_set_next(patch->last, tmp);
    }


    patch->nnodes++;
    patch->last = tmp;

    *node = tmp;

    return GF_OK;
}

static void delete_cable(gf_pointer *p)
{
    gf_cable *c;
    c = gf_pointer_data(p);
    gf_memory_free(p->patch, (void **) &c);
}

int gf_patch_new_cable(gf_patch *patch, gf_cable ** cable)
{
    gf_cable *tmp;
    int rc;
    rc = gf_memory_alloc(patch, sizeof(gf_cable), (void **) &tmp);
    if (rc != GF_OK)
	return rc;
    gf_cable_init(NULL, tmp);
    gf_patch_append_userdata(patch, delete_cable, tmp);
    *cable = tmp;
    return GF_OK;
}

int gf_patch_append_userdata(gf_patch *patch,
			     gf_pointer_function dfun, void *ud)
{
    gf_pointer *ptr;
    int rc;

    rc = gf_pointer_create(patch, &ptr, dfun, ud);
    if (rc != GF_OK)
	return rc;

    gf_pointerlist_append(&patch->plist, ptr);
    return GF_OK;
}

gf_stack *gf_patch_stack(gf_patch *patch)
{
    return &patch->stack;
}

int gf_patch_blksize(gf_patch *patch)
{
    return patch->blksize;
}

int gf_patch_blksize_set(gf_patch *patch, int blksize)
{
    int maxsize;

    maxsize = 64; /* TODO: don't hardcode */

    if (blksize <= 0 || blksize > maxsize) {
        return GF_NOT_OK;
    }

    patch->blksize = blksize;

    return GF_OK;
}

gf_bufferpool *gf_patch_pool(gf_patch *patch)
{
    return &patch->pool;
}

void gf_patch_srate_set(gf_patch *patch, int sr)
{
    patch->sr = sr;
}

int gf_patch_srate_get(gf_patch *patch)
{
    return patch->sr;
}

void gf_patch_data_set(gf_patch *patch, void *ud)
{
    patch->ud = ud;
}

void *gf_patch_data_get(gf_patch *patch)
{
    return patch->ud;
}

void gf_patch_holdbuf(gf_patch *patch, gf_cable *c)
{
    gf_bufferpool *pool;
    gf_buffer *buf;

    if (gf_cable_is_constant(c))
	return;

    pool = gf_patch_pool(patch);
    buf = gf_cable_get_buffer(c->pcable);
    gf_bufferpool_holdu(pool, buf);
    gf_bufferpool_clear_last_free(pool);
}

void gf_patch_unholdbuf(gf_patch *patch, gf_cable *c)
{
    gf_bufferpool *pool;
    gf_buffer *buf;

    if (gf_cable_is_constant(c))
	return;
    pool = gf_patch_pool(patch);
    buf = gf_cable_get_buffer(c->pcable);
    gf_bufferpool_unholdu(pool, buf);
}

int gf_patch_bhold(gf_patch *patch, gf_buffer ** b)
{
    int rc;
    gf_stack *stack;
    gf_buffer *buf;
    gf_bufferpool *pool;

    stack = gf_patch_stack(patch);
    pool = gf_patch_pool(patch);
    rc = gf_stack_pop(stack, &buf);

    if (rc != GF_OK)
	return rc;

    gf_bufferpool_clear_last_free(pool);
    gf_bufferpool_holdu(pool, buf);

    if (b != NULL)
	*b = buf;

    return GF_OK;
}

int gf_patch_bunhold(gf_patch *patch, gf_buffer *b)
{
    gf_bufferpool *pool;
    int rc;

    pool = gf_patch_pool(patch);
    rc = gf_bufferpool_unholdu(pool, b);
    if (rc != GF_OK)
	return rc;

    return GF_OK;
}

void gf_patch_err(gf_patch *patch, int rc)
{
    patch->err = rc;
}

gf_node *gf_patch_last_node(gf_patch *patch)
{
    return patch->last;
}

void gf_print(gf_patch *p, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    p->print(p, fmt, args);
    va_end(args);
}

static void gf_print_default(gf_patch *p, const char *str, va_list args)
{
    vprintf(str, args);
}

void gf_print_init(gf_patch *p)
{
    p->print = gf_print_default;
}

void gf_print_set(gf_patch *p,
		  void (*print)(gf_patch *, const char *, va_list))
{
    p->print = print;
}

void gf_subpatch_init(gf_subpatch *subpatch)
{
    subpatch->nnodes = 0;
    gf_pointerlist_init(&subpatch->plist);
}

void gf_subpatch_save(gf_patch *patch, gf_subpatch *subpatch)
{
    subpatch->nodes = patch->nodes;
    subpatch->last = patch->last;
    subpatch->nnodes = patch->nnodes;
    subpatch->plist = patch->plist;
}

void gf_subpatch_restore(gf_patch *patch, gf_subpatch *subpatch)
{
    patch->nodes = subpatch->nodes;
    patch->last = subpatch->last;
    patch->nnodes = subpatch->nnodes;
    patch->plist = subpatch->plist;
}

void gf_subpatch_compute(gf_subpatch *subpatch)
{
    int n;
    gf_node *node;
    gf_node *next;
    node = subpatch->nodes;
    for (n = 0; n < subpatch->nnodes; n++) {
        next = gf_node_get_next(node);
        gf_node_compute(node);
        node = next;
    }
}

void gf_subpatch_destroy(gf_subpatch *subpatch)
{
    int n;
    gf_node *node;
    gf_node *next;
    node = subpatch->nodes;
    for (n = 0; n < subpatch->nnodes; n++) {
        next = gf_node_get_next(node);
        gf_node_destroy(node);
        node = next;
    }
}

void gf_subpatch_free(gf_subpatch *subpatch)
{
    int n;
    gf_node *node;
    gf_node *next;
    node = subpatch->nodes;
    for (n = 0; n < subpatch->nnodes; n++) {
        next = gf_node_get_next(node);
        free(node);
        node = next;
    }
    subpatch->nnodes = 0;
    gf_pointerlist_free(&subpatch->plist);
    gf_pointerlist_init(&subpatch->plist);
}

int gf_memory_alloc(gf_patch *p, size_t size, void **ud)
{
    void *ptr;

    ptr = malloc(size);

    if (ptr == NULL)
	return GF_NOT_OK;

    *ud = ptr;
    return GF_OK;
}

int gf_memory_free(gf_patch *p, void **ud)
{
    free(*ud);
    return GF_OK;
}

void gf_memory_override(gf_patch *p, gf_mallocfun m, gf_freefun f)
{
    p->malloc = m;
    p->free = f;
}

static int default_malloc(gf_patch *p, size_t size, void **ud)
{
    void *ptr;

    ptr = malloc(size);

    if (ptr == NULL)
	return GF_NOT_OK;

    *ud = ptr;
    return GF_OK;
}

static int default_free(gf_patch *p, void **ud)
{
    free(*ud);
    return GF_OK;
}

void gf_memory_defaults(gf_patch *p)
{
    gf_memory_override(p, default_malloc, default_free);
}
