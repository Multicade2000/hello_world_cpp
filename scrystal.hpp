#include <sys/types.h>
#include <stdio.h>
#define _WCHAR_T
#include <stdlib.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

class SigmaCrystal
{
public:
    SigmaCrystal();
    ~SigmaCrystal();

    TMD_PRIM *mdl;
    int n_prim;

    SVECTOR Rotate;
    VECTOR Trans;

    VECTOR Scale;
    MATRIX Matrix;

    void PrepareModel(u_long *file);
    char *DrawModel(u_long *ot, char *pri, int max_ot);
};