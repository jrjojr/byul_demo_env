#include "internal/algo.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include <stdio.h>
#include "core.h"

static char get_map_char(
    const map m, gint x, gint y,
    const coord start, const coord goal,
    GHashTable* route_coords,
    GHashTable* visited_count
) {
    // 시작점과 도착점 우선 표시
    if (start && coord_get_x(start) == x && coord_get_y(start) == y) 
        return 'S';

    if (goal   && coord_get_x(goal)   == x && coord_get_y(goal)   == y) 
        return 'E';

    if (map_is_blocked(m, x, y)) return '#';

    // 비교용 좌표 생성
    coord tmp = coord_new_full(x, y);

    gboolean is_route    = route_coords && g_hash_table_contains(
        route_coords, tmp);

    gboolean is_visited = visited_count && g_hash_table_contains(
        visited_count, tmp);

    coord_free(tmp);

    if (is_route)    return '*';  // 실제 경로
    if (is_visited) return '+';  // 방문만 한 곳
    return '.';                  // 미방문 평지
}

static const char* get_map_string(
    const map m,
    gint x, gint y,
    const coord start,
    const coord goal,
    GHashTable* route_coords,    
    GHashTable* visited_count
) {
    static char buf[5]; // 충분한 버퍼 (3글자 + null)

    if (start && coord_get_x(start) == x && coord_get_y(start) == y)
        return "  S";
    if (goal && coord_get_x(goal) == x && coord_get_y(goal) == y)
        return "  E";
    if (map_is_blocked(m, x, y))
        return "  #";

    coord tmp = coord_new_full(x, y);

    gboolean is_route    = route_coords && g_hash_table_contains(
        route_coords, tmp);

    gpointer val = visited_count ? 
        g_hash_table_lookup(visited_count, tmp) : NULL;

    coord_free(tmp);

    if (is_route) {
        return "  *";  // 실제 경로
    } 
    else if (val) {
        gint count = GPOINTER_TO_INT(val);
        if (count > 99) count = 99;
        // snprintf(buf, sizeof(buf), "%2d", count);
        snprintf(buf, sizeof(buf), "%3d", count % 1000);
        // snprintf(buf, 4, "%2d", MIN(count, 99));

        return buf;
    }
    return "  .";
}

void map_print_ascii(const map m) {
    if (!m) return;

    printf("[MAP %dx%d ASCII]\n", m->width, m->height);

    for (gint y = 0; y < m->height; y++) {
        for (gint x = 0; x < m->width; x++) {
            putchar(get_map_char(m, x, y, NULL, NULL, NULL, NULL));
        }
        putchar('\n');
    }
}

void map_print_ascii_with_route(const map m,
    const route p, const coord start, const coord goal)
{
    if (!m) return;

    // hashset route_coords = NULL;
    GHashTable* route_coords = g_hash_table_new_full(
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free,
        NULL);

    if (p && route_get_success(p)) {
        for (const GList* l = route_get_coords(p); l != NULL; l = l->next)
            g_hash_table_add(route_coords, coord_copy((coord)l->data));
    }

    printf("[MAP %dx%d with PATH]\n", m->width, m->height);
    for (gint y = 0; y < m->height; y++) {
        for (gint x = 0; x < m->width; x++) {
            printf("%s", get_map_string(
                m, x, y, start, goal, route_coords, NULL));            
        }
        putchar('\n');
    }

    // if (route_coords) hashset_free(route_coords);
    if (route_coords) g_hash_table_destroy(route_coords);
}

void map_print_ascii_with_visited_count(
    const map m,
    const route p,
    const coord start,
    const coord goal
) {
    if (!m || !p || !route_get_visited_count(p)) return;

    GHashTable* route_coords = g_hash_table_new_full(
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free,
        NULL);

    if (p && route_get_success(p)) {
        for (const GList* l = route_get_coords(p); l != NULL; l = l->next)
            // hashset_add(route_coords, coord_copy((coord)l->data));
            // hashset_add_own(route_coords, flud_new_coord((coord)l->data));
            g_hash_table_add(route_coords, coord_copy((coord)l->data));
    }

    // printf("[MAP %dx%d - 방문 횟수 출력 (3자리)]\n", m->width, m->height);    
    printf("[MAP %dx%d - Displaying Visit Counts (3-digit)]\n", 
        m->width, m->height);

    GHashTable* visited = route_get_visited_count(p);    
    for (gint y = 0; y < m->height; y++) {
        for (gint x = 0; x < m->width; x++) {
            printf("%s", get_map_string(
                m, x, y, start, goal, route_coords, visited));
        }
        putchar('\n');
    }

    // if (route_coords) hashset_free(route_coords);
    if (route_coords) g_hash_table_destroy(route_coords);    
}

