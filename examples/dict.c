/* simple dictionary test */

#include <stdlib.h>
#include <stdio.h>
#include "graforge.h"
#include "core.h"

static void clean(void *ptr)
{
    int *n;
    n = (int *)ptr;
    printf("cleanup for %d\n", *n);
}

int main(int argc, char *argv)
{
    int data[3];
    sk_dict *d;
    void *ud;
    int rc;

    data[0] = 12345;
    data[1] = 88888;
    data[2] = 66666;

    d = malloc(sk_dict_sizeof());

    sk_dict_init(d);

    sk_dict_append(d, "foo", 3, &data[0], NULL);
    sk_dict_append(d, "bar", 3, &data[1], clean);

    ud = NULL;

    rc = sk_dict_lookup(d, "foo", 3, &ud);

    if (rc) {
        printf("could not find foo\n");
    } else {
        int *n;
        n = (int *) ud;
        printf("found foo: data is %d\n", *n);
    }

    rc = sk_dict_lookup(d, "foop", 3, &ud);

    if (!rc) {
        printf("foop (3 characters) was found. oops!\n");
    }

    rc = sk_dict_lookup(d, "foop", 4, &ud);

    if (rc) {
        printf("foop (4 characters) was not found. as it should be!\n");
    }

    sk_dict_remove(d, "foo", 3);
    sk_dict_append(d, "foo", 3, &data[2], clean);

    sk_dict_clean(d);

    free(d);
    return 0;
}
