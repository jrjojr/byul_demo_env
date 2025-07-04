#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "internal/route.h"
#include "internal/coord.h"

TEST_CASE("route creation and basic ops") {
    route_t* p = route_new();
    CHECK(route_get_cost(p) == doctest::Approx(0.0f));
    CHECK(route_get_success(p) == doctest::Approx(0.0f));

    coord_t* a = coord_new_full(1, 2);
    coord_t* b = coord_new_full(2, 2);
    coord_t* c = coord_new_full(3, 2);
    route_add_coord(p, a);
    route_add_coord(p, b);
    route_add_coord(p, c);

    const coord_list_t* coords = route_get_coords(p);
    CHECK(coord_list_length(coords) == 3);

    const coord_t* d = coord_list_get(coords, 0);
    const coord_t* e = coord_list_get(coords, 2);
    CHECK(coord_get_x(d) == 1);
    CHECK(coord_get_x(e) == 3);

    coord_free(a);
    coord_free(b);
    coord_free(c);
    route_free(p);
}

TEST_CASE("route visited tracking") {
    route_t* p = route_new();
    coord_t* a = coord_new_full(5, 5);
    coord_t* b = coord_new_full(6, 5);
    route_add_visited(p, a);
    route_add_visited(p, b);
    route_add_visited(p, a);

    const coord_hash_t* visited = route_get_visited_count(p);

    CHECK(*(int*)coord_hash_get(visited, a) == 2);
    CHECK(*(int*)coord_hash_get(visited, b) == 1);    

    const coord_list_t* order = route_get_visited_order(p);
    CHECK(coord_list_length(order) == 3);
    CHECK(coord_get_x(coord_list_get(order, 0)) == 5);
    CHECK(coord_get_x(coord_list_get(order, 2)) == 5);

    coord_free(a);
    coord_free(b);
    route_free(p);
}

TEST_CASE("route direction and angle") {
    route_t* p = route_new();
    coord_t* a = coord_new_full(1, 1);
    coord_t* b = coord_new_full(2, 1);
    coord_t* c = coord_new_full(3, 2);
    route_add_coord(p, a);
    route_add_coord(p, b);
    route_add_coord(p, c);

    coord_t* dir = route_look_at(p, 0);
    CHECK(coord_get_x(dir) == 1);
    CHECK(coord_get_y(dir) == 0);

    CHECK(route_get_direction_by_coord(dir) == ROUTE_DIR_RIGHT);
    CHECK(route_get_direction_by_index(p, 0) == ROUTE_DIR_RIGHT);

    coord_t* from = coord_new_full(2, 2);
    coord_t* to1  = coord_new_full(3, 2);
    coord_t* to2  = coord_new_full(2, 3);
    route_update_average_vector(p, from, to1);

    float angle = 0.0f;
    int changed = route_has_changed_with_angle(p, from, to2, 10.0f, &angle);
    CHECK(changed);
    CHECK(angle >= 89.0f);

    coord_free(a);
    coord_free(b);
    coord_free(c);
    coord_free(from);
    coord_free(to1);
    coord_free(to2);
    route_free(p);
}

TEST_CASE("route insert, remove, find") {
    route_t* r = route_new();
    coord_t* c1 = coord_new_full(1, 1);
    coord_t* c2 = coord_new_full(2, 2);
    coord_t* c3 = coord_new_full(3, 3);
    route_insert(r, 0, c1);
    route_insert(r, 1, c3);
    route_insert(r, 1, c2);

    CHECK(route_length(r) == 3);
    CHECK(route_find(r, c2) == 1);
    CHECK(route_contains(r, c3) == 1);

    route_remove_at(r, 1);
    CHECK(route_length(r) == 2);
    CHECK(route_contains(r, c2) == 0);

    route_remove_value(r, c3);
    CHECK(route_length(r) == 1);
    CHECK(route_find(r, c1) == 0);

    coord_free(c1);
    coord_free(c2);
    coord_free(c3);
    route_free(r);
}

TEST_CASE("route slice") {
    route_t* r = route_new();
    coord_t* tmp[5];
    for (int i = 0; i < 5; ++i) {
        tmp[i] = coord_new_full(i, i);
        route_add_coord(r, tmp[i]);
    }
    route_slice(r, 1, 4);
    CHECK(route_length(r) == 3);
    CHECK(coord_get_x(route_get_coord_at(r, 0)) == 1);
    CHECK(coord_get_x(route_get_coord_at(r, 2)) == 3);

    for (int i = 0; i < 5; ++i) coord_free(tmp[i]);
    route_free(r);
}
