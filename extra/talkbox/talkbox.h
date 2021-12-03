#ifndef SK_TALKBOX_H
#define SK_TALKBOX_H

#ifndef SKFLT
#define SKFLT float
#endif

#ifndef SK_TALKBOX_BUFMAX
#define SK_TALKBOX_BUFMAX 1600
#endif

typedef struct sk_talkbox sk_talkbox;

void sk_talkbox_init(sk_talkbox *t, int sr);
SKFLT sk_talkbox_tick(sk_talkbox *t, SKFLT src, SKFLT exc);

#endif
