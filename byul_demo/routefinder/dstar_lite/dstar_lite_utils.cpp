#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_set>
#include "internal/core.h"
#include "internal/dstar_lite_utils.h"
#include "internal/coord.h"
#include "internal/dstar_lite_key.h"
#include "internal/route.h"
#include "internal/map.h"
#include "internal/coord_hash.h"
#include "internal/dstar_lite_pqueue.h"

#include "internal/algo_utils.h"

void dsl_debug_print_g_table(const map_t* m, coord_hash_t* g_table) {
    if (!g_table) return;
    printf("\n📊 g_table (g-values):\n");

    coord_hash_iter_t* it = coord_hash_iter_new(g_table);
    coord_t* c;
    float* val;
    while (coord_hash_iter_next(it, &c, (void**)&val)) {
        printf("  (%d, %d) → g = %.3f\n", c->x, c->y, *val);
    }
}

void dsl_debug_print_rhs_table(const map_t* m, coord_hash_t* rhs_table) {
    if (!rhs_table) return;
    printf("\n📊 rhs_table:\n");

    coord_hash_iter_t* it = coord_hash_iter_new(rhs_table);
    coord_t* c;
    float* val;
    while (coord_hash_iter_next(it, &c, (void**)&val)) {
        printf("  (%d, %d) → rhs = %.3f\n", c->x, c->y, *val);
    }
}

void dsl_print_ascii_only_map(const dstar_lite_t* dsl){
    if(!dsl) return;
    map_print_ascii(dsl->m);
}

void dsl_print_ascii_route(
    const dstar_lite_t* dsl, const route_t* p, int margin) {

    if (!dsl || !dsl->m) return;

    map_print_ascii_with_route(dsl->m, p, margin);
}

void dsl_print_ascii_update_count(
    const dstar_lite_t* dsl, const route_t* p, int margin) {

    if (!dsl || !p) return;

    map_print_ascii_with_visited_count(dsl->m, p, margin);
}
