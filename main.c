#include <stdlib.h>
#include <stdio.h>
#include "lil/lil.h"

void sklil_loader_withextra(lil_t lil);
void sklil_clean(lil_t lil);
int lil_main(int argc, char *argv[],
             void (*loader)(lil_t),
             void (*clean)(lil_t));

int main(int argc, char *argv[])
{
    return lil_main(argc, argv, sklil_loader_withextra, sklil_clean);
}
