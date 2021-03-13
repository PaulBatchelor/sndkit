#ifndef SK_NODES_H
#define SK_NODES_H
int sk_node_biscale(sk_core *core);
int sk_node_scale(sk_core *core);
int sk_node_wavout(sk_core *core, const char *filename);
int sk_node_sine(sk_core *core);
int sk_node_dcblocker(sk_core *core);
int sk_node_rephasor(sk_core *core);
int sk_node_phsdiv(sk_core *core);
int sk_node_phsmul(sk_core *core);
int sk_node_fmpair(sk_core *core);
int sk_node_osc(sk_core *core);
int sk_node_phasor(sk_core *core, SKFLT iphs);
int sk_node_add(sk_core *core);
int sk_node_mul(sk_core *core);
int sk_node_sub(sk_core *core);
int sk_node_div(sk_core *core);
int sk_node_bigverb(sk_core *core);
int sk_node_gensine(sk_core *core);
int sk_node_smoother(sk_core *core);
#endif
