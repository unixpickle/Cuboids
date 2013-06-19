#ifndef __ARGUMENTS_H__
#define __ARGUMENTS_H__

/**
 * Command-line arguments
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef enum {
    CLArgumentTypeFlag,
    CLArgumentTypeInteger,
    CLArgumentTypeString
} CLArgumentType;

typedef struct {
    int boolValue;
} CLArgFlag;

typedef struct {
    int value;
} CLArgInteger;

typedef struct {
    char * value;
} CLArgString;

typedef struct {
    CLArgumentType type;
    char * name;
    union {
        CLArgFlag flag;
        CLArgInteger integer;
        CLArgString string;
    } contents;
} CLArgument;

typedef struct {
    CLArgument ** arguments;
    int count;
} CLArgumentList;

CLArgument * cl_argument_new(const char * name, CLArgumentType type);
CLArgument * cl_argument_new_flag(const char * name, int boolValue);
CLArgument * cl_argument_new_integer(const char * name, int value);
CLArgument * cl_argument_new_string(const char * name, const char * value);
CLArgument * cl_argument_copy(CLArgument * arg);
void cl_argument_free(CLArgument * argument);

CLArgumentList * cl_argument_list_new();
void cl_argument_list_add(CLArgumentList * list, CLArgument * arg);
void cl_argument_list_add_all(CLArgumentList * list, const CLArgumentList * items);
int cl_argument_list_count(const CLArgumentList * list);
CLArgument * cl_argument_list_get(const CLArgumentList * list, int index);
void cl_argument_list_remove(CLArgumentList * list, int index);
int cl_argument_list_find(const CLArgumentList * list, const char * name);
void cl_argument_list_free(CLArgumentList * list);

#endif