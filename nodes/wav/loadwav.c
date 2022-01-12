#include <stdio.h>

#include "graforge.h"
#include "core.h"
#include "dr_wav.h"

int sk_loadwav(sk_core *core, const char *filename)
{
    int rc;
    sk_table *tab;
    sk_drwav wav;
    SKFLT *buf;

    if (!sk_drwav_init_file(&wav, filename, NULL)) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        return 1;
    }

    rc = sk_core_table_new(core, wav.totalPCMFrameCount);
    SK_ERROR_CHECK(rc);
    rc = sk_core_table_pop(core, &tab);
    SK_ERROR_CHECK(rc);

    buf = sk_table_data(tab);

    sk_drwav_read_pcm_frames_f32(&wav, sk_table_size(tab), buf);

    sk_drwav_uninit(&wav);

    rc = sk_core_table_push(core, tab);
    SK_ERROR_CHECK(rc);

    return 0;
}
