#include "test.h"
#include "notation/parser.h"

void test_tokens();

int main() {
    test_tokens();
    
    tests_completed();
    return 0;
}

void test_tokens() {
    test_initiated("individual tokens");
    
    
    // test Rw
    Algorithm * wideRight = algorithm_for_token("Rw");
    
    if (wideRight->type != AlgorithmTypeWideTurn)
        puts("Error: Rw processed invalid type.");
    if (wideRight->contents.wideTurn.face != 'R')
        puts("Error: Rw processed invalid face.");
    if (wideRight->contents.wideTurn.numLayers != 2)
        puts("Error: Rw processed invalid layer count.");
    if (wideRight->inverseFlag)
        puts("Error: Rw processed an inverse.");
    if (wideRight->power != 1)
        puts("Error: Rw processed the wrong power.");
    
    algorithm_free(wideRight);
    
    Algorithm * shouldBeNull = algorithm_for_token("2M");
    if (shouldBeNull != NULL) {
        puts("Error: failed to detect that 2M is invalid.");
        algorithm_free(shouldBeNull);
    }
    
    Algorithm * mPrime = algorithm_for_token("M'");
    if (mPrime->type != AlgorithmTypeSlice)
        puts("Error: M' processed invalid type.");
    if (mPrime->contents.slice.layer != 'M')
        puts("Error: M' processed invalid slice.");
    if (!mPrime->inverseFlag)
        puts("Error: M' did not process the inverse.");
    if (mPrime->power != 1)
        puts("Error: M' processed the wrong power.");
    algorithm_free(mPrime);
    
    // test 3Uw2
    Algorithm * upWide = algorithm_for_token("13Uw2");
    if (upWide->type != AlgorithmTypeWideTurn)
        puts("Error: 13Uw2 processed invalid type.");
    if (upWide->contents.wideTurn.face != 'U')
        puts("Error: 13Uw2 processed invalid face.");
    if (upWide->contents.wideTurn.numLayers != 13)
        puts("Error: 13Uw2 processed invalid layer count.");
    if (upWide->inverseFlag)
        puts("Error: 13Uw2 processed an inverse.");
    if (upWide->power != 2)
        puts("Error: 13Uw2 processed the wrong power.");
    algorithm_free(upWide);
    
    test_completed();
}
