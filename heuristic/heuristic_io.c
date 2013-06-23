#include "heuristic_io.h"

static void _save_heuristic_parameters(HSParameters params, FILE * fp);
static void _save_cosets(Heuristic * heuristic, FILE * fp);

static int _load_subproblem(FILE * fp, HSubproblem * spOut);
static int _load_heuristic_parameters(FILE * fp, HSParameters * params);
static int _load_cosets(Heuristic * heuristic, FILE * fp);
static int _initialize_subproblem(Heuristic * heuristic, FILE * fp);
static void _generate_symmetries(Heuristic * heuristic);

void save_heuristic(Heuristic * heuristic, FILE * fp) {
    fwrite(heuristic->subproblem.name, 1,
           strlen(heuristic->subproblem.name) + 1, fp);
    _save_heuristic_parameters(heuristic->params, fp);
    _save_cosets(heuristic, fp);
    heuristic->subproblem.save(heuristic->spUserData, fp);
}

Heuristic * load_heuristic(FILE * fp, CuboidDimensions newDims) {
    HSubproblem subproblem;
    HSParameters params;
    params.symmetries.dims = newDims;
    if (!_load_subproblem(fp, &subproblem)) return NULL;
    if (!_load_heuristic_parameters(fp, &params)) return NULL;
    
    RotationBasis general = rotation_basis_standard(newDims);
    if (!rotation_basis_is_subset(general, params.symmetries)) {
        return NULL;
    }
    
    Heuristic * heuristic = (Heuristic *)malloc(sizeof(Heuristic));
    bzero(heuristic, sizeof(Heuristic));
    heuristic->subproblem = subproblem;
    heuristic->params = params;
    if (!_load_cosets(heuristic, fp)) {
        free(heuristic);
        return NULL;
    }
    
    if (!_initialize_subproblem(heuristic, fp)) {
        int i;
        for (i = 0; i < heuristic->cosetCount; i++) {
            data_list_free(heuristic->cosets[i]);
        }
        if (heuristic->cosets) free(heuristic->cosets);
        free(heuristic);
        return NULL;
    }
    
    _generate_symmetries(heuristic);
    return heuristic;
}

/*******************
 * Private: saving *
 *******************/

static void _save_heuristic_parameters(HSParameters params, FILE * fp) {
    RotationBasis basis = params.symmetries;
    uint8_t xPower = basis.xPower;
    uint8_t yPower = basis.yPower;
    uint8_t zPower = basis.zPower;
    fwrite(&xPower, 1, 1, fp);
    fwrite(&yPower, 1, 1, fp);
    fwrite(&zPower, 1, 1, fp);
}

static void _save_cosets(Heuristic * heuristic, FILE * fp) {
    uint32_t count = heuristic->cosetCount;
    save_uint32(count, fp);
    int i;
    for (i = 0; i < count; i++) {
        save_data_list(heuristic->cosets[i], fp);
    }
}

/********************
 * Private: loading *
 ********************/

static int _load_subproblem(FILE * fp, HSubproblem * spOut) {
    char * nameBuffer = (char *)malloc(1);
    nameBuffer[0] = 0;
    int len = 0, chr;
    while ((chr = fgetc(fp)) != EOF) {
        if (chr == 0) break;
        nameBuffer = (char *)realloc(nameBuffer, len + 2);
        nameBuffer[len] = (char)chr;
        nameBuffer[len + 1] = 0;
        len++;
    }
    int i, spCount = sizeof(HSubproblemTable) / sizeof(HSubproblem);
    for (i = 0; i < spCount; i++) {
        HSubproblem subproblem = HSubproblemTable[i];
        if (strcmp(subproblem.name, nameBuffer) == 0) {
            *spOut = subproblem;
            free(nameBuffer);
            return 1;
        }
    }
    free(nameBuffer);
    return 0;
}

static int _load_heuristic_parameters(FILE * fp, HSParameters * params) {
    uint8_t xPow, yPow, zPow;
    if (!load_uint8(&xPow, fp)) return 0;
    if (!load_uint8(&yPow, fp)) return 0;
    if (!load_uint8(&zPow, fp)) return 0;
    params->symmetries.xPower = xPow;
    params->symmetries.yPower = yPow;
    params->symmetries.zPower = zPow;
    return 1;
}

static int _load_cosets(Heuristic * heuristic, FILE * fp) {
    uint32_t count;
    if (!load_uint32(&count, fp)) return 0;
    heuristic->cosetCount = count;
    if (count > 0) {
        heuristic->cosets = (DataList **)malloc(sizeof(void *) * count);
        int i, j;
        for (i = 0; i < count; i++) {
            DataList * dl = load_data_list(fp);
            if (!dl) {
                for (j = 0; j < i; j++) {
                    data_list_free(heuristic->cosets[j]);
                }
                free(heuristic->cosets);
                return 0;
            }
            heuristic->cosets[i] = dl;
        }
    } else {
        heuristic->cosets = NULL;
    }
    return 1;
}

static int _initialize_subproblem(Heuristic * heuristic, FILE * fp) {
    return heuristic->subproblem.load(heuristic->params, fp,
                                      &heuristic->spUserData);
}

static void _generate_symmetries(Heuristic * heuristic) {
    RotationBasis basis = heuristic->params.symmetries;
    heuristic->symmetries = rotation_group_create_basis(basis);
}
