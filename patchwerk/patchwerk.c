/*4:*/
#line 117 "./patchwerk.w"

#include <stdlib.h> 
#include <stdio.h> 
#include "patchwerk.h"
/*8:*/
#line 5 "./node.w"

/*9:*/
#line 11 "./node.w"

struct pw_node{
/*10:*/
#line 19 "./node.w"

pw_patch*patch;

/*:10*//*11:*/
#line 26 "./node.w"

int id;

/*:11*//*12:*/
#line 30 "./node.w"

pw_function setup;

/*:12*//*13:*/
#line 35 "./node.w"

pw_function destroy;

/*:13*//*14:*/
#line 39 "./node.w"

pw_function compute;

/*:14*//*15:*/
#line 44 "./node.w"

void*ud;

/*:15*//*16:*/
#line 48 "./node.w"

pw_cable*cables;
int ncables;

/*:16*//*17:*/
#line 53 "./node.w"

int blksize;

/*:17*//*18:*/
#line 58 "./node.w"

int type;

/*:18*//*19:*/
#line 64 "./node.w"

int group;

/*:19*//*20:*/
#line 69 "./node.w"

pw_node*next;

/*:20*/
#line 13 "./node.w"

};

/*:9*//*23:*/
#line 79 "./node.w"

size_t pw_node_size()
{
return sizeof(pw_node);
}

/*:23*//*25:*/
#line 96 "./node.w"

static void empty(pw_node*node){}
static void free_cables(pw_node*node)
{
pw_node_cables_free(node);
}
void pw_node_init(pw_node*node,int blksize)
{
node->id= -1;
node->setup= empty;
node->compute= empty;
node->destroy= free_cables;
node->ncables= 0;
node->blksize= blksize;
node->type= -1;
node->group= -1;
node->next= NULL;
}

/*:25*//*27:*/
#line 119 "./node.w"

int pw_node_get_id(pw_node*node)
{
return node->id;
}

/*:27*//*29:*/
#line 129 "./node.w"

void pw_node_set_id(pw_node*node,int id)
{
node->id= id;
}

/*:29*//*31:*/
#line 144 "./node.w"

void pw_node_set_setup(pw_node*node,pw_function fun)
{
node->setup= fun;
}

void pw_node_set_compute(pw_node*node,pw_function fun)
{
node->compute= fun;
}

void pw_node_set_destroy(pw_node*node,pw_function fun)
{
node->destroy= fun;
}

/*:31*//*33:*/
#line 168 "./node.w"

void pw_node_setup(pw_node*node)
{
node->setup(node);
}

void pw_node_compute(pw_node*node)
{
node->compute(node);
}

void pw_node_destroy(pw_node*node)
{
node->destroy(node);
}

/*:33*//*35:*/
#line 191 "./node.w"

void pw_node_set_data(pw_node*node,void*data)
{
node->ud= data;
}

void*pw_node_get_data(pw_node*node)
{
return node->ud;
}

/*:35*//*37:*/
#line 207 "./node.w"

int pw_node_cables_alloc(pw_node*node,int ncables)
{
int n;
int rc;
if(node->patch==NULL)return PW_NOT_OK;
rc= pw_memory_alloc(node->patch,
sizeof(pw_cable)*ncables,
(void**)&node->cables);
if(rc!=PW_OK)return rc;
node->ncables= ncables;
for(n= 0;n<ncables;n++){
pw_cable_init(node,&node->cables[n]);
}

return PW_OK;
}

/*:37*//*39:*/
#line 230 "./node.w"

int pw_node_cables_free(pw_node*node)
{
int n;
if(node->patch==NULL)return PW_NOT_OK;
if(node->ncables> 0){
for(n= 0;n<node->ncables;n++){
pw_cable_free(&node->cables[n]);
}
return pw_memory_free(node->patch,(void**)&node->cables);
}

return PW_NOT_OK;
}

/*:39*//*41:*/
#line 251 "./node.w"

int pw_node_get_cable(pw_node*node,int id,pw_cable**cable)
{
if(id>=node->ncables){
return PW_INVALID_CABLE;
}

*cable= &node->cables[id];

return PW_OK;
}

/*:41*//*43:*/
#line 267 "./node.w"

int pw_node_blksize(pw_node*node)
{
return node->blksize;
}

/*:43*//*45:*/
#line 285 "./node.w"

int pw_node_set_block(pw_node*node,int id)
{
int rc;
if(id> node->ncables){
return PW_INVALID_CABLE;
}

rc= pw_cable_make_block(
&node->cables[id],
pw_patch_stack(node->patch),
node->blksize);

if(rc!=PW_OK){
pw_patch_err(node->patch,PW_POOL_FULL);
}

return rc;
}

/*:45*//*47:*/
#line 309 "./node.w"

int pw_node_get_ncables(pw_node*node)
{
return node->ncables;
}

/*:47*//*49:*/
#line 321 "./node.w"

int pw_node_get_type(pw_node*node)
{
return node->type;
}

void pw_node_set_type(pw_node*node,int type)
{
node->type= type;
}

/*:49*//*51:*/
#line 340 "./node.w"

void pw_node_set_patch(pw_node*node,pw_patch*patch)
{
node->patch= patch;
}

int pw_node_get_patch(pw_node*node,pw_patch**patch)
{
*patch= node->patch;
return PW_OK;
}

/*:51*//*53:*/
#line 358 "./node.w"

pw_node*pw_node_get_next(pw_node*node)
{
return node->next;
}

void pw_node_set_next(pw_node*node,pw_node*next)
{
node->next= next;
}
#line 1 "./cable.w"
/*:53*/
#line 6 "./node.w"


/*:8*//*54:*/
#line 7 "./cable.w"

/*61:*/
#line 61 "./cable.w"

enum{
CABLE_IVAL,
CABLE_BLOCK
};

/*:61*/
#line 8 "./cable.w"

/*66:*/
#line 91 "./cable.w"

void pw_cable_init(pw_node*node,pw_cable*cable)
{
cable->ival= 0;
cable->blksize= 0;
cable->blk= NULL;
cable->type= CABLE_IVAL;
cable->val= &cable->ival;
cable->node= node;
cable->pcable= cable;
cable->buf= NULL;
}

/*:66*//*68:*/
#line 111 "./cable.w"

void pw_cable_free(pw_cable*cable)
{
if(cable->blksize!=0){

}
}

/*:68*//*69:*/
#line 121 "./cable.w"

void pw_cable_set_block(pw_cable*cable,PWFLT*blk,int blksize)
{
cable->blk= blk;
cable->blksize= blksize;
cable->val= cable->blk;
cable->type= CABLE_BLOCK;
}

/*:69*//*72:*/
#line 137 "./cable.w"

void pw_cable_set_constant(pw_cable*cable,PWFLT val)
{
cable->val= &cable->ival;
cable->ival= val;
cable->type= CABLE_IVAL;
}

/*:72*//*74:*/
#line 153 "./cable.w"

void pw_cable_set_value(pw_cable*c,PWFLT val)
{
*c->val= val;
}

/*:74*//*76:*/
#line 166 "./cable.w"

PWFLT pw_cable_get(pw_cable*cable,int pos)
{
if(cable->type==CABLE_IVAL){
return*cable->val;
}else{
return cable->val[pos];
}
}

/*:76*//*78:*/
#line 179 "./cable.w"

void pw_cable_set(pw_cable*cable,int pos,PWFLT val)
{
if(cable->type==CABLE_IVAL){
*cable->val= val;
}else{
cable->val[pos]= val;
}
}


/*:78*//*80:*/
#line 201 "./cable.w"

int pw_cable_connect(pw_cable*c1,pw_cable*c2)
{
int id1,id2;

id1= pw_node_get_id(c1->node);
id2= pw_node_get_id(c2->node);

if(id1> id2)return PW_CONNECTION_MISMATCH;

pw_cable_connect_nocheck(c1,c2);

return PW_OK;
}

/*:80*//*82:*/
#line 224 "./cable.w"

void pw_cable_connect_nocheck(pw_cable*c1,pw_cable*c2)
{
c2->type= c1->type;
pw_cable_override(c1,c2);
}

/*:82*//*83:*/
#line 238 "./cable.w"


int pw_cable_pop(pw_cable*cab)
{
pw_stack*stack;
pw_node*node;
pw_buffer*tmp;

if(!pw_cable_is_block(cab))return PW_NOT_OK;

node= cab->node;
stack= pw_patch_stack(node->patch);
pw_stack_pop(stack,&tmp);

return PW_OK;
}

/*:83*//*85:*/
#line 258 "./cable.w"


void pw_cable_push(pw_cable*cab)
{
pw_stack*stack;
pw_node*node;
pw_buffer*tmp;
tmp= NULL;

node= cab->node;
stack= pw_patch_stack(node->patch);
pw_stack_push(stack,&tmp);
}

/*:85*//*87:*/
#line 278 "./cable.w"

int pw_cable_is_block(pw_cable*cab)
{
return cab->type==CABLE_BLOCK;
}

/*:87*//*89:*/
#line 290 "./cable.w"

int pw_cable_is_constant(pw_cable*cab)
{
return cab->type==CABLE_IVAL;
}

/*:89*//*91:*/
#line 305 "./cable.w"

pw_buffer*pw_cable_get_buffer(pw_cable*cab)
{
return cab->buf;
}

void pw_cable_set_buffer(pw_cable*cab,pw_buffer*buf)
{
cab->buf= buf;
}

/*:91*//*93:*/
#line 332 "./cable.w"

int pw_cable_make_block(pw_cable*cable,pw_stack*stack,int blksize)
{
pw_buffer*buf;
PWFLT*blk;

buf= NULL;

if(pw_stack_push(stack,&buf)!=PW_OK){
return PW_NOT_OK;
}

blk= pw_buffer_data(buf);
pw_cable_set_block(cable,blk,blksize);
pw_cable_set_buffer(cable,buf);

return PW_OK;
}

/*:93*//*95:*/
#line 357 "./cable.w"

int pw_cable_clear(pw_cable*cab)
{
int i;
if(!pw_cable_is_block(cab))return PW_NOT_OK;

for(i= 0;i<cab->blksize;i++){
cab->val[i]= 0;
}

return PW_OK;
}

/*:95*//*97:*/
#line 376 "./cable.w"

int pw_cable_mix(pw_cable*in,pw_cable*sum,PWFLT mix)
{
int i;
if(!pw_cable_is_block(in)||!pw_cable_is_block(sum))return PW_NOT_OK;

for(i= 0;i<sum->blksize;i++){
sum->val[i]+= in->val[i]*mix;
}

return PW_OK;
}

/*:97*//*99:*/
#line 393 "./cable.w"


int pw_cable_blksize(pw_cable*cable)
{
return cable->blksize;
}

/*:99*//*103:*/
#line 416 "./cable.w"

void pw_cable_override(pw_cable*c1,pw_cable*c2)
{
c2->val= c1->val;
c2->pcable= c1;
c2->blksize= c1->blksize;
}

/*:103*//*106:*/
#line 430 "./cable.w"

void pw_cable_copy(pw_cable*c1,pw_cable*c2)
{
int blksize;
int n;
PWFLT tmp;

blksize= c1->blksize;

for(n= 0;n<blksize;n++){
tmp= pw_cable_get(c1,n);
pw_cable_set(c2,n,tmp);
}
}
#line 1 "./error.w"
/*:106*/
#line 9 "./cable.w"


/*:54*//*110:*/
#line 51 "./error.w"

static const char*errmsg[]= {
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

/*:110*//*112:*/
#line 73 "./error.w"

const char*pw_error(int rc)
{
if(rc>=(sizeof(errmsg)/sizeof(*errmsg))||rc<0){
return errmsg[PW_I_DONT_KNOW];
}else{
return errmsg[rc];
}
}

/*:112*//*114:*/
#line 8 "./pointer.w"

/*118:*/
#line 22 "./pointer.w"

struct pw_pointer{
/*119:*/
#line 31 "./pointer.w"

pw_patch*patch;

/*:119*//*120:*/
#line 37 "./pointer.w"

int type;
void*ud;

/*:120*//*121:*/
#line 43 "./pointer.w"

pw_pointer_function free;

/*:121*//*122:*/
#line 48 "./pointer.w"

pw_pointer*next;

/*:122*/
#line 24 "./pointer.w"

};

/*:118*//*129:*/
#line 84 "./pointer.w"


int pw_pointer_create(
pw_patch*patch,
pw_pointer**pointer,
pw_pointer_function free,
void*ud
)
{
pw_pointer*pptr;
int rc;

rc= pw_memory_alloc(patch,sizeof(pw_pointer),(void**)&pptr);

if(rc!=PW_OK)return rc;

*pointer= pptr;
pptr->patch= patch;
pptr->next= NULL;
pptr->type= 0;
pptr->ud= ud;
pptr->free= free;
return PW_OK;
}

/*:129*//*131:*/
#line 115 "./pointer.w"

void pw_pointer_destroy(pw_pointer**pointer)
{
(*pointer)->free(*pointer);
pw_memory_free((*pointer)->patch,(void**)pointer);
}

/*:131*//*133:*/
#line 127 "./pointer.w"

void*pw_pointer_data(pw_pointer*pointer)
{
return pointer->ud;
}

/*:133*//*135:*/
#line 139 "./pointer.w"

void pw_pointer_set_type(pw_pointer*pointer,int id)
{
pointer->type= id;
}

int pw_pointer_get_id(pw_pointer*pointer)
{
return pointer->type;
}

/*:135*//*137:*/
#line 156 "./pointer.w"

pw_pointer*pw_pointer_get_next(pw_pointer*p)
{
return p->next;
}

/*:137*//*139:*/
#line 166 "./pointer.w"

void pw_pointer_set_next(pw_pointer*p,pw_pointer*next)
{
p->next= next;
}

/*:139*//*142:*/
#line 180 "./pointer.w"

void pw_pointerlist_init(pw_pointerlist*plist)
{
plist->root= NULL;
plist->last= NULL;
plist->size= 0;
}

/*:142*//*144:*/
#line 192 "./pointer.w"

pw_pointer*pw_pointerlist_top(pw_pointerlist*plist)
{
return plist->root;
}

/*:144*//*146:*/
#line 211 "./pointer.w"

void pw_pointerlist_append(pw_pointerlist*plist,pw_pointer*p)
{
if(plist->size==0){
plist->root= p;
}else{
pw_pointer_set_next(plist->last,p);
}

plist->last= p;
plist->size++;
}

/*:146*//*148:*/
#line 230 "./pointer.w"

void pw_pointerlist_free(pw_pointerlist*plist)
{
unsigned int i;
pw_pointer*next;
pw_pointer*val;

val= pw_pointerlist_top(plist);
for(i= 0;i<plist->size;i++){
next= pw_pointer_get_next(val);
pw_pointer_destroy(&val);
val= next;
}
}
#line 1 "./pool.w"
/*:148*/
#line 9 "./pointer.w"



/*:114*//*149:*/
#line 7 "./pool.w"

/*150:*/
#line 13 "./pool.w"

/*152:*/
#line 25 "./pool.w"

struct pw_buffer{
/*153:*/
#line 32 "./pool.w"

int id;

/*:153*//*154:*/
#line 39 "./pool.w"

int read;
PWFLT*buf;

/*:154*/
#line 27 "./pool.w"

};

/*:152*//*161:*/
#line 76 "./pool.w"

size_t pw_buffer_size(void)
{
return sizeof(pw_buffer);
}

/*:161*//*163:*/
#line 88 "./pool.w"

void pw_buffer_alloc(pw_patch*patch,pw_buffer*buf,int size)
{
pw_memory_alloc(patch,sizeof(PWFLT)*size,(void**)&buf->buf);
}
/*:163*//*166:*/
#line 97 "./pool.w"

void pw_buffer_free(pw_patch*patch,pw_buffer*buf)
{
pw_memory_free(patch,(void**)&buf->buf);
}
/*:166*//*168:*/
#line 106 "./pool.w"

void pw_buffer_init(pw_buffer*buf)
{
buf->id= -1;
pw_buffer_reinit(buf);
}

/*:168*//*170:*/
#line 117 "./pool.w"

void pw_buffer_reinit(pw_buffer*buf)
{
buf->read= 0;
}

/*:170*//*172:*/
#line 130 "./pool.w"

void pw_buffer_mark(pw_buffer*buf)
{
if(buf->read>=0)buf->read++;
}
/*:172*//*174:*/
#line 150 "./pool.w"

int pw_buffer_unmark(pw_buffer*buf)
{
if(buf->read<0)return-3;
if(buf->read==0)return-2;
buf->read--;
if(buf->read==0)return buf->id;
return-1;
}

/*:174*//*176:*/
#line 163 "./pool.w"

PWFLT*pw_buffer_data(pw_buffer*buf)
{
return buf->buf;
}

/*:176*//*179:*/
#line 181 "./pool.w"

void pw_buffer_hold(pw_buffer*buf)
{
buf->read= -1;
}

/*:179*//*181:*/
#line 194 "./pool.w"

void pw_buffer_holdu(pw_buffer*buf)
{
buf->read= -2;
}

/*:181*//*182:*/
#line 201 "./pool.w"

int pw_buffer_unhold(pw_buffer*buf)
{
if(buf->read<0){
buf->read= 0;
return 1;
}else{
return 0;
}
}
/*:182*//*184:*/
#line 215 "./pool.w"

int pw_buffer_id(pw_buffer*buf)
{
return buf->id;
}

/*:184*/
#line 14 "./pool.w"

/*155:*/
#line 44 "./pool.w"

struct pw_bufferpool{
/*156:*/
#line 50 "./pool.w"

pw_buffer*buffers;


/*:156*//*157:*/
#line 55 "./pool.w"

int size;

/*:157*//*158:*/
#line 60 "./pool.w"

int nactive;

/*:158*//*159:*/
#line 66 "./pool.w"

int last_free;

/*:159*//*206:*/
#line 428 "./pool.w"

int usrnactive;

/*:206*/
#line 46 "./pool.w"

};

/*:155*//*186:*/
#line 227 "./pool.w"

void pw_bufferpool_init(pw_bufferpool*pool)
{
pool->size= 0;
pool->nactive= 0;
pool->usrnactive= 0;
}

/*:186*//*188:*/
#line 242 "./pool.w"

void pw_bufferpool_create(pw_patch*patch,
pw_bufferpool*pool,int nbuf,int blksize)
{
int i;
pool->size= nbuf;
pool->nactive= 0;
pw_memory_alloc(patch,
sizeof(pw_buffer)*nbuf,
(void**)&pool->buffers);
for(i= 0;i<nbuf;i++){
pw_buffer_alloc(patch,&pool->buffers[i],blksize);
pw_buffer_init(&pool->buffers[i]);
pool->buffers[i].id= i;
}
}

/*:188*//*190:*/
#line 263 "./pool.w"

void pw_bufferpool_reset(pw_bufferpool*pool)
{
int i;
pool->last_free= -1;
pool->nactive= 0;
for(i= 0;i<pool->size;i++){
if(pool->buffers[i].read>=0){
pw_buffer_reinit(&pool->buffers[i]);
}else{
pool->nactive++;
}
}
}
/*:190*//*192:*/
#line 282 "./pool.w"

void pw_bufferpool_destroy(pw_patch*patch,pw_bufferpool*pool)
{
int i;
for(i= 0;i<pool->size;i++){
pw_buffer_free(patch,&pool->buffers[i]);
}

if(pool->size> 0){
pw_memory_free(patch,(void**)&pool->buffers);
}
}

/*:192*//*194:*/
#line 300 "./pool.w"

int pw_bufferpool_nactive(pw_bufferpool*pool)
{
return pool->nactive;
}

/*:194*//*196:*/
#line 323 "./pool.w"

int pw_bufferpool_unhold(pw_bufferpool*pool,pw_buffer*buf)
{
if(buf->id<0)return 0;
if(pw_buffer_unhold(buf)){
pool->nactive--;
return 1;
}else{
return 0;
}
}

/*:196*//*199:*/
#line 347 "./pool.w"

int pw_bufferpool_nextfree(pw_bufferpool*pool,pw_buffer**buf)
{
int i;
if(/*200:*/
#line 366 "./pool.w"

pool->nactive>=pool->size

/*:200*/
#line 351 "./pool.w"
){
return PW_POOL_FULL;
}else if(/*201:*/
#line 373 "./pool.w"

pool->last_free>=0

/*:201*/
#line 353 "./pool.w"
){
/*202:*/
#line 380 "./pool.w"

*buf= &pool->buffers[pool->last_free];
pool->last_free= -1;


/*:202*/
#line 354 "./pool.w"

}else{
/*203:*/
#line 388 "./pool.w"

for(i= 0;i<pool->size;i++){
if(pool->buffers[i].read==0){
*buf= &pool->buffers[i];
break;
}
}

/*:203*/
#line 356 "./pool.w"

}

/*204:*/
#line 399 "./pool.w"

pool->nactive++;
pw_buffer_mark(*buf);

/*:204*/
#line 359 "./pool.w"

return PW_OK;
}

/*:199*//*207:*/
#line 436 "./pool.w"

int pw_bufferpool_holdu(pw_bufferpool*pool,pw_buffer*buf)
{
if(buf==NULL)return PW_NULL_VALUE;
if(buf->read>=0){
pw_buffer_holdu(buf);
pool->usrnactive++;
pool->nactive++;
return PW_OK;
}
return PW_INVALID_BUFFER;
}

/*:207*//*209:*/
#line 462 "./pool.w"

int pw_bufferpool_unholdu(pw_bufferpool*pool,pw_buffer*buf)
{
if(buf==NULL)return PW_NULL_VALUE;
if(buf->id==-1)return PW_OK;
if(buf->read!=-2)return PW_INVALID_BUFFER;
if(!pw_buffer_unhold(buf))return PW_NOT_OK;
pool->nactive--;
pool->usrnactive--;
return PW_OK;
}

/*:209*//*211:*/
#line 488 "./pool.w"


int pw_bufferpool_unholdu_all(pw_bufferpool*pool)
{
int i;
if(pool->usrnactive==0)return PW_NOT_OK;
for(i= 0;i<pool->size;i++){
pw_bufferpool_unholdu(pool,&pool->buffers[i]);
}
return PW_OK;
}

/*:211*//*213:*/
#line 506 "./pool.w"

int pw_bufferpool_uactive(pw_bufferpool*pool)
{
return pool->usrnactive;
}

/*:213*//*215:*/
#line 522 "./pool.w"

void pw_bufferpool_clear_last_free(pw_bufferpool*pool)
{
pool->last_free= -1;
}

/*:215*/
#line 15 "./pool.w"


/*:150*/
#line 8 "./pool.w"


/*:149*//*217:*/
#line 6 "./stack.w"

/*218:*/
#line 10 "./stack.w"

/*219:*/
#line 14 "./stack.w"

struct pw_stack{
int pos;
int size;
pw_bufferpool*pool;
pw_buffer**buffers;
};

/*:219*/
#line 11 "./stack.w"


/*:218*//*222:*/
#line 32 "./stack.w"

void pw_stack_init(pw_stack*stack,pw_bufferpool*pool)
{
stack->pool= pool;
stack->pos= 0;
stack->size= 0;
}

/*:222*//*224:*/
#line 46 "./stack.w"

int pw_stack_alloc(pw_patch*patch,pw_stack*stack,int size)
{
stack->size= size;
return pw_memory_alloc(patch,
sizeof(pw_buffer*)*size,
(void**)&stack->buffers);
}

/*:224*//*226:*/
#line 59 "./stack.w"

int pw_stack_free(pw_patch*patch,pw_stack*stack)
{
return pw_memory_free(patch,(void**)&stack->buffers);
}

/*:226*//*228:*/
#line 78 "./stack.w"

int pw_stack_push(pw_stack*stack,pw_buffer**buf)
{
pw_buffer*pbuf;
int rc;

pbuf= NULL;

if(stack->pos>=stack->size){
return PW_STACK_OVERFLOW;
}

rc= pw_bufferpool_nextfree(stack->pool,&pbuf);

if(rc!=PW_OK)return rc;

stack->buffers[stack->pos]= pbuf;
stack->pos++;
if(buf!=NULL)*buf= pbuf;
return PW_OK;
}

/*:228*//*230:*/
#line 107 "./stack.w"

int pw_stack_push_buffer(pw_stack*stack,pw_buffer*buf)
{
if(stack->pos>=stack->size){
return PW_STACK_OVERFLOW;
}
stack->buffers[stack->pos]= buf;
stack->pos++;
return PW_OK;
}

/*:230*//*232:*/
#line 136 "./stack.w"


static int pop_from_stack(pw_stack*stack,pw_buffer**buf)
{
if(stack->pos==0)return PW_NOT_OK;
*buf= stack->buffers[stack->pos-1];
stack->pos--;
return PW_OK;
}

int pw_stack_pop(pw_stack*stack,pw_buffer**buf)
{
int rc;
pw_buffer*tmp;

rc= pop_from_stack(stack,&tmp);
if(rc!=PW_OK)return PW_NOT_OK;

rc= pw_buffer_unmark(tmp);
if(rc>=0){
stack->pool->last_free= rc;
stack->pool->nactive--;
}
if(buf!=NULL)*buf= tmp;
return PW_OK;
}
/*:232*//*234:*/
#line 174 "./stack.w"

int pw_stack_dup(pw_stack*stack)
{
pw_buffer*buf;
if(stack->pos==0){
return PW_NOT_OK;
}

if(stack->pos>=stack->size-1){
return PW_NOT_OK;
}

stack->buffers[stack->pos]= stack->buffers[stack->pos-1];
buf= stack->buffers[stack->pos];
pw_buffer_mark(buf);
stack->pos++;
return PW_OK;
}
/*:234*//*236:*/
#line 198 "./stack.w"

int pw_stack_drop(pw_stack*stack)
{
if(stack->pos==0)return PW_NOT_OK;
stack->pos--;
return PW_OK;
}
/*:236*//*238:*/
#line 211 "./stack.w"

int pw_stack_swap(pw_stack*stack)
{
pw_buffer*tmp;
if(stack->pos<2)return PW_NOT_OK;
tmp= stack->buffers[stack->pos-1];
stack->buffers[stack->pos-1]= stack->buffers[stack->pos-2];
stack->buffers[stack->pos-2]= tmp;

return PW_OK;
}
/*:238*//*240:*/
#line 231 "./stack.w"

int pw_stack_hold(pw_stack*stack,pw_buffer**buf)
{
if(pop_from_stack(stack,buf)!=PW_OK){
return PW_NOT_OK;
}
pw_buffer_hold(*buf);
return PW_OK;
}

/*:240*//*242:*/
#line 243 "./stack.w"

int pw_stack_size(pw_stack*stack)
{
return stack->size;
}
/*:242*//*244:*/
#line 250 "./stack.w"

int pw_stack_pos(pw_stack*stack)
{
return stack->pos;
}

/*:244*//*248:*/
#line 267 "./stack.w"

void pw_stack_reset(pw_stack*stack)
{
stack->pos= 0;
}
#line 1 "./patch.w"
/*:248*/
#line 7 "./stack.w"


/*:217*//*249:*/
#line 6 "./patch.w"

/*250:*/
#line 12 "./patch.w"

struct pw_patch{
/*252:*/
#line 27 "./patch.w"

pw_node*nodes;
pw_node*last;
int nnodes;

/*:252*//*253:*/
#line 34 "./patch.w"

pw_cable*out;
pw_cable zero;

/*:253*//*254:*/
#line 39 "./patch.w"

int blksize;

/*:254*//*255:*/
#line 46 "./patch.w"

int counter;

/*:255*//*256:*/
#line 52 "./patch.w"

int nodepos;

/*:256*//*257:*/
#line 57 "./patch.w"

pw_pointerlist plist;

/*:257*//*258:*/
#line 61 "./patch.w"

pw_bufferpool pool;

/*:258*//*259:*/
#line 66 "./patch.w"

pw_stack stack;

/*:259*//*260:*/
#line 72 "./patch.w"

int sr;

/*:260*//*261:*/
#line 78 "./patch.w"

void*ud;

/*:261*//*262:*/
#line 86 "./patch.w"

int err;

/*:262*//*394:*/
#line 49 "./memory.w"

pw_mallocfun malloc;
pw_freefun free;

/*:394*//*403:*/
#line 35 "./print.w"

void(*print)(pw_patch*,const char*fmt,va_list);

/*:403*/
#line 14 "./patch.w"

};

/*:250*/
#line 7 "./patch.w"

/*265:*/
#line 96 "./patch.w"

void pw_patch_init(pw_patch*patch,int blksize)
{
patch->blksize= blksize;
pw_bufferpool_init(&patch->pool);
pw_patch_srate_set(patch,44100);
pw_memory_defaults(patch);
pw_print_init(patch);
pw_patch_reinit(patch);
}

/*:265*//*267:*/
#line 121 "./patch.w"

void pw_patch_alloc(pw_patch*patch,int nbuffers,int stack_size)
{
pw_bufferpool_create(patch,&patch->pool,nbuffers,patch->blksize);
pw_bufferpool_reset(&patch->pool);

pw_stack_init(&patch->stack,&patch->pool);
pw_stack_alloc(patch,&patch->stack,stack_size);
}

/*:267*//*269:*/
#line 145 "./patch.w"

void pw_patch_realloc(pw_patch*patch,
int nbuffers,
int stack_size,
int blksize)
{
pw_bufferpool_destroy(patch,&patch->pool);
pw_stack_free(patch,&patch->stack);
patch->blksize= blksize;
pw_patch_alloc(patch,nbuffers,stack_size);
}

/*:269*//*271:*/
#line 163 "./patch.w"

void pw_patch_reinit(pw_patch*patch)
{
pw_cable_init(NULL,&patch->zero);
patch->counter= 0;
patch->nodepos= 0;

pw_patch_clear(patch);

pw_stack_reset(&patch->stack);
pw_bufferpool_reset(&patch->pool);

patch->err= PW_OK;
}

/*:271*//*273:*/
#line 193 "./patch.w"

void pw_patch_clear(pw_patch*patch)
{
patch->out= &patch->zero;
patch->nodes= NULL;
patch->last= NULL;
patch->nnodes= 0;
pw_pointerlist_init(&patch->plist);
}


/*:273*//*275:*/
#line 210 "./patch.w"

void pw_patch_free_nodes(pw_patch*patch)
{
int i;
pw_node*node;
pw_node*next;
node= patch->nodes;
for(i= 0;i<patch->nnodes;i++){
next= pw_node_get_next(node);
free(node);
node= next;
}

patch->nnodes= 0;

}

/*:275*//*277:*/
#line 234 "./patch.w"

void pw_patch_setup(pw_patch*patch)
{
int n;
pw_node*node;
pw_node*next;
node= patch->nodes;
for(n= 0;n<patch->nnodes;n++){
next= pw_node_get_next(node);
pw_node_setup(node);
node= next;
}
}

void pw_patch_destroy(pw_patch*patch)
{
int n;
pw_node*node;
pw_node*next;
node= patch->nodes;
for(n= 0;n<patch->nnodes;n++){
next= pw_node_get_next(node);
pw_node_destroy(node);
node= next;
}
pw_pointerlist_free(&patch->plist);
pw_bufferpool_destroy(patch,&patch->pool);
pw_stack_free(patch,&patch->stack);
}

void pw_patch_compute(pw_patch*patch)
{
int n;
pw_node*node;
pw_node*next;
node= patch->nodes;
for(n= 0;n<patch->nnodes;n++){
next= pw_node_get_next(node);
pw_node_compute(node);
node= next;
}
}

/*:277*//*279:*/
#line 281 "./patch.w"

void pw_patch_set_out(pw_patch*patch,pw_cable*cable)
{
patch->out= cable;
}

/*:279*//*281:*/
#line 291 "./patch.w"

pw_cable*pw_patch_get_out(pw_patch*patch)
{
return patch->out;
}

/*:281*//*283:*/
#line 301 "./patch.w"

size_t pw_patch_size(void)
{
return sizeof(pw_patch);
}

/*:283*//*286:*/
#line 317 "./patch.w"

PWFLT pw_patch_tick(pw_patch*patch)
{
PWFLT smp;
pw_cable*out;
if(patch->counter==0){
pw_patch_compute(patch);
}

out= pw_patch_get_out(patch);
smp= pw_cable_get(out,patch->counter);
patch->counter= (patch->counter+1)%patch->blksize;

return smp;
}

/*:286*//*288:*/
#line 339 "./patch.w"

int pw_patch_new_node(pw_patch*patch,pw_node**node)
{
pw_node*tmp;
int rc;

if(patch==NULL)return PW_NULL_VALUE;

if(patch->err!=PW_OK)return patch->err;

rc= pw_memory_alloc(patch,sizeof(pw_node),(void**)&tmp);

if(rc!=PW_OK)return rc;

pw_node_init(tmp,patch->blksize);
pw_node_set_id(tmp,patch->nnodes);
pw_node_set_patch(tmp,patch);

if(patch->nnodes==0){
patch->nodes= tmp;
}else{
pw_node_set_next(patch->last,tmp);
}


patch->nnodes++;
patch->last= tmp;

*node= tmp;

return PW_OK;
}

/*:288*//*290:*/
#line 378 "./patch.w"


static void delete_cable(pw_pointer*p)
{
pw_cable*c;
c= pw_pointer_data(p);
pw_memory_free(p->patch,(void**)&c);
}

int pw_patch_new_cable(pw_patch*patch,pw_cable**cable)
{
pw_cable*tmp;
int rc;
rc= pw_memory_alloc(patch,sizeof(pw_cable),(void**)&tmp);
if(rc!=PW_OK)return rc;
pw_cable_init(NULL,tmp);
pw_patch_append_userdata(patch,delete_cable,tmp);
*cable= tmp;
return PW_OK;
}

/*:290*//*293:*/
#line 408 "./patch.w"

int pw_patch_append_userdata(
pw_patch*patch,
pw_pointer_function dfun,
void*ud
)
{
pw_pointer*ptr;
int rc;

rc= pw_pointer_create(patch,&ptr,dfun,ud);
if(rc!=PW_OK)return rc;

pw_pointerlist_append(&patch->plist,ptr);
return PW_OK;
}

/*:293*//*295:*/
#line 429 "./patch.w"

pw_stack*pw_patch_stack(pw_patch*patch)
{
return&patch->stack;
}

/*:295*//*297:*/
#line 440 "./patch.w"


int pw_patch_blksize(pw_patch*patch)
{
return patch->blksize;
}

/*:297*//*299:*/
#line 451 "./patch.w"


pw_bufferpool*pw_patch_pool(pw_patch*patch)
{
return&patch->pool;
}

/*:299*//*301:*/
#line 465 "./patch.w"

void pw_patch_srate_set(pw_patch*patch,int sr)
{
patch->sr= sr;
}

int pw_patch_srate_get(pw_patch*patch)
{
return patch->sr;
}

/*:301*//*303:*/
#line 483 "./patch.w"

void pw_patch_data_set(pw_patch*patch,void*ud)
{
patch->ud= ud;
}

void*pw_patch_data_get(pw_patch*patch)
{
return patch->ud;
}

/*:303*//*305:*/
#line 511 "./patch.w"

void pw_patch_holdbuf(pw_patch*patch,pw_cable*c)
{
pw_bufferpool*pool;
pw_buffer*buf;

if(pw_cable_is_constant(c))return;

pool= pw_patch_pool(patch);
buf= pw_cable_get_buffer(c->pcable);
pw_bufferpool_holdu(pool,buf);
pw_bufferpool_clear_last_free(pool);
}

void pw_patch_unholdbuf(pw_patch*patch,pw_cable*c)
{
pw_bufferpool*pool;
pw_buffer*buf;

if(pw_cable_is_constant(c))return;
pool= pw_patch_pool(patch);
buf= pw_cable_get_buffer(c->pcable);
pw_bufferpool_unholdu(pool,buf);
}

/*:305*//*307:*/
#line 559 "./patch.w"

int pw_patch_bhold(pw_patch*patch,pw_buffer**b)
{
int rc;
pw_stack*stack;
pw_buffer*buf;
pw_bufferpool*pool;

stack= pw_patch_stack(patch);
pool= pw_patch_pool(patch);
rc= pw_stack_pop(stack,&buf);

if(rc!=PW_OK)return rc;

pw_bufferpool_clear_last_free(pool);
pw_bufferpool_holdu(pool,buf);

if(b!=NULL)*b= buf;

return PW_OK;
}

int pw_patch_bunhold(pw_patch*patch,pw_buffer*b)
{
pw_bufferpool*pool;
int rc;

pool= pw_patch_pool(patch);
rc= pw_bufferpool_unholdu(pool,b);
if(rc!=PW_OK)return rc;

return PW_OK;
}

/*:307*//*309:*/
#line 602 "./patch.w"

void pw_patch_err(pw_patch*patch,int rc)
{
patch->err= rc;
}

/*:309*//*311:*/
#line 615 "./patch.w"

pw_node*pw_patch_last_node(pw_patch*patch)
{
return patch->last;
}
#line 1 "./subpatch.w"
/*:311*//*402:*/
#line 23 "./print.w"

void pw_print(pw_patch*p,const char*fmt,...)
{
va_list args;
va_start(args,fmt);
p->print(p,fmt,args);
va_end(args);
}

/*:402*//*405:*/
#line 45 "./print.w"

/*406:*/
#line 55 "./print.w"

static void pw_print_default(pw_patch*p,const char*str,va_list args)
{
vprintf(str,args);
}


/*:406*/
#line 46 "./print.w"

void pw_print_init(pw_patch*p)
{
p->print= pw_print_default;
}

/*:405*//*408:*/
#line 69 "./print.w"

void pw_print_set(pw_patch*p,void(*print)(pw_patch*,const char*,va_list))
{
p->print= print;
}/*:408*/
#line 8 "./patch.w"


/*:249*//*312:*/
#line 9 "./subpatch.w"

/*318:*/
#line 48 "./subpatch.w"

void pw_subpatch_init(pw_subpatch*subpatch)
{
subpatch->nnodes= 0;
pw_pointerlist_init(&subpatch->plist);
}

/*:318*//*321:*/
#line 63 "./subpatch.w"

void pw_subpatch_save(pw_patch*patch,pw_subpatch*subpatch)
{
subpatch->nodes= patch->nodes;
subpatch->last= patch->last;
subpatch->nnodes= patch->nnodes;
subpatch->out= patch->out;
subpatch->plist= patch->plist;
}

/*:321*//*324:*/
#line 79 "./subpatch.w"

void pw_subpatch_restore(pw_patch*patch,pw_subpatch*subpatch)
{
patch->nodes= subpatch->nodes;
patch->last= subpatch->last;
patch->nnodes= subpatch->nnodes;
patch->out= subpatch->out;
patch->plist= subpatch->plist;
}

/*:324*//*326:*/
#line 94 "./subpatch.w"

void pw_subpatch_compute(pw_subpatch*subpatch)
{
int n;
pw_node*node;
pw_node*next;
node= subpatch->nodes;
for(n= 0;n<subpatch->nnodes;n++){
next= pw_node_get_next(node);
pw_node_compute(node);
node= next;
}
}

/*:326*//*328:*/
#line 113 "./subpatch.w"

void pw_subpatch_destroy(pw_subpatch*subpatch)
{
int n;
pw_node*node;
pw_node*next;
node= subpatch->nodes;
for(n= 0;n<subpatch->nnodes;n++){
next= pw_node_get_next(node);
pw_node_destroy(node);
node= next;
}
}

/*:328*//*330:*/
#line 133 "./subpatch.w"

void pw_subpatch_free(pw_subpatch*subpatch)
{
int n;
pw_node*node;
pw_node*next;
node= subpatch->nodes;
for(n= 0;n<subpatch->nnodes;n++){
next= pw_node_get_next(node);
free(node);
node= next;
}
subpatch->nnodes= 0;
pw_pointerlist_free(&subpatch->plist);
pw_pointerlist_init(&subpatch->plist);
}

/*:330*//*332:*/
#line 155 "./subpatch.w"

pw_cable*pw_subpatch_out(pw_subpatch*subpatch)
{
return subpatch->out;
}
#line 1 "./egraph.w"
/*:332*/
#line 10 "./subpatch.w"


/*:312*//*333:*/
#line 8 "./egraph.w"

/*337:*/
#line 52 "./egraph.w"

struct pw_evnode{
pw_evnode_cb event;
void(*clean)(pw_evnode*);
int dur;
void*ud;
pw_evnode*prev;
pw_evnode*next;
};

/*:337*//*340:*/
#line 76 "./egraph.w"

int pw_evnode_new(pw_patch*patch,pw_evnode**evn)
{
pw_evnode*e;
int rc;
e= NULL;
rc= pw_memory_alloc(patch,sizeof(pw_evnode),(void**)&e);
if(e!=NULL){
pw_evnode_init(e);
*evn= e;
}
return rc;
}

int pw_evnode_bye(pw_patch*patch,pw_evnode**evn)
{
return pw_memory_free(patch,(void**)evn);
}

/*:340*//*342:*/
#line 100 "./egraph.w"

/*336:*/
#line 28 "./egraph.w"

static pw_evnode*empty_event(pw_evnode*evn,int pos)
{
return pw_evnode_next(evn);
}

/*:336*/
#line 101 "./egraph.w"

/*343:*/
#line 115 "./egraph.w"

static void empty_clean(pw_evnode*evn)
{

}

/*:343*/
#line 102 "./egraph.w"

void pw_evnode_init(pw_evnode*evn)
{
evn->dur= 0;
evn->ud= NULL;
evn->event= empty_event;
evn->clean= empty_clean;
evn->prev= NULL;
evn->next= NULL;
}

/*:342*//*345:*/
#line 126 "./egraph.w"

pw_evnode*pw_evnode_fire(pw_evnode*evn,int pos)
{
return evn->event(evn,pos);
}

/*:345*//*347:*/
#line 136 "./egraph.w"

void pw_evnode_callback(pw_evnode*evn,pw_evnode_cb cb)
{
evn->event= cb;
}

/*:347*//*349:*/
#line 147 "./egraph.w"

void pw_evnode_clean(pw_evnode*evn)
{
evn->clean(evn);
}

/*:349*//*351:*/
#line 157 "./egraph.w"

void pw_evnode_clean_set(pw_evnode*evn,void(*cb)(pw_evnode*))
{
evn->clean= cb;
}

/*:351*//*353:*/
#line 180 "./egraph.w"

void pw_evnode_data_set(pw_evnode*evn,void*ud)
{
evn->ud= ud;
}

void*pw_evnode_data_get(pw_evnode*evn)
{
return evn->ud;
}

/*:353*//*355:*/
#line 197 "./egraph.w"

pw_evnode*pw_evnode_next(pw_evnode*evn)
{
return evn->next;
}

/*:355*//*357:*/
#line 208 "./egraph.w"

void pw_evnode_dur(pw_evnode*evn,int dur)
{
evn->dur= dur;
}

/*:357*//*359:*/
#line 218 "./egraph.w"

int pw_evnode_dur_get(pw_evnode*evn)
{
return evn->dur;
}

/*:359*//*361:*/
#line 232 "./egraph.w"

int pw_evnode_is_terminal(pw_evnode*evn)
{
return evn->next==NULL;
}

/*:361*//*363:*/
#line 245 "./egraph.w"

void pw_evnode_set_next(pw_evnode*evn,pw_evnode*next)
{
evn->next= next;
}

/*:363*//*364:*/
#line 256 "./egraph.w"

/*369:*/
#line 297 "./egraph.w"

void pw_evwalker_reset(pw_evwalker*walker)
{
walker->node= NULL;
walker->count= 0;
walker->terminal= 1;
}

/*:369*//*371:*/
#line 335 "./egraph.w"

void pw_evwalker_init(pw_evwalker*walker,pw_evnode*start,int delay)
{
walker->count= delay;
walker->node= start;
walker->terminal= 0;
if(start==NULL){
walker->terminal= 1;
}
}

/*:371*//*373:*/
#line 403 "./egraph.w"

int pw_evwalker_walk(pw_evwalker*walker,int pos)
{
if(walker->terminal)return 0;

if(walker->count> 0){
walker->count--;
return 1;
}else if(walker->count==0){
while(walker->count==0){
if(pw_evnode_is_terminal(walker->node)){
pw_evnode_fire(walker->node,pos);
walker->terminal= 1;
return 0;
}else{
walker->count= walker->node->dur;
walker->node= pw_evnode_fire(walker->node,pos);
if(walker->node==NULL){
walker->terminal= 1;
return 0;
}
}
}
walker->count--;
return!walker->terminal;
}

return 0;
}
#line 1 "./dump.w"
/*:373*/
#line 257 "./egraph.w"


/*:364*/
#line 9 "./egraph.w"


/*:333*//*374:*/
#line 9 "./dump.w"

/*376:*/
#line 16 "./dump.w"

void pw_dump_cable(FILE*fp,pw_cable*c)
{
fprintf(fp,"{");
fprintf(fp,"\"type\":");
if(c->type==CABLE_BLOCK){
fprintf(fp,"\"block\",");
}else if(c->type==CABLE_IVAL){
fprintf(fp,"\"constant\",");
}
fprintf(fp,"\"node_id\":");
if(c->node==NULL){
fprintf(fp,"-1,");
}else{
fprintf(fp,"%d,",pw_node_get_id(c->node));
}
fprintf(fp,"\"ival\":%g,",c->ival);
fprintf(fp,"\"buffer\":");
if(c->buf!=NULL){
fprintf(fp,"%d,",pw_buffer_id(c->buf));
}else{
fprintf(fp,"-1,");
}
fprintf(fp,"\"pcable\":");
if(c->pcable!=c){
pw_dump_cable(fp,c->pcable);
}else{
fprintf(fp,"{}");
}
fprintf(fp,"}");
}

/*:376*//*378:*/
#line 51 "./dump.w"

void pw_dump_node(FILE*fp,pw_node*n,int print_cables)
{
int c;
fprintf(fp,"{");
fprintf(fp,"\"id\":%d,",n->id);
fprintf(fp,"\"ncables\":%d,",n->ncables);
fprintf(fp,"\"type\":%d",n->type);
if(print_cables){
fprintf(fp,",");
fprintf(fp,"\"cables\":[");
for(c= 0;c<n->ncables;c++){
pw_dump_cable(fp,&n->cables[c]);
if(c!=n->ncables-1){
fprintf(fp,",");
}
}
fprintf(fp,"]");
}
fprintf(fp,"}");
}

/*:378*//*380:*/
#line 78 "./dump.w"

void pw_dump_nodes(FILE*fp,pw_node*nodes,int nnodes,int print_cables)
{
int n;
pw_node*nd;
pw_node*nxt;
nd= nodes;
fprintf(fp,"{");
fprintf(fp,"\"nnodes\":%d,",nnodes);
fprintf(fp,"\"nodes\":[");
for(n= 0;n<nnodes;n++){
nxt= nd->next;
pw_dump_node(fp,nd,print_cables);
if(n!=nnodes-1){
fprintf(fp,",");
}
nd= nxt;
}
fprintf(fp,"]");
fprintf(fp,"}");
}

/*:380*//*382:*/
#line 104 "./dump.w"

void pw_dump_nodelist(FILE*fp,pw_patch*p,int print_cables)
{
pw_dump_nodes(fp,p->nodes,p->nnodes,print_cables);
}


/*:382*//*384:*/
#line 115 "./dump.w"

void pw_dump_subpatch(FILE*fp,pw_subpatch*s,int print_cables)
{
pw_dump_nodes(fp,s->nodes,s->nnodes,print_cables);
}

/*:384*//*386:*/
#line 124 "./dump.w"

void pw_dump_bufferpool(FILE*fp,pw_bufferpool*bp,int print_buffers)
{
int b;
int pos;
fprintf(fp,"{");
fprintf(fp,"\"size\": %d,",bp->size);
fprintf(fp,"\"nactive\": %d,",bp->nactive);
fprintf(fp,"\"user_buffers\": %d,",pw_bufferpool_uactive(bp));
fprintf(fp,"\"last_free\": %d",bp->last_free);
if(print_buffers){
fprintf(fp,",\"buffers\":[");
pos= 0;
for(b= 0;b<bp->size;b++){
if(bp->buffers[b].read!=0){
pw_dump_buffer(fp,&bp->buffers[b]);
pos++;
if(pos!=bp->nactive)fprintf(fp,",");
}
}
fprintf(fp,"]");
}
fprintf(fp,"}");
}

/*:386*//*388:*/
#line 152 "./dump.w"

void pw_dump_stack(FILE*fp,pw_stack*s,int print_buffers)
{
int b;
fprintf(fp,"{");
fprintf(fp,"\"size\": %d,",s->size);
fprintf(fp,"\"pos\": %d",s->pos);
if(print_buffers){
fprintf(fp,",\"buffers\":[");
for(b= 0;b<s->pos;b++){
pw_dump_buffer(fp,s->buffers[b]);
if(b!=s->pos-1){
fprintf(fp,",");
}
}
fprintf(fp,"]");
}
fprintf(fp,"}");
}

/*:388*//*390:*/
#line 175 "./dump.w"

void pw_dump_buffer(FILE*fp,pw_buffer*b)
{
fprintf(fp,"{");
fprintf(fp,"\"id\": %d,",b->id);
fprintf(fp,"\"read\": %d",b->read);
fprintf(fp,"}");
}
#line 1 "./memory.w"
/*:390*/
#line 10 "./dump.w"


/*:374*//*391:*/
#line 10 "./memory.w"

/*393:*/
#line 27 "./memory.w"

int pw_memory_alloc(pw_patch*p,size_t size,void**ud)
{
void*ptr;

ptr= malloc(size);

if(ptr==NULL)return PW_NOT_OK;

*ud= ptr;
return PW_OK;
}

int pw_memory_free(pw_patch*p,void**ud)
{
free(*ud);
return PW_OK;
}

/*:393*//*397:*/
#line 66 "./memory.w"

void pw_memory_override(pw_patch*p,pw_mallocfun m,pw_freefun f)
{
p->malloc= m;
p->free= f;
}

/*:397*//*399:*/
#line 81 "./memory.w"

static int default_malloc(pw_patch*p,size_t size,void**ud)
{
void*ptr;

ptr= malloc(size);

if(ptr==NULL)return PW_NOT_OK;

*ud= ptr;
return PW_OK;
}

static int default_free(pw_patch*p,void**ud)
{
free(*ud);
return PW_OK;
}


void pw_memory_defaults(pw_patch*p)
{
pw_memory_override(p,default_malloc,default_free);
}
#line 1 "./print.w"
/*:399*/
#line 11 "./memory.w"


/*:391*/
#line 121 "./patchwerk.w"

#line 1 "./header.w"
/*:4*/
