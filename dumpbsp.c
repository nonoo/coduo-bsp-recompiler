#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define COD1_VERSION 59
#define NUM_LUMPS 33

typedef struct { uint32_t size; uint32_t offset; } lump_t;
typedef struct { char magic[4]; uint32_t version; lump_t lumps[NUM_LUMPS]; } dheader_t;

typedef struct { char name[64]; uint32_t flags[2]; } dmaterial_t;
typedef struct { float normal[3]; float dist; } dplane_t;
typedef struct { union { uint32_t plane_idx; float distance; uint32_t raw; } u; uint32_t material_id; } dbrushside_t;
typedef struct { uint16_t num_sides; uint16_t material_id; } dbrush_t;
typedef struct { uint16_t material_id; uint16_t draw_order; uint32_t vertex_offset; uint16_t vertex_length; uint16_t triangle_length; uint32_t triangle_offset; } dtrianglesoup_t;
typedef struct { float pos[3]; float normal[3]; uint32_t rgba; float extra[4]; } ddrawvertex_t;
typedef struct { uint16_t v[3]; } ddrawindex_t;
typedef struct { int plane_idx; int children[2]; int mins[3]; int maxs[3]; } dnode_t;
typedef struct { int data[9]; } dleaf_t;
typedef struct { int brush_idx; } dleafbrush_t;
typedef struct { float mins[3]; float maxs[3]; uint32_t trianglesoups_offset; uint32_t trianglesoups_size; uint32_t collisionaabbs_offset; uint32_t collisionaabbs_size; uint32_t brushes_offset; uint32_t brushes_size; } dmodel_t;

uint32_t float_to_hex(float f) {
    union { float f; uint32_t u; } u;
    u.f = f;
    return u.u;
}

void print_lump_0(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dmaterial_t);
    printf("--- Lump %d: Materials ---\n", id);
    if (count == 0) return;
    dmaterial_t *data = (dmaterial_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        char sanitized[65] = {0};
        for (int j = 0; j < 64; j++) sanitized[j] = (data[i].name[j] >= 32 && data[i].name[j] <= 126) ? data[i].name[j] : (data[i].name[j] ? '.' : 0);
        printf("[%d] Name: %s, Flags: 0x%X 0x%X\n", i, sanitized, data[i].flags[0], data[i].flags[1]);
    }
    free(data);
    printf("\n");
}

void print_lump_2(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dplane_t);
    printf("--- Lump %d: Planes ---\n", id);
    if (count == 0) return;
    dplane_t *data = (dplane_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Normal: (%08X, %08X, %08X), Dist: %08X\n", i, 
            float_to_hex(data[i].normal[0]), float_to_hex(data[i].normal[1]), float_to_hex(data[i].normal[2]), float_to_hex(data[i].dist));
    }
    free(data);
    printf("\n");
}

void print_lump_3(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dbrushside_t);
    printf("--- Lump %d: Brushsides ---\n", id);
    if (count == 0) return;
    dbrushside_t *data = (dbrushside_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Union: %08X, MatID: %u\n", i, data[i].u.raw, data[i].material_id);
    }
    free(data);
    printf("\n");
}

void print_lump_4(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dbrush_t);
    printf("--- Lump %d: Brushes ---\n", id);
    if (count == 0) return;
    dbrush_t *data = (dbrush_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Sides: %u, MatID: %u\n", i, data[i].num_sides, data[i].material_id);
    }
    free(data);
    printf("\n");
}

void print_lump_6(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dtrianglesoup_t);
    printf("--- Lump %d: TriangleSoups ---\n", id);
    if (count == 0) return;
    dtrianglesoup_t *data = (dtrianglesoup_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] MatID: %u, Order: %u, VOff: %u, VLen: %u, TriLen: %u, TriOff: %u\n", i, 
            data[i].material_id, data[i].draw_order, data[i].vertex_offset, data[i].vertex_length, data[i].triangle_length, data[i].triangle_offset);
    }
    free(data);
    printf("\n");
}

void print_lump_7(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(ddrawvertex_t);
    printf("--- Lump %d: DrawVertices ---\n", id);
    if (count == 0) return;
    ddrawvertex_t *data = (ddrawvertex_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Pos: (%08X %08X %08X) Norm: (%08X %08X %08X) Color: %08X UV: (%08X %08X) LM: (%08X %08X)\n", i,
            float_to_hex(data[i].pos[0]), float_to_hex(data[i].pos[1]), float_to_hex(data[i].pos[2]),
            float_to_hex(data[i].normal[0]), float_to_hex(data[i].normal[1]), float_to_hex(data[i].normal[2]),
            data[i].rgba,
            float_to_hex(data[i].extra[0]), float_to_hex(data[i].extra[1]), float_to_hex(data[i].extra[2]), float_to_hex(data[i].extra[3]));
    }
    free(data);
    printf("\n");
}

void print_lump_8(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(ddrawindex_t);
    printf("--- Lump %d: DrawIndices ---\n", id);
    if (count == 0) return;
    ddrawindex_t *data = (ddrawindex_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] %u %u %u\n", i, data[i].v[0], data[i].v[1], data[i].v[2]);
    }
    free(data);
    printf("\n");
}

void print_lump_20(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dnode_t);
    printf("--- Lump %d: Nodes ---\n", id);
    if (count == 0) return;
    dnode_t *data = (dnode_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Plane: %d, Children: %d %d, Mins: (%d %d %d), Maxs: (%d %d %d)\n", i,
            data[i].plane_idx, data[i].children[0], data[i].children[1],
            data[i].mins[0], data[i].mins[1], data[i].mins[2],
            data[i].maxs[0], data[i].maxs[1], data[i].maxs[2]);
    }
    free(data);
    printf("\n");
}

void print_lump_21(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dleaf_t);
    printf("--- Lump %d: Leaves ---\n", id);
    if (count == 0) return;
    dleaf_t *data = (dleaf_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Data: ", i);
        for (int k=0; k<9; k++) printf("%d ", data[i].data[k]);
        printf("\n");
    }
    free(data);
    printf("\n");
}

void print_lump_22(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dleafbrush_t);
    printf("--- Lump %d: LeafBrushes ---\n", id);
    if (count == 0) return;
    dleafbrush_t *data = (dleafbrush_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] BrushIdx: %d\n", i, data[i].brush_idx);
    }
    free(data);
    printf("\n");
}

void print_lump_27(FILE *f, lump_t l, int id) {
    int count = l.size / sizeof(dmodel_t);
    printf("--- Lump %d: Models ---\n", id);
    if (count == 0) return;
    dmodel_t *data = (dmodel_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    for (int i = 0; i < count; i++) {
        printf("[%d] Mins: (%08X %08X %08X), Maxs: (%08X %08X %08X)\n", i,
               float_to_hex(data[i].mins[0]), float_to_hex(data[i].mins[1]), float_to_hex(data[i].mins[2]),
               float_to_hex(data[i].maxs[0]), float_to_hex(data[i].maxs[1]), float_to_hex(data[i].maxs[2]));
        printf("    TriSoups: off=%u size=%u\n", data[i].trianglesoups_offset, data[i].trianglesoups_size);
        printf("    ColAABBs: off=%u size=%u\n", data[i].collisionaabbs_offset, data[i].collisionaabbs_size);
        printf("    Brushes:  off=%u size=%u\n", data[i].brushes_offset, data[i].brushes_size);
    }
    free(data);
    printf("\n");
}

void print_lump_29(FILE *f, lump_t l, int id) {
    printf("--- Lump %d: Entities ---\n", id);
    if (l.size == 0) return;
    char *data = (char *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    fwrite(data, l.size, 1, stdout);
    printf("\n");
    free(data);
}

void print_unknown(FILE *f, lump_t l, int id) {
    printf("--- Lump %d: Unknown ---\n", id);
    if (l.size == 0) return;
    uint8_t *data = (uint8_t *)malloc(l.size);
    fseek(f, l.offset, SEEK_SET);
    fread(data, l.size, 1, f);
    printf("HexData:");
    for (uint32_t i = 0; i < l.size; i++) {
        if (i % 32 == 0) printf("\n");
        printf("%02X", data[i]);
    }
    printf("\n\n");
    free(data);
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <file.bsp>\n", argv[0]); return 1; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    dheader_t header;
    if (fread(&header, sizeof(dheader_t), 1, f) != 1) { fprintf(stderr, "Error reading header\n"); fclose(f); return 1; }
    if (memcmp(header.magic, "IBSP", 4) != 0) { fprintf(stderr, "Invalid magic number\n"); fclose(f); return 1; }
    // printf("Processing %s (Version %u)\n\n", argv[1], header.version);

    for (int i = 0; i < NUM_LUMPS; i++) {
        if (header.lumps[i].size == 0) { printf("--- Lump %d: Empty ---\n\n", i); continue; }
        switch (i) {
            case 0: print_lump_0(f, header.lumps[i], i); break;
            case 2: print_lump_2(f, header.lumps[i], i); break;
            case 3: print_lump_3(f, header.lumps[i], i); break;
            case 4: print_lump_4(f, header.lumps[i], i); break;
            case 6: print_lump_6(f, header.lumps[i], i); break;
            case 7: print_lump_7(f, header.lumps[i], i); break;
            case 8: print_lump_8(f, header.lumps[i], i); break;
            case 20: print_lump_20(f, header.lumps[i], i); break;
            case 21: print_lump_21(f, header.lumps[i], i); break;
            case 22: print_lump_22(f, header.lumps[i], i); break;
            case 27: print_lump_27(f, header.lumps[i], i); break;
            case 29: print_lump_29(f, header.lumps[i], i); break;
            default: print_unknown(f, header.lumps[i], i); break;
        }
    }
    fclose(f);
    return 0;
}