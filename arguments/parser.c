#include "parser.h"

static int _cl_count_dashes(const char * argument);
static int _cl_first_equals(const char * argument);

static char * _cl_argument_name(const char * argument);
static int _cl_argument_number(const char * argument);

static void _cl_arguments_add_defaults(CLArgumentList * list, CLArgumentList * defs);

// main implementation
CLArgumentList * cl_parse_arguments(const char ** argv, int argc,
                                    CLArgumentList * defaults, int * failIndex) {
    int i;
    CLArgumentList * result = cl_argument_list_new();
    for (i = 0; i < argc; i++) {
        if (failIndex) *failIndex = i;
        
        char * name = _cl_argument_name(argv[i]);
        int defIndex = cl_argument_list_find(defaults, name);
        if (defIndex < 0) {
            free(name);
            cl_argument_list_free(result);
            return NULL;
        }
        
        CLArgument * def = cl_argument_list_get(defaults, defIndex);
        free(name);
        
        CLArgument * addArg = NULL;
        
        if (def->type == CLArgumentTypeFlag) {
            if (_cl_first_equals(argv[i]) < 0) {
                addArg = cl_argument_new_flag(def->name, !def->contents.flag.boolValue);
            }
        } else if (def->type == CLArgumentTypeInteger) {
            int numValue = _cl_argument_number(argv[i]);
            if (numValue >= 0) {
                addArg = cl_argument_new_integer(def->name, numValue);
            }
        } else if (def->type == CLArgumentTypeString) {
            if (_cl_first_equals(argv[i]) < 0 && i + 1 < argc) {
                addArg = cl_argument_new_string(def->name, argv[i + 1]);
                i++;
            }
        }
        if (!addArg) {
            cl_argument_list_free(result);
            return NULL;
        } else {
            cl_argument_list_add(result, addArg);
        }
    }
    
    _cl_arguments_add_defaults(result, defaults);
    
    return result;
}

/***********
 * Private *
 ***********/

static int _cl_count_dashes(const char * argument) {
    int count = 0, i;
    for (i = 0; i < strlen(argument); i++) {
        if (argument[i] != '-') break;
        count ++;
    }
    return count;
}

static int _cl_first_equals(const char * argument) {
    int i;
    for (i = 0; i < strlen(argument); i++) {
        if (argument[i] == '=') return i;
    }
    return -1;
}

static char * _cl_argument_name(const char * argument) {
    int dashCount = _cl_count_dashes(argument);
    int firstEquals = _cl_first_equals(argument);
    int nameStart = dashCount < 3 ? dashCount : 2;
    int nameLen = strlen(argument) - nameStart;
    
    if (firstEquals > 0) {
        nameLen = firstEquals - nameStart;
    }
    
    char * nameStr = (char *)malloc(nameLen + 1);
    nameStr[nameLen] = 0;
    memcpy(nameStr, &argument[nameStart], nameLen);
    return nameStr;
}

static int _cl_argument_number(const char * argument) {
    int firstEquals = _cl_first_equals(argument);
    if (firstEquals < 0) return -1;
    if (firstEquals + 1 >= strlen(argument)) return -1;
    
    int areDigits = 1, i;
    for (i = firstEquals + 1; i < strlen(argument); i++) {
        if (argument[i] < '0' || argument[i] > '9') {
            areDigits = 0;
            break;
        }
    }
    if (!areDigits) return -1;
    
    return atoi(&argument[firstEquals + 1]);
}

static void _cl_arguments_add_defaults(CLArgumentList * list, CLArgumentList * defs) {
    int i;
    for (i = 0; i < defs->count; i++) {
        CLArgument * arg = cl_argument_list_get(defs, i);
        if (arg->type == CLArgumentTypeString) {
            if (strlen(arg->contents.string.value) == 0) {
                continue;
            }
        }
        if (cl_argument_list_find(list, arg->name) < 0) {
            CLArgument * newArg = cl_argument_copy(arg);
            cl_argument_list_add(list, newArg);
        }
    }
}
