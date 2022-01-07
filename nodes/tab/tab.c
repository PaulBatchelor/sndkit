#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lil/lil.h"
#include "graforge.h"
#include "core.h"
#include "sklil.h"

static lil_value_t tabnew(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    unsigned long sz;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tabnew", argc, 1);

    sz = lil_to_integer(argv[0]);

    if (argc == 1) {
        rc = sk_core_table_new(core, sz);
    } else {
        const char *key;

        key = lil_to_string(argv[1]);
        rc = sk_core_append_table(core, key, strlen(key), sz);
    }

    SKLIL_ERROR_CHECK(lil, rc, "Could not create table.");

    return NULL;
}

static lil_value_t tabdump(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    const char *filename;
    int rc;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tabdump", argc, 2);

    filename = lil_to_string(argv[1]);

    rc = sk_core_tabdump(core, filename);

    SKLIL_ERROR_CHECK(lil, rc, "Could not dump table.");

    return NULL;
}

/* tabdup: duplicates a table on the stack, and creates
 * a new entry in the dictionary
 */
static lil_value_t tabdup(lil_t lil, size_t argc, lil_value_t *argv)
{
    sk_core *core;
    int rc;
    sk_table *old;
    sk_table *new;
    const char *key;
    SKFLT *oldvals;
    SKFLT *newvals;
    int sz;
    int n;

    core = lil_get_data(lil);

    SKLIL_ARITY_CHECK(lil, "tabdup", argc, 2);

    rc = sk_core_table_pop(core, &old);

    SKLIL_ERROR_CHECK(lil, rc, "Could not get table.");

    key = lil_to_string(argv[1]);
    sz = sk_table_size(old);

    rc = sk_core_append_table(core, key, strlen(key), sz);
    SKLIL_ERROR_CHECK(lil, rc, "Could not create new table.");
    rc = sk_core_table_pop(core, &new);
    SKLIL_ERROR_CHECK(lil, rc, "Could not get new table.");

    newvals = sk_table_data(new);
    oldvals = sk_table_data(old);

    for (n = 0; n < sz; n++) newvals[n] = oldvals[n];

    rc = sk_core_table_push(core, new);
    SKLIL_ERROR_CHECK(lil, rc, "Could not push table.");

    return NULL;
}

void sklil_load_tab(lil_t lil)
{
    lil_register(lil, "tabnew", tabnew);
    lil_register(lil, "tabdump", tabdump);
    lil_register(lil, "tabdup", tabdup);
}
