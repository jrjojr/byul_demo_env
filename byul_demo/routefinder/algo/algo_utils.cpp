#include "internal/algo.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include "internal/core.h"
#include "internal/coord_hash.h"
#include "internal/coord_list.h"
#include "internal/route.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static char get_map_char(
    const map_t* m, int x, int y,
    const coord_t* start, const coord_t* goal,
    const coord_hash_t* route_coords,
    const coord_hash_t* visited_count
) {
    if (start && coord_get_x(start) == x && coord_get_y(start) == y) 
        return 'S';

    if (goal && coord_get_x(goal) == x && coord_get_y(goal) == y) 
        return 'G';

    if (map_is_blocked(m, x, y)) return '#';

    coord_t* tmp = coord_new_full(x, y);

    bool is_route = route_coords && coord_hash_contains(route_coords, tmp);
    bool is_visited = visited_count && coord_hash_contains(visited_count, tmp);

    coord_free(tmp);

    if (is_route) return '*';
    if (is_visited) return '+';
    return '.';
}

static const char* get_map_string(
    const map_t* m, int x, int y,
    const coord_t* start, const coord_t* goal,
    const coord_hash_t* route_coords,
    const coord_hash_t* visited_count
) {
    static char buf[5];

    if (start && coord_get_x(start) == x && coord_get_y(start) == y)
        return "  S";
    if (goal && coord_get_x(goal) == x && coord_get_y(goal) == y)
        return "  G";

    if (map_is_blocked(m, x, y)) return "  #";

    coord_t* tmp = coord_new_full(x, y);

    bool is_route = route_coords && coord_hash_contains(route_coords, tmp);
    void* val = visited_count ? coord_hash_get(visited_count, tmp) : NULL;

    coord_free(tmp);

    if (is_route) return "  *";    

    if (val) {
        int count = *static_cast<int*>(val);
        if (count > 999) count = 999;
        snprintf(buf, sizeof(buf), "%3d", count);
        return buf;
    }

    return "  .";
}

void map_print_ascii(const map_t* m) {
    if (!m) return;

    int width = m->width;
    int height = m->height;
    bool override_width = false;
    bool override_height = false;

    if (width == 0) {
        width = 10;
        override_width = true;
    }
    if (height == 0) {
        height = 10;
        override_height = true;
    }

    printf("[MAP %dx%d ASCII]\n", width, height);

    if (override_width || override_height) {
        printf("[AUTO SIZE OVERRIDE:");
        if (override_width)  printf(" width=0→10");
        if (override_width && override_height) printf(",");
        if (override_height) printf(" height=0→10");
        printf("]\n");
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            printf("%s", get_map_string(m, x, y, NULL, NULL, NULL, NULL));
        }
        putchar('\n');
    }
}

void map_print_ascii_with_route(const map_t* m, const route_t* p, int margin) {
    if (!m || !p) return;

    const coord_list_t* list = route_get_coords(p);
    if (!list || coord_list_length(list) == 0) return;

    const coord_t* start = coord_list_get(list, 0);
    const coord_t* goal = coord_list_get(list, coord_list_length(list) - 1);

    coord_hash_t* route_coords = coord_hash_new();

    int min_x = coord_get_x(start), max_x = coord_get_x(start);
    int min_y = coord_get_y(start), max_y = coord_get_y(start);

    for (int i = 0; i < coord_list_length(list); ++i) {
        const coord_t* c = coord_list_get(list, i);
        coord_hash_replace(route_coords, coord_copy(c), NULL);

        int x = coord_get_x(c);
        int y = coord_get_y(c);
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }

    // const int margin = 2;
    if (m->width == 0) {
        min_x -= margin; max_x += margin;
    } else {
        min_x = 0; max_x = m->width - 1;
    }

    if (m->height == 0) {
        min_y -= margin; max_y += margin;
    } else {
        min_y = 0; max_y = m->height - 1;
    }

    printf("MAP %d,%d to %d,%d with PATH - total_retry: %d\n", 
        min_x, min_y, max_x, max_y, p->total_retry_count);

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            printf("%s", get_map_string(m, x, y, 
                start, goal, route_coords, NULL));
        }
        putchar('\n');
    }

    coord_hash_free(route_coords);
}

void map_print_ascii_with_visited_count(
    const map_t* m, const route_t* p, int margin) {

    if (!m || !p || !route_get_visited_count(p)) return;

    const coord_hash_t* visited = route_get_visited_count(p);
    const coord_list_t* list = route_get_coords(p);
    coord_hash_t* route_coords = coord_hash_new();

    if (!list || coord_list_length(list) == 0) {
        coord_hash_free(route_coords);
        return;
    }

    const coord_t* start = coord_list_get(list, 0);
    const coord_t* goal = coord_list_get(list, coord_list_length(list) - 1);

    // 경로를 해시로 기록하고, 동시에 최소/최대 좌표 계산
    int min_x = coord_get_x(start), max_x = coord_get_x(start);
    int min_y = coord_get_y(start), max_y = coord_get_y(start);

    int len = coord_list_length(list);
    for (int i = 0; i < len; ++i) {
        const coord_t* c = coord_list_get(list, i);
        coord_hash_replace(route_coords, coord_copy(c), NULL);

        int x = coord_get_x(c);
        int y = coord_get_y(c);
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }

    // 여유 여백 추가 (옵션)
    if (m->width == 0) {
        min_x -= margin; max_x += margin;
    } else {
        min_x = 0; max_x = m->width - 1;
    }

    if (m->height == 0) {
        min_y -= margin; max_y += margin;
    } else {
        min_y = 0; max_y = m->height - 1;
    }

    printf("MAP %d,%d to %d,%d with PATH and Visit Counts - total_retry: %d\n", 
        min_x, min_y, max_x, max_y, p->total_retry_count);

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            printf("%s", get_map_string(m, x, y, 
                start, goal, route_coords, visited));
        }
        putchar('\n');
    }

    coord_hash_free(route_coords);
}
