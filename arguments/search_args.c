#include "search_args.h"

static int _cl_sa_process_dims(CLArgumentList * args, CuboidDimensions * dims);
static void _cl_sa_process_flags(CLArgumentList * args, CLSearchParameters * params);
static int _cl_sa_process_min_max(CLArgumentList * args, CLSearchParameters * params);
static int _cl_sa_process_threads(CLArgumentList * args, CLSearchParameters * params);
static int _cl_sa_process_operations(CLArgumentList * args, CLSearchParameters * params);
static int _cl_sa_process_heuristics(CLArgumentList * args, CLSearchParameters * params);

CLArgumentList * cl_sa_default_arguments() {
    CLArgumentList * list = cl_argument_list_new();
    
    cl_argument_list_add(list, cl_argument_new_string("dimensions", "3x3x3"));
    cl_argument_list_add(list, cl_argument_new_string("operations", ""));
    cl_argument_list_add(list, cl_argument_new_string("heuristic", ""));
    cl_argument_list_add(list, cl_argument_new_flag("multiple", 0));
    cl_argument_list_add(list, cl_argument_new_flag("verbose", 0));
    cl_argument_list_add(list, cl_argument_new_integer("mindepth", 0));
    cl_argument_list_add(list, cl_argument_new_integer("maxdepth", 20));
    cl_argument_list_add(list, cl_argument_new_integer("threads", 8));
    
    return list;
}

int cl_sa_process(CLArgumentList * args, CLSearchParameters * params) {
    
    if (!_cl_sa_process_dims(args, &params->dimensions)) return 0;
    _cl_sa_process_flags(args, params);
    if (!_cl_sa_process_min_max(args, params)) return 0;
    if (!_cl_sa_process_threads(args, params)) return 0;
    if (!_cl_sa_process_operations(args, params)) return 0;
    if (!_cl_sa_process_heuristics(args, params)) {
        alg_list_release(params->operations);
        return 0;
    }
    
    return 1;
    
}

int cl_sa_parse_dimensions(const char * str, CuboidDimensions * dims) {
    int xCount = 0;
    int xIndices[2];
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == 'x') {
            if (xCount == 2) return 0;
            xIndices[xCount] = i;
            xCount++;
        } else if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    
    if (xCount != 2) return 0;
    char * buffer = (char *)malloc(strlen(str) + 1);
    
    bzero(buffer, strlen(str) + 1);
    memcpy(buffer, str, xIndices[0]);
    dims->x = atoi(buffer);
    
    bzero(buffer, strlen(str) + 1);
    memcpy(buffer, &str[xIndices[0] + 1], xIndices[1] - xIndices[0] - 1);
    dims->y = atoi(buffer);
    
    bzero(buffer, strlen(str) + 1);
    strcpy(buffer, &str[xIndices[1] + 1]);
    dims->z = atoi(buffer);
    
    free(buffer);
    return 1;
}

void cl_sa_remove_all(CLArgumentList * args) {
    CLArgumentList * defaults = cl_sa_default_arguments();
    int i;
    for (i = 0; i < cl_argument_list_count(defaults); i++) {
        int j;
        const char * name = cl_argument_list_get(defaults, i)->name;
        while ((j = cl_argument_list_find(args, name)) >= 0) {
            cl_argument_list_remove(args, j);
        }
    }
    cl_argument_list_free(defaults);
}

void cl_sa_print_usage(CLArgumentList * defArgs) {
    int j;
    for (j = 0; j < cl_argument_list_count(defArgs); j++) {
        CLArgument * arg = cl_argument_list_get(defArgs, j);
        printf("%s--%s", j == 0 ? "" : " ", arg->name);
        if (arg->type == CLArgumentTypeString) {
            printf(" <str>");
        } else if (arg->type == CLArgumentTypeInteger) {
            printf("=<num>");
        }
    }
}

/**************
 * Processing *
 **************/

static int _cl_sa_process_dims(CLArgumentList * args, CuboidDimensions * dims) {
    int index = cl_argument_list_find(args, "dimensions");
    assert(index >= 0);
    
    CLArgument * argument = cl_argument_list_get(args, index);
    if (!cl_sa_parse_dimensions(argument->contents.string.value, dims)) {
        return 0;
    }
    if (dims->x < 2 || dims->y < 2 || dims->z < 2) {
        return 0;
    }
    return 1;
}

static void _cl_sa_process_flags(CLArgumentList * args, CLSearchParameters * params) {
    int index;
    CLArgument * argument;
    
    index = cl_argument_list_find(args, "multiple");
    assert(index >= 0);
    argument = cl_argument_list_get(args, index);
    params->multipleFlag = argument->contents.flag.boolValue;
    
    index = cl_argument_list_find(args, "verbose");
    assert(index >= 0);
    argument = cl_argument_list_get(args, index);
    params->verboseFlag = argument->contents.flag.boolValue;
}

static int _cl_sa_process_min_max(CLArgumentList * args, CLSearchParameters * params) {
    int index;
    CLArgument * argument;
    
    index = cl_argument_list_find(args, "mindepth");
    assert(index >= 0);
    argument = cl_argument_list_get(args, index);
    params->minDepth = argument->contents.integer.value;
    
    index = cl_argument_list_find(args, "maxdepth");
    assert(index >= 0);
    argument = cl_argument_list_get(args, index);
    params->maxDepth = argument->contents.integer.value;
    
    return (params->maxDepth >= params->minDepth);
}

static int _cl_sa_process_threads(CLArgumentList * args, CLSearchParameters * params) {
    int index;
    CLArgument * argument;
    
    index = cl_argument_list_find(args, "threads");
    assert(index >= 0);
    argument = cl_argument_list_get(args, index);
    params->threadCount = argument->contents.integer.value;
    
    return (params->threadCount > 0);
}

static int _cl_sa_process_operations(CLArgumentList * args, CLSearchParameters * params) {
    int index;
    CLArgument * argument;
    
    index = cl_argument_list_find(args, "operations");
    if (index < 0) {
        // give the standard basis
        params->operations = cuboid_standard_basis(params->dimensions);
    } else {
        argument = cl_argument_list_get(args, index);
        params->operations = alg_list_parse(argument->contents.string.value,
                                            params->dimensions);
    }
    
    return (params->operations != NULL);
}

static int _cl_sa_process_heuristics(CLArgumentList * args, CLSearchParameters * params) {
    HeuristicList * list = heuristic_list_new();
    
    int i;
    for (i = 0; i < cl_argument_list_count(args); i++) {
        CLArgument * argument = cl_argument_list_get(args, i);
        if (strcmp(argument->name, "heuristic")) continue;
        const char * fileName = argument->contents.string.value;
        Heuristic * heuristic = heuristic_from_file(fileName, params->dimensions);
        if (!heuristic) {
            heuristic_list_free(list);
            return 0;
        }
        heuristic_list_add(list, heuristic, fileName);
    }
    
    Cuboid * temp = cuboid_create(params->dimensions);
    heuristic_list_prepare(list, temp);
    cuboid_free(temp);
    
    params->heuristics = list;
    return 1;
}
