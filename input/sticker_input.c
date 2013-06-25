#include "sticker_input.h"

static int _read_face(StickerMap * map, int face);
static int _validate_edges(Cuboid * cuboid);
static int _validate_centers(Cuboid * cuboid);
static int _validate_corners(Cuboid * cuboid);

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
    if (!convert_sm_to_cb(c, map)) {
        fprintf(stderr, "Error: failed to convert sticker data to piece array.\n");
        stickermap_free(map);
        cuboid_free(c);
        return NULL;
    }
    stickermap_free(map);
    
    if (!_validate_corners(c)) {
        fprintf(stderr, "Error: invalid corners.\n");
        cuboid_free(c);
        return NULL;
    }
    if (!_validate_centers(c)) {
        fprintf(stderr, "Error: invalid centers.\n");
        cuboid_free(c);
        return NULL;
    }
    if (!_validate_edges(c)) {
        fprintf(stderr, "Error: invalid edges.\n");
        cuboid_free(c);
        return NULL;
    }
    
    return c;
}

static int _read_face(StickerMap * map, int face) {
    int numBytes = stickermap_count_stickers_for_face(map, face);
    uint32_t index = stickermap_face_start_index(map, face);
    int i, numRead = 0, hitEnter = 0;
    for (i = 0; i < numBytes; i++) {
        int chr = fgetc(stdin);
        if (chr == EOF) return 0;
        if (chr == '\r' || chr == ' ') {
            i--;
            continue;
        }
        if (chr == '\n') {
            hitEnter = 1;
            break;
        }
        int color = chr - '1' + 1;
        if (color < 1 || color > 6) {
            fprintf(stderr, "Error: invalid sticker color `%c`.\n", chr);
            return 0;
        }
        map->stickers[index + i] = color;
        numRead++;
    }
    for (i = numRead; i < numBytes; i++) {
        map->stickers[index + i] = face;
    }
    
    // suck in a return if we didn't get one
    if (!hitEnter) {
        while (1) {
            int c = fgetc(stdin);
            if (c == EOF) break;
            if (c == '\n') break;
        }
    }
    return 1;
}

static int _validate_edges(Cuboid * cuboid) {
    int i, j, edgeCount = cuboid_count_edges(cuboid);
    for (i = 0; i < 12; i++) {
        int expected = cuboid_count_edges_for_dedge(cuboid, i);
        int count = 0;
        for (j = 0; j < edgeCount; j++) {
            CuboidEdge edge = cuboid->edges[j];
            if (edge.dedgeIndex == i) {
                count ++;
            }
        }
        if (count != expected) {
            return 0;
        }
    }
    return 1;
}

static int _validate_centers(Cuboid * cuboid) {
    int i, j, centerCount = cuboid_count_centers(cuboid);
    for (i = 1; i <= 6; i++) {
        int expected = cuboid_count_centers_for_face(cuboid, i);
        int count = 0;
        for (j = 0; j < centerCount; j++) {
            CuboidCenter center = cuboid->centers[j];
            if (center.side == i) {
                count ++;
            }
        }
        if (count != expected) {
            return 0;
        }
    }
    return 1;
}

static int _validate_corners(Cuboid * cuboid) {
    int flags = 0, i;
    for (i = 0; i < 8; i++) {
        flags |= (1 << (cuboid->corners[i].index));
    }
    return (flags == 0xff);
}
