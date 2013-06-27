#include "center_index.h"

typedef struct {
    uint8_t centerFlags[12];
    CuboidDimensions dims;
    uint8_t compact;
    uint8_t oppCenters; // does not distinguish between 1 and 2, 3 and 4, etc.
} CenterIndexData;

static int _process_center_flags(uint8_t * flagsOut, const char * str);
static void _find_and_copy_center(CenterIndexData * data, const Cuboid * cb,
                                  uint8_t * out, int face);
static int _centers_can_be_one_byte(CenterIndexData * data);

CLArgumentList * center_index_default_arguments() {
    CLArgumentList * list = cl_argument_list_new();
    cl_argument_list_add(list, cl_argument_new_string("centers", "111111"));
    cl_argument_list_add(list, cl_argument_new_flag("oppcenters", 0));
    return list;
}

int center_index_initialize(HSParameters params, CLArgumentList * arguments, void ** userData) {
    int index = cl_argument_list_find(arguments, "centers");
    if (index < 0) return 0;
    CLArgument * arg = cl_argument_list_get(arguments, index);
    
    CenterIndexData * data = (CenterIndexData *)malloc(sizeof(CenterIndexData));
    data->dims = params.symmetries.dims;
    if (!_process_center_flags(data->centerFlags, arg->contents.string.value)) {
        free(data);
        return 0;
    }
    data->compact = _centers_can_be_one_byte(data);
    
    index = cl_argument_list_find(arguments, "oppcenters");
    if (index < 0) {
        free(data);
        return 0;
    }
    arg = cl_argument_list_get(arguments, index);
    data->oppCenters = arg->contents.flag.boolValue;
    
    *userData = data;
    return 1;
}

int center_index_load(HSParameters params, FILE * fp, void ** userData) {
    uint8_t saved;
    uint8_t oppCenters;
    if (!load_uint8(&saved, fp)) return 0;
    if (!load_uint8(&oppCenters, fp)) return 0;
    CenterIndexData * data = (CenterIndexData *)malloc(sizeof(CenterIndexData));
    data->oppCenters = oppCenters;
    int i;
    for (i = 0; i < 6; i++) {
        data->centerFlags[i] = (saved >> i) & 1;
    }
    if (!load_cuboid_dimensions(&data->dims, fp)) {
        free(data);
        return 0;
    }
    if (!center_index_supports_dimensions(data, params.symmetries.dims)) {
        free(data);
        return 0;
    }
    data->compact = _centers_can_be_one_byte(data);
    *userData = data;
    return 1;
}

void center_index_save(void * userData, FILE * fp) {
    CenterIndexData * data = (CenterIndexData *)userData;
    uint8_t flags = 0;
    int i;
    for (i = 0; i < 6; i++) {
        if (data->centerFlags[i]) {
            flags |= (1 << i);
        }
    }
    save_uint8(flags, fp);
    save_uint8(data->oppCenters, fp);
    save_cuboid_dimensions(data->dims, fp);
}

int center_index_supports_dimensions(void * userData, CuboidDimensions dims) {
    CenterIndexData * data = (CenterIndexData *)userData;
    return cuboid_dimensions_equal(dims, data->dims);
}

int center_index_data_size(void * userData) {
    CenterIndexData * data = (CenterIndexData *)userData;
    CuboidDimensions dims = data->dims;
    int frontCount = (dims.x - 2) * (dims.y - 2);
    int rightCount = (dims.z - 2) * (dims.y - 2);
    int topCount = (dims.x - 2) * (dims.z - 2);
    int i, count = 0;
    for (i = 1; i <= 6; i++) {
        if (!data->centerFlags[i - 1]) continue;
        if (i == 1 || i == 2) {
            count += frontCount;
        } else if (i == 3 || i == 4) {
            count += topCount;
        } else {
            count += rightCount;
        }
    }
    return count * (data->compact ? 1 : 2);
}

int center_index_angle_count(void * userData) {
    return 1;
}

int center_index_angles_are_equivalent(void * userData, int a1, int a2) {
    puts("error: center_index_angles_are_equivalent() should never be called.");
    abort();
}

void center_index_get_data(void * userData, const Cuboid * cb, uint8_t * out, int angle) {
    CenterIndexData * data = (CenterIndexData *)userData;
    int dataIndex = 0, face;
    for (face = 1; face <= 6; face++) {
        if (!data->centerFlags[face - 1]) continue;
        
        int centerCount = cuboid_count_centers_for_face(cb, face);
        _find_and_copy_center(data, cb, &out[dataIndex], face);
        dataIndex += centerCount * (data->compact ? 1 : 2);
    }
}

void center_index_completed(void * userData) {
    CenterIndexData * data = (CenterIndexData *)userData;
    free(data);
}

/***********
 * Private *
 ***********/

static int _process_center_flags(uint8_t * flagsOut, const char * str) {
    if (strlen(str) != 6) return 0;
    int i;
    for (i = 0; i < 6; i++) {
        if (str[i] == '1') {
            flagsOut[i] = 1;
        } else if (str[i] == '0') {
            flagsOut[i] = 0;
        } else {
            return 0;
        }
    }
    return 1;
}

static void _find_and_copy_center(CenterIndexData * data, const Cuboid * cb,
                                  uint8_t * out, int theFace) {
    int face, index, gottenCount = 0;
    int expected = cuboid_count_centers_for_face(cb, theFace);
    for (face = 1; face <= 6; face++) {
        int centerCount = cuboid_count_centers_for_face(cb, face);
        for (index = 0; index < centerCount; index++) {
            int absIndex = cuboid_center_index(cb, face, index);
            CuboidCenter c = cb->centers[absIndex];
            if (c.side == theFace) {
                int useFace = face;
                if (data->oppCenters) {
                    useFace = (useFace - 1) / 2;
                }
                if (data->compact) {
                    out[gottenCount] = useFace | (index << 3);
                } else {
                    out[gottenCount * 2] = useFace;
                    out[1 + gottenCount * 2] = index;
                }
                gottenCount++;
                if (gottenCount >= expected) break;
            }
        }
        if (gottenCount >= expected) break;
    }
    assert(gottenCount == expected);
}

static int _centers_can_be_one_byte(CenterIndexData * data) {
    CuboidDimensions dims = data->dims;
    int frontCount = (dims.x - 2) * (dims.y - 2);
    int rightCount = (dims.z - 2) * (dims.y - 2);
    int topCount = (dims.x - 2) * (dims.z - 2);
    return (frontCount < 32 && rightCount < 32 && topCount < 32);
}
