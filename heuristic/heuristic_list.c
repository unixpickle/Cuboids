#include "heuristic_list.h"

HeuristicList * heuristic_list_new() {
    HeuristicList * list = (HeuristicList *)malloc(sizeof(HeuristicList));
    bzero(list, sizeof(HeuristicList));
    return list;
}

void heuristic_list_free(HeuristicList * list) {
    free(list);
}

void heuristic_list_add(HeuristicList * list, Heuristic * h, const char * file) {
    if (!list->heuristics) {
        list->heuristics = (Heuristic **)malloc(sizeof(void *));
    } else {
        int newSize = (list->count + 1) * sizeof(void *);
        list->heuristics = (Heuristic **)realloc(list->heuristics, newSize);
    }
    if (!list->fileNames) {
        list->fileNames = (char **)malloc(sizeof(void *));
    } else {
        int newSize = (list->count + 1) * sizeof(void *);
        list->fileNames = (char **)realloc(list->fileNames, newSize);
    }
    list->heuristics[list->count] = h;
    list->fileNames[list->count] = (char *)malloc(strlen(file) + 1);
    strcpy(list->fileNames[list->count], file);
    list->count++;
}
