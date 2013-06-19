#include "sticker_input.h"

static int _read_face(StickerMap * map, int face);

StickerMap * input_stickermap(CuboidDimensions dims) {
    StickerMap * map = stickermap_create(dims);
    
    const char * prompts[] = {
        "front face",
        "back face",
        "top face",
        "bottom face",
        "right face",
        "left face"
    };
    int i, j;
    for (i = 1; i <= 6; i++) {
        const char * prompt = prompts[i - 1];
        int len = 11 - strlen(prompt);
        if (len < 0) len = 0;
        
        for (j = 0; j < len; j++) {
            printf(" ");
        }
        
        printf("Enter %s: ", prompt);
        fflush(stdout);
        if (!_read_face(map, i)) {
            stickermap_free(map);
            return NULL;
        }
    }
    
    return map;
}

Cuboid * input_stickermap_to_cuboid(CuboidDimensions dims) {
    StickerMap * map = input_stickermap(dims);
    if (!map) return NULL;
    
    Cuboid * c = cuboid_create(map->dimensions);
    convert_sm_to_cb(c, map);
    stickermap_free(map);
    return c;
}

static int _read_face(StickerMap * map, int face) {
    int numBytes = stickermap_count_stickers_for_face(map, face);
    uint32_t index = stickermap_face_start_index(map, face);
    int i;
    for (i = 0; i < numBytes; i++) {
        int chr = fgetc(stdin);
        if (chr == '\r' || chr == ' ') {
            i--;
            continue;
        }
        if (chr == '\n') break;
        int color = chr - '1' + 1;
        if (color < 1 || color > 6) {
            fprintf(stderr, "Error: invalid sticker color `%c`.\n", chr);
            return 0;
        }
        map->stickers[index + i] = color;
    }
    for (i = i; i < numBytes; i++) {
        map->stickers[index + i] = face;
    }
    return 1;
}
