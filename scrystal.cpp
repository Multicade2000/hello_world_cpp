#include "scrystal.hpp"

SigmaCrystal::SigmaCrystal()
{
    n_prim = 0;

    Rotate = {0, 0, 0, 0};
    Trans = {0, 0, 512, 0};

    Scale = {ONE / 2, ONE / 2, ONE / 2, 0};
    Matrix = {0};
}

SigmaCrystal::~SigmaCrystal()
{
    free(mdl);
}

void SigmaCrystal::PrepareModel(u_long *file)
{
    n_prim = OpenTMD(file, 0);
    mdl = (TMD_PRIM *)malloc(n_prim * sizeof(TMD_PRIM));
    TMD_PRIM tmd;
    for (int i = 0; i < n_prim && ReadTMD(&tmd) != 0; i++)
    {
        mdl[i] = tmd;
    }
}

char *SigmaCrystal::DrawModel(u_long *ot, char *pri, int max_ot)
{
    long p, OTz, Flag;
    
    Rotate.vy += 16;

    RotMatrix(&Rotate, &Matrix);
    TransMatrix(&Matrix, &Trans);
    ScaleMatrix(&Matrix, &Scale);
    SetRotMatrix(&Matrix);
    SetTransMatrix(&Matrix);

    for (int i = 0; i < n_prim; i++)
    {
        POLY_GT3 *poly = (POLY_GT3 *)pri;

        SetPolyGT3(poly);
        poly->tpage = mdl[i].tpage;
        poly->clut = mdl[i].clut;
        setRGB0(poly, 128, 128, 128);
        setRGB1(poly, 128, 128, 128);
        setRGB2(poly, 128, 128, 128);
        setUV3(poly, mdl[i].u0, mdl[i].v0, mdl[i].u1, mdl[i].v1, mdl[i].u2, mdl[i].v2);

        OTz = RotTransPers(&mdl[i].x0, (long *)&poly->x0, &p, &Flag);
        OTz += RotTransPers(&mdl[i].x1, (long *)&poly->x1, &p, &Flag);
        OTz += RotTransPers(&mdl[i].x2, (long *)&poly->x2, &p, &Flag);

        OTz /= 3;
        if ((OTz > 0) && (OTz < max_ot))
            AddPrim(&ot[OTz - 2], poly);
        pri += sizeof(POLY_GT3);
    }

    return pri;
}