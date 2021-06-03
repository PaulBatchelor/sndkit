/*5:*/
#line 4 "./header.w"

#ifndef PATCHWERK_H
#define PATCHWERK_H
#include <stdio.h> 
#include <stdarg.h>  
/*6:*/
#line 23 "./header.w"

#ifndef PWFLT
#define PWFLT float
#endif

typedef struct pw_node pw_node;
typedef struct pw_pointer pw_pointer;
typedef void(*pw_function)(pw_node*);
typedef void(*pw_nodefun)(pw_node*,void*);
/*56:*/
#line 22 "./cable.w"

typedef struct pw_cable pw_cable;

/*:56*//*117:*/
#line 18 "./pointer.w"

typedef void(*pw_pointer_function)(pw_pointer*p);

/*:117*//*123:*/
#line 54 "./pointer.w"

typedef struct{
/*124:*/
#line 62 "./pointer.w"

pw_pointer*root;
pw_pointer*last;

/*:124*//*125:*/
#line 68 "./pointer.w"

unsigned int size;

/*:125*/
#line 56 "./pointer.w"

}pw_pointerlist;

/*:123*//*151:*/
#line 19 "./pool.w"

typedef struct pw_buffer pw_buffer;
typedef struct pw_bufferpool pw_bufferpool;

/*:151*//*220:*/
#line 22 "./stack.w"
typedef struct pw_stack pw_stack;

/*:220*//*251:*/
#line 18 "./patch.w"

typedef struct pw_patch pw_patch;

/*:251*//*314:*/
#line 32 "./subpatch.w"

typedef struct{
pw_node*nodes;
pw_node*last;
int nnodes;
pw_cable*out;
pw_pointerlist plist;
}pw_subpatch;

/*:314*//*334:*/
#line 14 "./egraph.w"

typedef struct pw_evnode pw_evnode;

/*:334*//*365:*/
#line 262 "./egraph.w"

typedef struct{
/*366:*/
#line 284 "./egraph.w"

pw_evnode*node;
int count;
int terminal;

/*:366*/
#line 264 "./egraph.w"

}pw_evwalker;

/*:365*//*395:*/
#line 55 "./memory.w"

typedef int(*pw_mallocfun)(pw_patch*,size_t,void**);
typedef int(*pw_freefun)(pw_patch*,void**);

/*:395*/
#line 32 "./header.w"


/*:6*//*7:*/
#line 35 "./header.w"

/*55:*/
#line 14 "./cable.w"

struct pw_cable{
/*57:*/
#line 27 "./cable.w"

pw_node*node;

/*:57*//*58:*/
#line 33 "./cable.w"

PWFLT*val;
PWFLT ival;

/*:58*//*59:*/
#line 44 "./cable.w"

PWFLT*blk;
int blksize;

/*:59*//*60:*/
#line 51 "./cable.w"

unsigned char type;

/*:60*//*62:*/
#line 70 "./cable.w"

pw_cable*pcable;

/*:62*//*63:*/
#line 80 "./cable.w"

pw_buffer*buf;

/*:63*/
#line 16 "./cable.w"

};

/*:55*/
#line 36 "./header.w"

#line 1 "./node.w"
/*:7*//*22:*/
#line 76 "./node.w"

size_t pw_node_size();

/*:22*//*24:*/
#line 92 "./node.w"

void pw_node_init(pw_node*node,int blksize);


/*:24*//*26:*/
#line 116 "./node.w"

int pw_node_get_id(pw_node*node);

/*:26*//*28:*/
#line 126 "./node.w"

void pw_node_set_id(pw_node*node,int id);

/*:28*//*30:*/
#line 138 "./node.w"

void pw_node_set_setup(pw_node*node,pw_function fun);
void pw_node_set_compute(pw_node*node,pw_function fun);
void pw_node_set_destroy(pw_node*node,pw_function fun);


/*:30*//*32:*/
#line 163 "./node.w"

void pw_node_setup(pw_node*node);
void pw_node_compute(pw_node*node);
void pw_node_destroy(pw_node*node);

/*:32*//*34:*/
#line 187 "./node.w"

void pw_node_set_data(pw_node*node,void*data);
void*pw_node_get_data(pw_node*node);

/*:34*//*36:*/
#line 204 "./node.w"

int pw_node_cables_alloc(pw_node*node,int ncables);

/*:36*//*38:*/
#line 227 "./node.w"

int pw_node_cables_free(pw_node*node);

/*:38*//*40:*/
#line 248 "./node.w"

int pw_node_get_cable(pw_node*node,int id,pw_cable**cable);

/*:40*//*42:*/
#line 264 "./node.w"

int pw_node_blksize(pw_node*node);

/*:42*//*44:*/
#line 277 "./node.w"

int pw_node_set_block(pw_node*node,int id);

/*:44*//*46:*/
#line 306 "./node.w"

int pw_node_get_ncables(pw_node*node);

/*:46*//*48:*/
#line 317 "./node.w"

int pw_node_get_type(pw_node*node);
void pw_node_set_type(pw_node*node,int type);

/*:48*//*50:*/
#line 336 "./node.w"

void pw_node_set_patch(pw_node*node,pw_patch*patch);
int pw_node_get_patch(pw_node*node,pw_patch**patch);

/*:50*//*52:*/
#line 354 "./node.w"

pw_node*pw_node_get_next(pw_node*node);
void pw_node_set_next(pw_node*node,pw_node*next);

/*:52*//*65:*/
#line 88 "./cable.w"

void pw_cable_init(pw_node*node,pw_cable*cable);

/*:65*//*67:*/
#line 108 "./cable.w"

void pw_cable_free(pw_cable*cable);

/*:67*//*70:*/
#line 130 "./cable.w"

void pw_cable_set_block(pw_cable*cable,PWFLT*blk,int blksize);

/*:70*//*71:*/
#line 134 "./cable.w"

void pw_cable_set_constant(pw_cable*cable,PWFLT val);

/*:71*//*73:*/
#line 150 "./cable.w"

void pw_cable_set_value(pw_cable*c,PWFLT val);

/*:73*//*75:*/
#line 163 "./cable.w"

PWFLT pw_cable_get(pw_cable*cable,int pos);

/*:75*//*77:*/
#line 176 "./cable.w"

void pw_cable_set(pw_cable*cable,int pos,PWFLT val);

/*:77*//*79:*/
#line 198 "./cable.w"

int pw_cable_connect(pw_cable*c1,pw_cable*c2);

/*:79*//*81:*/
#line 220 "./cable.w"

void pw_cable_connect_nocheck(pw_cable*c1,pw_cable*c2);


/*:81*//*84:*/
#line 255 "./cable.w"
int pw_cable_pop(pw_cable*cab);

/*:84*//*86:*/
#line 272 "./cable.w"
void pw_cable_push(pw_cable*cab);

/*:86*//*88:*/
#line 284 "./cable.w"

int pw_cable_is_block(pw_cable*cab);

/*:88*//*90:*/
#line 296 "./cable.w"

int pw_cable_is_constant(pw_cable*cab);

/*:90*//*92:*/
#line 316 "./cable.w"

pw_buffer*pw_cable_get_buffer(pw_cable*cab);
void pw_cable_set_buffer(pw_cable*cab,pw_buffer*buf);

/*:92*//*94:*/
#line 351 "./cable.w"

int pw_cable_make_block(pw_cable*cable,pw_stack*stack,int blksize);

/*:94*//*96:*/
#line 370 "./cable.w"

int pw_cable_clear(pw_cable*cab);

/*:96*//*98:*/
#line 389 "./cable.w"

int pw_cable_mix(pw_cable*in,pw_cable*sum,PWFLT mix);

/*:98*//*100:*/
#line 400 "./cable.w"

int pw_cable_blksize(pw_cable*cable);

/*:100*//*102:*/
#line 407 "./cable.w"

void pw_cable_override(pw_cable*c1,pw_cable*c2);

/*:102*//*105:*/
#line 427 "./cable.w"

void pw_cable_copy(pw_cable*c1,pw_cable*c2);

/*:105*//*108:*/
#line 7 "./error.w"

enum{
/*109:*/
#line 34 "./error.w"

PW_OK,
PW_NOT_OK,
PW_INVALID_CABLE,
PW_INVALID_NODE,
PW_INVALID_BUFFER,
PW_ALREADY_ALLOCATED,
PW_CONNECTION_MISMATCH,
PW_INVALID_ENTRY,
PW_NOT_ENOUGH_NODES,
PW_NULL_VALUE,
PW_I_DONT_KNOW,
PW_POOL_FULL,
PW_STACK_OVERFLOW

/*:109*/
#line 9 "./error.w"

};

/*:108*//*111:*/
#line 70 "./error.w"

const char*pw_error(int rc);

/*:111*//*113:*/
#line 86 "./error.w"

#define PW_ERROR_CHECK(rc) if(rc != PW_OK) return rc
#line 1 "./pointer.w"
/*:113*//*128:*/
#line 76 "./pointer.w"

int pw_pointer_create(
pw_patch*patch,
pw_pointer**pointer,
pw_pointer_function free,
void*ud
);

/*:128*//*130:*/
#line 112 "./pointer.w"

void pw_pointer_destroy(pw_pointer**pointer);

/*:130*//*132:*/
#line 124 "./pointer.w"

void*pw_pointer_data(pw_pointer*pointer);

/*:132*//*134:*/
#line 135 "./pointer.w"

void pw_pointer_set_type(pw_pointer*pointer,int id);
int pw_pointer_get_type(pw_pointer*pointer);

/*:134*//*136:*/
#line 153 "./pointer.w"

pw_pointer*pw_pointer_get_next(pw_pointer*p);

/*:136*//*138:*/
#line 163 "./pointer.w"

void pw_pointer_set_next(pw_pointer*p,pw_pointer*next);

/*:138*//*141:*/
#line 177 "./pointer.w"

void pw_pointerlist_init(pw_pointerlist*plist);

/*:141*//*143:*/
#line 189 "./pointer.w"

pw_pointer*pw_pointerlist_top(pw_pointerlist*plist);

/*:143*//*145:*/
#line 202 "./pointer.w"

void pw_pointerlist_append(pw_pointerlist*plist,pw_pointer*p);

/*:145*//*147:*/
#line 227 "./pointer.w"

void pw_pointerlist_free(pw_pointerlist*plist);

/*:147*//*162:*/
#line 82 "./pool.w"

size_t pw_buffer_size(void);


/*:162*//*164:*/
#line 93 "./pool.w"
void pw_buffer_alloc(pw_patch*patch,pw_buffer*buf,int size);

/*:164*//*167:*/
#line 102 "./pool.w"
void pw_buffer_free(pw_patch*patch,pw_buffer*buf);

/*:167*//*169:*/
#line 113 "./pool.w"
void pw_buffer_init(pw_buffer*buf);

/*:169*//*171:*/
#line 123 "./pool.w"
void pw_buffer_reinit(pw_buffer*buf);

/*:171*//*173:*/
#line 135 "./pool.w"
void pw_buffer_mark(pw_buffer*buf);

/*:173*//*175:*/
#line 160 "./pool.w"
int pw_buffer_unmark(pw_buffer*buf);

/*:175*//*177:*/
#line 169 "./pool.w"
PWFLT*pw_buffer_data(pw_buffer*buf);

/*:177*//*178:*/
#line 175 "./pool.w"
void pw_buffer_hold(pw_buffer*buf);

/*:178*//*180:*/
#line 191 "./pool.w"
void pw_buffer_holdu(pw_buffer*buf);

/*:180*//*183:*/
#line 211 "./pool.w"
int pw_buffer_unhold(pw_buffer*buf);

/*:183*//*185:*/
#line 221 "./pool.w"

int pw_buffer_id(pw_buffer*buf);


/*:185*//*187:*/
#line 235 "./pool.w"

void pw_bufferpool_init(pw_bufferpool*pool);


/*:187*//*189:*/
#line 259 "./pool.w"

void pw_bufferpool_create(pw_patch*patch,
pw_bufferpool*pool,int nbuf,int blksize);

/*:189*//*191:*/
#line 277 "./pool.w"

void pw_bufferpool_reset(pw_bufferpool*pool);

/*:191*//*193:*/
#line 295 "./pool.w"

void pw_bufferpool_destroy(pw_patch*patch,pw_bufferpool*pool);

/*:193*//*195:*/
#line 306 "./pool.w"

int pw_bufferpool_nactive(pw_bufferpool*pool);

/*:195*//*197:*/
#line 335 "./pool.w"

int pw_bufferpool_unhold(pw_bufferpool*pool,pw_buffer*buf);

/*:197*//*198:*/
#line 343 "./pool.w"

int pw_bufferpool_nextfree(pw_bufferpool*pool,pw_buffer**buf);

/*:198*//*208:*/
#line 449 "./pool.w"

int pw_bufferpool_holdu(pw_bufferpool*pool,pw_buffer*buf);

/*:208*//*210:*/
#line 474 "./pool.w"

int pw_bufferpool_unholdu(pw_bufferpool*pool,pw_buffer*buf);

/*:210*//*212:*/
#line 500 "./pool.w"

int pw_bufferpool_unholdu_all(pw_bufferpool*pool);

/*:212*//*214:*/
#line 512 "./pool.w"

int pw_bufferpool_uactive(pw_bufferpool*pool);

/*:214*//*216:*/
#line 528 "./pool.w"

void pw_bufferpool_clear_last_free(pw_bufferpool*pool);
#line 1 "./stack.w"
/*:216*//*223:*/
#line 40 "./stack.w"
void pw_stack_init(pw_stack*stack,pw_bufferpool*pool);

/*:223*//*225:*/
#line 55 "./stack.w"
int pw_stack_alloc(pw_patch*patch,pw_stack*stack,int size);

/*:225*//*227:*/
#line 65 "./stack.w"
int pw_stack_free(pw_patch*patch,pw_stack*stack);

/*:227*//*229:*/
#line 100 "./stack.w"
int pw_stack_push(pw_stack*stack,pw_buffer**buf);

/*:229*//*231:*/
#line 118 "./stack.w"
int pw_stack_push_buffer(pw_stack*stack,pw_buffer*buf);

/*:231*//*233:*/
#line 162 "./stack.w"
int pw_stack_pop(pw_stack*stack,pw_buffer**buf);

/*:233*//*235:*/
#line 192 "./stack.w"
int pw_stack_dup(pw_stack*stack);

/*:235*//*237:*/
#line 205 "./stack.w"
int pw_stack_drop(pw_stack*stack);

/*:237*//*239:*/
#line 222 "./stack.w"
int pw_stack_swap(pw_stack*stack);

/*:239*//*241:*/
#line 241 "./stack.w"
int pw_stack_hold(pw_stack*stack,pw_buffer**buf);

/*:241*//*243:*/
#line 248 "./stack.w"
int pw_stack_size(pw_stack*stack);

/*:243*//*246:*/
#line 258 "./stack.w"
int pw_stack_pos(pw_stack*stack);

/*:246*//*247:*/
#line 263 "./stack.w"

void pw_stack_reset(pw_stack*stack);

/*:247*//*264:*/
#line 93 "./patch.w"

void pw_patch_init(pw_patch*patch,int blksize);

/*:264*//*266:*/
#line 118 "./patch.w"

void pw_patch_alloc(pw_patch*patch,int nbuffers,int stack_size);

/*:266*//*268:*/
#line 134 "./patch.w"

void pw_patch_realloc(pw_patch*patch,
int nbuffers,
int stack_size,
int blksize);

/*:268*//*270:*/
#line 160 "./patch.w"

void pw_patch_reinit(pw_patch*patch);

/*:270*//*272:*/
#line 181 "./patch.w"

void pw_patch_clear(pw_patch*patch);

/*:272*//*274:*/
#line 207 "./patch.w"

void pw_patch_free_nodes(pw_patch*patch);

/*:274*//*276:*/
#line 229 "./patch.w"

void pw_patch_setup(pw_patch*patch);
void pw_patch_destroy(pw_patch*patch);
void pw_patch_compute(pw_patch*patch);

/*:276*//*278:*/
#line 278 "./patch.w"

void pw_patch_set_out(pw_patch*patch,pw_cable*cable);

/*:278*//*280:*/
#line 288 "./patch.w"

pw_cable*pw_patch_get_out(pw_patch*patch);

/*:280*//*282:*/
#line 298 "./patch.w"

size_t pw_patch_size(void);

/*:282*//*285:*/
#line 314 "./patch.w"

PWFLT pw_patch_tick(pw_patch*patch);

/*:285*//*287:*/
#line 336 "./patch.w"

int pw_patch_new_node(pw_patch*patch,pw_node**node);

/*:287*//*289:*/
#line 375 "./patch.w"

int pw_patch_new_cable(pw_patch*patch,pw_cable**cable);

/*:289*//*292:*/
#line 401 "./patch.w"

int pw_patch_append_userdata(
pw_patch*patch,
pw_pointer_function dfun,
void*ud
);

/*:292*//*294:*/
#line 425 "./patch.w"
pw_stack*pw_patch_stack(pw_patch*patch);

/*:294*//*296:*/
#line 436 "./patch.w"

int pw_patch_blksize(pw_patch*patch);

/*:296*//*298:*/
#line 448 "./patch.w"

pw_bufferpool*pw_patch_pool(pw_patch*patch);
/*:298*//*300:*/
#line 461 "./patch.w"

void pw_patch_srate_set(pw_patch*patch,int sr);
int pw_patch_srate_get(pw_patch*patch);

/*:300*//*302:*/
#line 479 "./patch.w"

void pw_patch_data_set(pw_patch*patch,void*ud);
void*pw_patch_data_get(pw_patch*patch);

/*:302*//*304:*/
#line 498 "./patch.w"

void pw_patch_holdbuf(pw_patch*patch,pw_cable*c);
void pw_patch_unholdbuf(pw_patch*patch,pw_cable*c);

/*:304*//*306:*/
#line 550 "./patch.w"

int pw_patch_bhold(pw_patch*patch,pw_buffer**b);
int pw_patch_bunhold(pw_patch*patch,pw_buffer*b);

/*:306*//*308:*/
#line 597 "./patch.w"

void pw_patch_err(pw_patch*patch,int rc);

/*:308*//*310:*/
#line 611 "./patch.w"

pw_node*pw_patch_last_node(pw_patch*patch);

/*:310*//*317:*/
#line 45 "./subpatch.w"

void pw_subpatch_init(pw_subpatch*subpatch);

/*:317*//*320:*/
#line 61 "./subpatch.w"

void pw_subpatch_save(pw_patch*patch,pw_subpatch*subpatch);
/*:320*//*323:*/
#line 76 "./subpatch.w"

void pw_subpatch_restore(pw_patch*patch,pw_subpatch*subpatch);

/*:323*//*325:*/
#line 91 "./subpatch.w"

void pw_subpatch_compute(pw_subpatch*subpatch);

/*:325*//*327:*/
#line 110 "./subpatch.w"

void pw_subpatch_destroy(pw_subpatch*subpatch);

/*:327*//*329:*/
#line 130 "./subpatch.w"

void pw_subpatch_free(pw_subpatch*subpatch);

/*:329*//*331:*/
#line 152 "./subpatch.w"

pw_cable*pw_subpatch_out(pw_subpatch*subpatch);

/*:331*//*335:*/
#line 21 "./egraph.w"

typedef pw_evnode*(*pw_evnode_cb)(pw_evnode*,int);

/*:335*//*339:*/
#line 69 "./egraph.w"

int pw_evnode_new(pw_patch*patch,pw_evnode**evn);
int pw_evnode_bye(pw_patch*patch,pw_evnode**evn);

/*:339*//*341:*/
#line 97 "./egraph.w"

void pw_evnode_init(pw_evnode*evn);

/*:341*//*344:*/
#line 123 "./egraph.w"

pw_evnode*pw_evnode_fire(pw_evnode*evn,int pos);

/*:344*//*346:*/
#line 133 "./egraph.w"

void pw_evnode_callback(pw_evnode*evn,pw_evnode_cb cb);

/*:346*//*348:*/
#line 144 "./egraph.w"

void pw_evnode_clean(pw_evnode*evn);

/*:348*//*350:*/
#line 154 "./egraph.w"

void pw_evnode_clean_set(pw_evnode*evn,void(*cb)(pw_evnode*));

/*:350*//*352:*/
#line 166 "./egraph.w"

void pw_evnode_data_set(pw_evnode*evn,void*ud);
void*pw_evnode_data_get(pw_evnode*evn);

/*:352*//*354:*/
#line 193 "./egraph.w"

pw_evnode*pw_evnode_next(pw_evnode*evn);

/*:354*//*356:*/
#line 205 "./egraph.w"

void pw_evnode_dur(pw_evnode*evn,int dur);

/*:356*//*358:*/
#line 215 "./egraph.w"

int pw_evnode_dur_get(pw_evnode*evn);

/*:358*//*360:*/
#line 227 "./egraph.w"

int pw_evnode_is_terminal(pw_evnode*evn);

/*:360*//*362:*/
#line 240 "./egraph.w"

void pw_evnode_set_next(pw_evnode*evn,pw_evnode*next);

/*:362*//*368:*/
#line 292 "./egraph.w"

void pw_evwalker_reset(pw_evwalker*walker);

/*:368*//*370:*/
#line 311 "./egraph.w"

void pw_evwalker_init(pw_evwalker*walker,pw_evnode*start,int delay);


/*:370*//*372:*/
#line 355 "./egraph.w"

int pw_evwalker_walk(pw_evwalker*walker,int pos);


/*:372*//*375:*/
#line 13 "./dump.w"

void pw_dump_cable(FILE*fp,pw_cable*c);

/*:375*//*377:*/
#line 49 "./dump.w"

void pw_dump_node(FILE*fp,pw_node*n,int print_cables);
/*:377*//*379:*/
#line 76 "./dump.w"

void pw_dump_nodes(FILE*fp,pw_node*nodes,int nnodes,int print_cables);
/*:379*//*381:*/
#line 102 "./dump.w"

void pw_dump_nodelist(FILE*fp,pw_patch*p,int print_cables);
/*:381*//*383:*/
#line 112 "./dump.w"

void pw_dump_subpatch(FILE*fp,pw_subpatch*s,int print_cables);

/*:383*//*385:*/
#line 122 "./dump.w"

void pw_dump_bufferpool(FILE*fp,pw_bufferpool*bp,int print_buffers);
/*:385*//*387:*/
#line 150 "./dump.w"

void pw_dump_stack(FILE*fp,pw_stack*s,int print_buffers);
/*:387*//*389:*/
#line 173 "./dump.w"

void pw_dump_buffer(FILE*fp,pw_buffer*b);
/*:389*//*392:*/
#line 22 "./memory.w"

int pw_memory_alloc(pw_patch*p,size_t size,void**ud);
int pw_memory_free(pw_patch*p,void**ud);

/*:392*//*396:*/
#line 62 "./memory.w"

void pw_memory_override(pw_patch*p,pw_mallocfun m,pw_freefun f);

/*:396*//*398:*/
#line 77 "./memory.w"

void pw_memory_defaults(pw_patch*p);

/*:398*//*401:*/
#line 17 "./print.w"

void pw_print(pw_patch*p,const char*fmt,...);

/*:401*//*404:*/
#line 41 "./print.w"

void pw_print_init(pw_patch*p);

/*:404*//*407:*/
#line 65 "./print.w"

void pw_print_set(pw_patch*p,void(*print)(pw_patch*,const char*,va_list));

/*:407*/
#line 9 "./header.w"


#ifdef __plan9__
#pragma incomplete pw_buffer
#pragma incomplete pw_patch
#pragma incomplete pw_node
#pragma incomplete pw_pointer
#pragma incomplete pw_stack
#endif
#endif

/*:5*/
