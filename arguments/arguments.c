#include "arguments.h"

CLArgument * cl_argument_new(const char * name, CLArgumentType type) {
    CLArgument * argument = (CLArgument *)malloc(sizeof(CLArgument));
    bzero(argument, sizeof(CLArgument));
    
    argument->name = (char *)malloc(strlen(name) + 1);
    strcpy(argument->name, name);
    argument->type = type;
    
    return argument;
}

CLArgument * cl_argument_new_flag(const char * name, int boolValue) {
    CLArgument * arg = cl_argument_new(name, CLArgumentTypeFlag);
    arg->contents.flag.boolValue = boolValue;
    return arg;
}

CLArgument * cl_argument_new_integer(const char * name, int value) {
    CLArgument * arg = cl_argument_new(name, CLArgumentTypeInteger);
    arg->contents.integer.value = value;
    return arg;
}

CLArgument * cl_argument_new_string(const char * name, const char * value) {
    CLArgument * arg = cl_argument_new(name, CLArgumentTypeString);
    arg->contents.string.value = (char *)malloc(strlen(value) + 1);
    strcpy(arg->contents.string.value, value);
    return arg;
}

CLArgument * cl_argument_copy(CLArgument * arg) {
    CLArgument * copy = cl_argument_new(arg->name, arg->type);
    if (copy->type == CLArgumentTypeInteger) {
        copy->contents.integer.value = arg->contents.integer.value;
    } else if (copy->type == CLArgumentTypeFlag) {
        copy->contents.flag.boolValue = arg->contents.flag.boolValue;
    } else if (copy->type == CLArgumentTypeString) {
        const char * copyMe = arg->contents.string.value;
        copy->contents.string.value = (char *)malloc(strlen(copyMe) + 1);
        strcpy(copy->contents.string.value, copyMe);
    }
    return copy;
}

void cl_argument_free(CLArgument * argument) {
    if (argument->type == CLArgumentTypeString) {
        free(argument->contents.string.value);
    }
    free(argument->name);
    free(argument);
}

/******************
 * Argument lists *
 ******************/

CLArgumentList * cl_argument_list_new() {
    CLArgumentList * list = (CLArgumentList *)malloc(sizeof(CLArgumentList));
    bzero(list, sizeof(CLArgumentList));
    return list;
}

void cl_argument_list_add(CLArgumentList * list, CLArgument * arg) {
    if (list->count == 0) {
        list->arguments = (CLArgument **)malloc(sizeof(CLArgument *));
    } else {
        int size = sizeof(CLArgument *) * (list->count + 1);
        list->arguments = (CLArgument **)realloc(list->arguments, size);
    }
    list->arguments[list->count] = arg;
    list->count++;
}

void cl_argument_list_add_all(CLArgumentList * list, const CLArgumentList * items) {
    int i;
    for (i = 0; i < items->count; i++) {
        CLArgument * arg = items->arguments[i];
        cl_argument_list_add(list, cl_argument_copy(arg));
    }
}

int cl_argument_list_count(const CLArgumentList * list) {
    return list->count;
}

CLArgument * cl_argument_list_get(const CLArgumentList * list, int index) {
    assert(index >= 0 && index < list->count);
    return list->arguments[index];
}

void cl_argument_list_remove(CLArgumentList * list, int index) {
    assert(index >= 0 && index < list->count);
    int numCopied = 0, i;
    for (i = 0; i < list->count; i++) {
        if (i != index) {
            list->arguments[numCopied] = list->arguments[i];
            numCopied++;
        } else {
            cl_argument_free(list->arguments[i]);
        }
    }
    list->count = numCopied;
}

int cl_argument_list_find(const CLArgumentList * list, const char * name) {
    int i;
    for (i = 0; i < list->count; i++) {
        CLArgument * arg = cl_argument_list_get(list, i);
        if (strcmp(arg->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void cl_argument_list_free(CLArgumentList * list) {
    int i;
    for (i = 0; i < list->count; i++) {
        cl_argument_free(list->arguments[i]);
    }
    free(list->arguments);
    free(list);
}

