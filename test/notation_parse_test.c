#include "test.h"
#include "notation/parser.h"

void test_tokens();
void test_algorithm();

int main() {
    test_tokens();
    test_algorithm();
    
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
    
    Algorithm * rotation = algorithm_for_token("x'3");
    if (rotation->type != AlgorithmTypeRotation)
        puts("Error: x'3 processed invalid type.");
    if (rotation->contents.rotation.axis != 'x')
        puts("Error: x'3 processed invalid axis.");
    if (!rotation->inverseFlag)
        puts("Error: x'3 processed invalid inverse flag.");
    if (rotation->power != 3)
        puts("Error: x'3 processed invalid power.");
    algorithm_free(rotation);
    
    test_completed();
}

void test_algorithm() {
    test_initiated("algorithm parser");
    
    Algorithm * algo = algorithm_for_string("R2 U' 12Rw15 (R z2)2 R' S'");
    if (algorithm_container_count(algo) != 6) {
        puts("Error: invalid algorithm token count.");
    }
    
    Algorithm * algo1 = algorithm_container_get(algo, 0);
    if (algo1->type != AlgorithmTypeWideTurn ||
        algo1->contents.wideTurn.face != 'R' ||
        algo1->contents.wideTurn.numLayers != 1 ||
        algo1->inverseFlag != 0 ||
        algo1->power != 2) {
        puts("Error: invalid token at index 0.");
    }
    algo1 = algorithm_container_get(algo, 1);
    if (algo1->type != AlgorithmTypeWideTurn ||
        algo1->contents.wideTurn.face != 'U' ||
        algo1->contents.wideTurn.numLayers != 1 ||
        algo1->inverseFlag != 1 ||
        algo1->power != 1) {
        puts("Error: invalid token at index 1.");
    }
    algo1 = algorithm_container_get(algo, 2);
    if (algo1->type != AlgorithmTypeWideTurn ||
        algo1->contents.wideTurn.face != 'R' ||
        algo1->contents.wideTurn.numLayers != 12 ||
        algo1->inverseFlag != 0 ||
        algo1->power != 15) {
        puts("Error: invalid token at index 2.");
    }
    algo1 = algorithm_container_get(algo, 4);
    if (algo1->type != AlgorithmTypeWideTurn ||
        algo1->contents.wideTurn.face != 'R' ||
        algo1->contents.wideTurn.numLayers != 1 ||
        algo1->inverseFlag != 1 ||
        algo1->power != 1) {
        puts("Error: invalid token at index 4.");
    }
    algo1 = algorithm_container_get(algo, 5);
    if (algo1->type != AlgorithmTypeSlice ||
        algo1->contents.slice.layer != 'S' ||
        algo1->inverseFlag != 1 ||
        algo1->power != 1) {
        puts("Error: invalid token at index 5.");
    }
    
    Algorithm * nested = algorithm_container_get(algo, 3);
    if (nested->type != AlgorithmTypeContainer) {
        puts("Error: invalid type for nested subexpression at index 3.");
    }
    if (algorithm_container_count(nested) != 2) {
        puts("Error: invalid count for nested subexpression.");
    }
    if (nested->power != 2) {
        puts("Error: invalid power on nested subexpression.");
    }
    algo1 = algorithm_container_get(nested, 0);
    if (algo1->type != AlgorithmTypeWideTurn ||
        algo1->contents.wideTurn.face != 'R' ||
        algo1->contents.wideTurn.numLayers != 1 ||
        algo1->inverseFlag != 0 ||
        algo1->power != 1) {
        puts("Error: invalid token at subexpression index 0.");
    }
    
    algo1 = algorithm_container_get(nested, 1);
    if (algo1->type != AlgorithmTypeRotation ||
        algo1->contents.rotation.axis != 'z' ||
        algo1->inverseFlag != 0 ||
        algo1->power != 2) {
        puts("Error: invalid token at subexpression index 1.");
    }
    
    algorithm_free(algo);
    
    test_completed();
}
