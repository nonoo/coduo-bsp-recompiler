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

void *lump_data[NUM_LUMPS];
size_t lump_sizes[NUM_LUMPS];

void append_data(int lump_id, const void *data, size_t size) {
    lump_data[lump_id] = realloc(lump_data[lump_id], lump_sizes[lump_id] + size);
    memcpy((uint8_t *)lump_data[lump_id] + lump_sizes[lump_id], data, size);
    lump_sizes[lump_id] += size;
}

float hex_to_float(uint32_t u) {
    union { float f; uint32_t u; } un;
    un.u = u;
    return un.f;
}

int main(int argc, char **argv) {
    FILE *f;
    if (argc < 2) {
        f = stdin;
    } else {
        f = fopen(argv[1], "r");
        if (!f) { perror("fopen"); return 1; }
    }

    char line[4096];
    char pushed_line[4096];
    int has_pushed = 0;
    int current_lump = -1;
    uint32_t entity_size_expected = 0;
    
    while (1) {
        if (has_pushed) {
            strcpy(line, pushed_line);
            has_pushed = 0;
        } else {
            if (!fgets(line, sizeof(line), f)) break;
        }

        if (strncmp(line, "--- Lump", 8) == 0) {
            sscanf(line, "--- Lump %d:", &current_lump);
            if (current_lump == 29) {
                // If explicit size was present (old format), we might parse it, but we support implicit size now.
                // sscanf(line, "--- Lump 29: Entities (Size: %u bytes)", &entity_size_expected);
            }
            continue;
        }
        if (current_lump == -1) continue;
        if (current_lump != 29 && (line[0] == '\n' || line[0] == '\r')) continue;

        switch (current_lump) {
            case 0: {
                dmaterial_t mat; memset(&mat, 0, sizeof(mat));
                char *nptr = strstr(line, "Name: ");
                if (nptr) {
                    nptr += 6;
                    char *fptr = strstr(nptr, ", Flags: ");
                    if (fptr) {
                        int len = fptr - nptr; if (len > 63) len = 63;
                        strncpy(mat.name, nptr, len);
                        sscanf(fptr, ", Flags: 0x%X 0x%X", &mat.flags[0], &mat.flags[1]);
                        append_data(0, &mat, sizeof(mat));
                    }
                }
                break;
            }
            case 2: {
                dplane_t p; int idx;
                uint32_t n1, n2, n3, d;
                if (sscanf(line, "[%d] Normal: (%X, %X, %X), Dist: %X", &idx, &n1, &n2, &n3, &d) == 5) {
                    p.normal[0] = hex_to_float(n1); p.normal[1] = hex_to_float(n2); p.normal[2] = hex_to_float(n3); p.dist = hex_to_float(d);
                    append_data(2, &p, sizeof(p));
                }
                break;
            }
            case 3: {
                dbrushside_t bs; int idx;
                if (sscanf(line, "[%d] Union: %X, MatID: %u", &idx, &bs.u.raw, &bs.material_id) == 3)
                    append_data(3, &bs, sizeof(bs));
                break;
            }
            case 4: {
                dbrush_t b; int idx; uint32_t s, m;
                if (sscanf(line, "[%d] Sides: %u, MatID: %u", &idx, &s, &m) == 3) {
                    b.num_sides = s; b.material_id = m; append_data(4, &b, sizeof(b));
                }
                break;
            }
            case 6: {
                dtrianglesoup_t t; int idx; uint32_t m, o, vlen, trilen;
                if (sscanf(line, "[%d] MatID: %u, Order: %u, VOff: %u, VLen: %u, TriLen: %u, TriOff: %u", &idx, &m, &o, &t.vertex_offset, &vlen, &trilen, &t.triangle_offset) == 7) {
                    t.material_id = m; t.draw_order = o; t.vertex_length = vlen; t.triangle_length = trilen;
                    append_data(6, &t, sizeof(t));
                }
                break;
            }
            case 7: {
                ddrawvertex_t v; int idx;
                uint32_t p1, p2, p3, n1, n2, n3, uv1, uv2, lm1, lm2;
                if (sscanf(line, "[%d] Pos: (%X %X %X) Norm: (%X %X %X) Color: %X UV: (%X %X) LM: (%X %X)", 
                    &idx, &p1, &p2, &p3, &n1, &n2, &n3, &v.rgba, &uv1, &uv2, &lm1, &lm2) == 12) {
                    v.pos[0] = hex_to_float(p1); v.pos[1] = hex_to_float(p2); v.pos[2] = hex_to_float(p3);
                    v.normal[0] = hex_to_float(n1); v.normal[1] = hex_to_float(n2); v.normal[2] = hex_to_float(n3);
                    v.extra[0] = hex_to_float(uv1); v.extra[1] = hex_to_float(uv2); v.extra[2] = hex_to_float(lm1); v.extra[3] = hex_to_float(lm2);
                    append_data(7, &v, sizeof(v));
                }
                break;
            }
            case 8: {
                ddrawindex_t di; int idx; uint32_t v1, v2, v3;
                if (sscanf(line, "[%d] %u %u %u", &idx, &v1, &v2, &v3) == 4) {
                    di.v[0] = v1; di.v[1] = v2; di.v[2] = v3; append_data(8, &di, sizeof(di));
                }
                break;
            }
            case 20: {
                dnode_t n; int idx;
                if (sscanf(line, "[%d] Plane: %d, Children: %d %d, Mins: (%d %d %d), Maxs: (%d %d %d)", &idx, &n.plane_idx, &n.children[0], &n.children[1], &n.mins[0], &n.mins[1], &n.mins[2], &n.maxs[0], &n.maxs[1], &n.maxs[2]) == 10)
                    append_data(20, &n, sizeof(n));
                break;
            }
            case 21: {
                dleaf_t l; int idx;
                if (sscanf(line, "[%d] Data: %d %d %d %d %d %d %d %d %d", &idx, &l.data[0], &l.data[1], &l.data[2], &l.data[3], &l.data[4], &l.data[5], &l.data[6], &l.data[7], &l.data[8]) == 10)
                    append_data(21, &l, sizeof(l));
                break;
            }
            case 22: {
                dleafbrush_t lb; int idx;
                if (sscanf(line, "[%d] BrushIdx: %d", &idx, &lb.brush_idx) == 2)
                    append_data(22, &lb, sizeof(lb));
                break;
            }
            case 27: {
                dmodel_t m; int idx;
                uint32_t mn1, mn2, mn3, mx1, mx2, mx3;
                if (sscanf(line, "[%d] Mins: (%X %X %X), Maxs: (%X %X %X)", &idx, &mn1, &mn2, &mn3, &mx1, &mx2, &mx3) == 7) {
                    m.mins[0] = hex_to_float(mn1); m.mins[1] = hex_to_float(mn2); m.mins[2] = hex_to_float(mn3);
                    m.maxs[0] = hex_to_float(mx1); m.maxs[1] = hex_to_float(mx2); m.maxs[2] = hex_to_float(mx3);
                    
                    // Read subsequent detail lines
                    m.trianglesoups_offset = m.trianglesoups_size = m.collisionaabbs_offset = m.collisionaabbs_size = m.brushes_offset = m.brushes_size = 0;
                    
                    char buf[512];
                    for (int i=0; i<3; i++) {
                        // We must be careful not to consume a header line.
                        // We can't peek easily with fgets/fseek on stdin.
                        // But we can use our pushed_line logic.
                        if (!fgets(buf, sizeof(buf), f)) break;
                        
                        if (strncmp(buf, "--- Lump", 8) == 0) {
                            strcpy(pushed_line, buf);
                            has_pushed = 1;
                            break;
                        }
                        
                        if (strstr(buf, "TriSoups:")) sscanf(buf, "    TriSoups: off=%u size=%u", &m.trianglesoups_offset, &m.trianglesoups_size);
                        else if (strstr(buf, "ColAABBs:")) sscanf(buf, "    ColAABBs: off=%u size=%u", &m.collisionaabbs_offset, &m.collisionaabbs_size);
                        else if (strstr(buf, "Brushes:")) sscanf(buf, "    Brushes:  off=%u size=%u", &m.brushes_offset, &m.brushes_size);
                        // If it's none of these and not a header, we assume it's garbage or extra info and ignore it (consume it).
                    }
                    append_data(27, &m, sizeof(m));
                }
                break;
            }
            case 29: {
                char buf[4096];
                strcpy(buf, line);
                
                // We need to peek next line to see if it is a header.
                // If it is a header, we push it back and finish this lump.
                
                char next_line[4096];
                if (fgets(next_line, sizeof(next_line), f)) {
                    if (strncmp(next_line, "--- Lump", 8) == 0) {
                        strcpy(pushed_line, next_line);
                        has_pushed = 1;
                        
                        // Current 'buf' is the last line.
                        size_t len = strlen(buf);
                        if (len > 0 && buf[len-1] == '\n') len--;
                        append_data(29, buf, len);
                        
                        uint8_t null_term = 0;
                        append_data(29, &null_term, 1);
                        
                        current_lump = -1;
                    } else {
                        // Next line is valid data.
                        // Wait, we consumed next_line. We must process 'buf' fully, 
                        // and then in NEXT iteration process 'next_line'.
                        // But we are in a switch case.
                        // We can push 'next_line' back.
                        strcpy(pushed_line, next_line);
                        has_pushed = 1;
                        
                        // Just append 'buf'
                        append_data(29, buf, strlen(buf));
                    }
                } else {
                    // EOF
                    size_t len = strlen(buf);
                    if (len > 0 && buf[len-1] == '\n') len--;
                    append_data(29, buf, len);
                    uint8_t null_term = 0;
                    append_data(29, &null_term, 1);
                    current_lump = -1;
                }
                break;
            }
            default: {
                char *start = line;
                char *h = strstr(line, "HexData:");
                if (h) start = h + 8;
                for (int i=0; start[i]; i++) {
                    if (isxdigit(start[i]) && isxdigit(start[i+1])) {
                        unsigned int b;
                        sscanf(start+i, "%02X", &b);
                        uint8_t byte = (uint8_t)b;
                        append_data(current_lump, &byte, 1);
                        i++;
                    }
                }
                break;
            }
        }
    }
    if (f != stdin) fclose(f);

    FILE *out = fopen("output.bsp", "wb");
    dheader_t header; memcpy(header.magic, "IBSP", 4); header.version = COD1_VERSION;
    uint32_t off = sizeof(dheader_t);
    for (int i=0; i<NUM_LUMPS; i++) {
        header.lumps[i].offset = off;
        header.lumps[i].size = lump_sizes[i];
        off += lump_sizes[i];
        int pad = (off % 4) ? (4 - (off % 4)) : 0; off += pad;
    }
    fwrite(&header, sizeof(header), 1, out);
    for (int i=0; i<NUM_LUMPS; i++) {
        if (lump_sizes[i] > 0) {
            fwrite(lump_data[i], lump_sizes[i], 1, out);
            int pad = (ftell(out) % 4) ? (4 - (ftell(out) % 4)) : 0;
            for (int p=0; p<pad; p++) fputc(0, out);
        }
        free(lump_data[i]);
    }
    fclose(out);
    return 0;
}