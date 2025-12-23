#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int start_brush;
    int end_brush;
} range_t;

typedef struct {
    long long start_bs_idx; // Starting index in Lump 3 (Brushsides)
    long long count;        // Number of brushsides
    int brush_idx;          // The brush index this segment belongs to
} brush_info_t;

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dump_file> <brush_index_or_range> [output_start_index]\n", argv[0]);
        fprintf(stderr, "Range format: start-end (inclusive)\n");
        return 1;
    }

    const char *filename = argv[1];
    
    // Parse Range
    int start_brush, end_brush;
    char *dash = strchr(argv[2], '-');
    if (dash) {
        *dash = '\0';
        start_brush = atoi(argv[2]);
        end_brush = atoi(dash + 1);
    } else {
        start_brush = atoi(argv[2]);
        end_brush = start_brush;
    }

    long long print_index_offset = 0;
    if (argc > 3) {
        print_index_offset = atoll(argv[3]);
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    // Prepare to store info for all brushes in range
    int count_in_range = end_brush - start_brush + 1;
    if (count_in_range <= 0) {
        fprintf(stderr, "Invalid range.\n");
        fclose(f);
        return 1;
    }

    brush_info_t *targets = malloc(sizeof(brush_info_t) * count_in_range);
    for(int i=0; i<count_in_range; i++) {
        targets[i].brush_idx = start_brush + i;
        targets[i].start_bs_idx = -1;
        targets[i].count = 0;
    }

    char line[4096];
    int in_lump_4 = 0;
    int current_brush_idx = 0;
    long long current_brushside_accumulator = 0;
    int found_count = 0;

    // Pass 1: Find target brushes in Lump 4
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "--- Lump", 8) == 0) {
            int lump_id = -1;
            if (sscanf(line, "--- Lump %d:", &lump_id) == 1) {
                if (lump_id == 4) in_lump_4 = 1;
                else in_lump_4 = 0;
            }
            continue;
        }

        if (in_lump_4) {
            if (line[0] == '[') {
                int sides = 0;
                char *sides_ptr = strstr(line, "Sides: ");
                if (sides_ptr) {
                    if (sscanf(sides_ptr, "Sides: %d", &sides) == 1) {
                        if (current_brush_idx >= start_brush && current_brush_idx <= end_brush) {
                            int local_idx = current_brush_idx - start_brush;
                            targets[local_idx].start_bs_idx = current_brushside_accumulator;
                            targets[local_idx].count = sides;
                            found_count++;
                        }
                        
                        current_brushside_accumulator += sides;
                        current_brush_idx++;
                        
                        // Optimization: if we passed the range, we can stop scanning Lump 4
                        if (current_brush_idx > end_brush) break;
                    }
                }
            }
        }
    }

    if (found_count < count_in_range) {
        // Just warning, maybe user asked for out of range
        // fprintf(stderr, "Warning: Only found %d out of %d requested brushes.\n", found_count, count_in_range);
    }

    // Pass 2: Find Lump 3 and print specific lines
    rewind(f);
    int in_lump_3 = 0;
    long long current_bs_idx = 0;
    long long current_print_idx = print_index_offset;
    
    // We need to print them in order of brush index 
    // BUT the brushsides in Lump 3 are also sequential corresponding to brushes in Lump 4.
    // So we can just iterate Lump 3 and check if current_bs_idx belongs to any of our targets.
    // Since targets are sequential, we can just track the "current target".
    
    int target_ptr = 0; 
    // Find first valid target
    while(target_ptr < count_in_range && targets[target_ptr].start_bs_idx == -1) target_ptr++;

    if (target_ptr >= count_in_range) {
        // Nothing found
        fclose(f);
        free(targets);
        return 0;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "--- Lump", 8) == 0) {
            int lump_id = -1;
            if (sscanf(line, "--- Lump %d:", &lump_id) == 1) {
                if (lump_id == 3) in_lump_3 = 1;
                else in_lump_3 = 0;
            }
            continue;
        }

        if (in_lump_3) {
            if (line[0] == '[') {
                // Check if current_bs_idx is within current target's range
                long long start = targets[target_ptr].start_bs_idx;
                long long end = start + targets[target_ptr].count;

                if (current_bs_idx >= start && current_bs_idx < end) {
                    // Print with modified index
                    // Parse the rest of the line to print it exactly but with new index
                    char *close_bracket = strchr(line, ']');
                    if (close_bracket) {
                        printf("[%lld]%s", current_print_idx, close_bracket + 1);
                        current_print_idx++;
                    }
                }

                // Advance target if done with this one
                if (current_bs_idx == end - 1) {
                    target_ptr++;
                    // Skip invalid targets (though they shouldn't exist if range was valid)
                    while(target_ptr < count_in_range && targets[target_ptr].start_bs_idx == -1) target_ptr++;
                    if (target_ptr >= count_in_range) break; // Done all
                }

                current_bs_idx++;
            }
        }
    }

    fclose(f);
    free(targets);
    return 0;
}