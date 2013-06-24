#include "heuristic_angles.h"

HeuristicAngles * heuristic_angles_new() {
    HeuristicAngles * angles = (HeuristicAngles *)malloc(sizeof(HeuristicAngles));
    bzero(angles, sizeof(HeuristicAngles));
    return angles;
}

void heuristic_angles_add_distinct(HeuristicAngles * angles) {
    if (angles->saveAngles) {
        int newSize = sizeof(int) * (angles->numAngles + 1);
        angles->saveAngles = (int *)realloc(angles->saveAngles, newSize);
    } else {
        angles->saveAngles = (int *)malloc(sizeof(int));
    }
    if (angles->distinct) {
        int newSize = sizeof(int) * (angles->numDistinct + 1);
        angles->distinct = (int *)realloc(angles->distinct, newSize);
    } else {
        angles->distinct = (int *)malloc(sizeof(int));
    }
    
    int angle = angles->numAngles;
    angles->saveAngles[angles->numAngles] = angle;
    angles->distinct[angles->numDistinct] = angle;
    
    angles->numAngles++;
    angles->numDistinct++;
}

void heuristic_angles_add_duplicate(HeuristicAngles * angles, int saveAngle) {
    assert(angles->numAngles > 0);
    if (angles->saveAngles) {
        int newSize = sizeof(int) * (angles->numAngles + 1);
        angles->saveAngles = (int *)realloc(angles->saveAngles, newSize);
    } else {
        angles->saveAngles = (int *)malloc(sizeof(int));
    }
    
    angles->saveAngles[angles->numAngles] = saveAngle;
    angles->numAngles++;
}

void heuristic_angles_free(HeuristicAngles * angles) {
    if (angles->saveAngles) {
        free(angles->saveAngles);
    }
    if (angles->distinct) {
        free(angles->distinct);
    }
    free(angles);
}

HeuristicAngles * heuristic_angles_for_subproblem(HSubproblem sp, void * userData) {
    HeuristicAngles * angles = heuristic_angles_new();
    int i, j, saveAngle;
    for (i = 0; i < sp.angle_count(userData); i++) {
        saveAngle = -1;
        for (j = 0; j < angles->numDistinct; j++) {
            int testAngle = angles->distinct[j];
            if (sp.angles_are_equivalent(userData, testAngle, i)) {
                saveAngle = testAngle;
                break;
            }
        }
        if (saveAngle < 0) {
            heuristic_angles_add_distinct(angles);
        } else {
            heuristic_angles_add_duplicate(angles, saveAngle);
        }
    }
    return angles;
}

int heuristic_angles_equal(HeuristicAngles * a1, HeuristicAngles * a2) {
    if (a1->numAngles != a2->numAngles) return 0;
    if (a1->numDistinct != a2->numDistinct) return 0;
    int i;
    for (i = 0; i < a1->numAngles; i++) {
        if (a1->saveAngles[i] != a2->saveAngles[i]) return 0;
    }
    for (i = 0; i < a1->numDistinct; i++) {
        if (a1->distinct[i] != a2->distinct[i]) return 0;
    }
    return 1;
}
