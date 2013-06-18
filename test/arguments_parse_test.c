#include "test.h"
#include "arguments/parser.h"

void test_parse();

int main() {
    test_parse();
    tests_completed();
    return 0;
}

void test_parse() {
    test_initiated("cl_parse_arguments");
    
    const char * arguments[] = {"--maxdepth=2", "--heuristic", "/file", "--multiple"};
    CLArgumentList * defs = cl_argument_list_new();
    cl_argument_list_add(defs, cl_argument_new_integer("mindepth", 0));
    cl_argument_list_add(defs, cl_argument_new_integer("maxdepth", 20));
    cl_argument_list_add(defs, cl_argument_new_string("heuristic", ""));
    cl_argument_list_add(defs, cl_argument_new_flag("multiple", 0));
    
    int index = 0;
    CLArgumentList * parsed = cl_parse_arguments(arguments, 4, defs, &index);
    cl_argument_list_free(defs);
    
    if (!parsed) {
        printf("Error: parser returned NULL. Error at %d.\n", index);
        test_completed();
        return;
    }
    
    if (parsed->count != 4) {
        printf("Error: parser returned invalid argument count of %d.\n", parsed->count);
    }
    
    CLArgument * arg = cl_argument_list_get(parsed, 0);
    if (arg->type != CLArgumentTypeInteger || 
        strcmp(arg->name, "maxdepth") ||
        arg->contents.integer.value != 2) {
            puts("Error: invalid argument at index 0.");
    }
    arg = cl_argument_list_get(parsed, 1);
    if (arg->type != CLArgumentTypeString ||
        strcmp(arg->name, "heuristic") ||
        strcmp(arg->contents.string.value, "/file")) {
            puts("Error: invalid argument at index 1.");
    }
    arg = cl_argument_list_get(parsed, 2);
    if (arg->type != CLArgumentTypeFlag ||
        strcmp(arg->name, "multiple") ||
        !arg->contents.flag.boolValue) {
            puts("Error: invalid argument at index 2.");
    }
    arg = cl_argument_list_get(parsed, 3);
    if (arg->type != CLArgumentTypeInteger || 
        strcmp(arg->name, "mindepth") ||
        arg->contents.integer.value != 0) {
            puts("Error: invalid argument at index 3.");
    }
    
    cl_argument_list_free(parsed);
    test_completed();
}
