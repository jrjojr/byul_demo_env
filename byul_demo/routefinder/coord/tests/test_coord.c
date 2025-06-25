#include <glib.h>
#include "internal/coord.h"

static void test_coord_create_free(void) {
    coord c = coord_new_full(10, 20);
    g_assert_nonnull(c);
    g_assert_cmpint(coord_get_x(c), ==, 10);
    g_assert_cmpint(coord_get_y(c), ==, 20);
    coord_free(c);
}

static void test_coord_hash_equal_copy(void) {
    coord c1 = coord_new_full(5, 5);
    coord c2 = coord_copy(c1);
    g_assert_true(coord_equal(c1, c2));
    g_assert_cmpuint(coord_hash(c1), ==, coord_hash(c2));
    coord_free(c1);
    coord_free(c2);
}

static void test_coord_pack_unpack(void) {
    coord c = coord_new_full(42, 84);
    guint64 packed = coord_pack(c);
    coord c2 = coord_unpack(packed);
    g_assert_true(coord_equal(c, c2));
    coord_free(c);
    coord_free(c2);
}

static void test_coord_degree_basic(void)
{
    coord a = coord_new_full(0, 0);
    coord b = coord_new_full(1, 0);
    coord c = coord_new_full(0, 1);
    coord d = coord_new_full(-1, 0);
    coord e = coord_new_full(0, -1);

    gdouble r;

    r = coord_degree(a, b);
    g_assert_cmpfloat(r, ==, 0.0);

    r = coord_degree(a, c);
    g_assert_cmpfloat(r, ==, 90.0);

    r = coord_degree(a, d);
    g_assert_cmpfloat(r, ==, 180.0);

    r = coord_degree(a, e);
    g_assert_cmpfloat(r, ==, 270.0);

    coord_free(a);
    coord_free(b);
    coord_free(c);
    coord_free(d);
    coord_free(e);
}

static void test_coord_degree_diagonal(void)
{
    coord a = coord_new_full(0, 0);
    coord b = coord_new_full(1, 1);
    coord c = coord_new_full(-1, 1);
    coord d = coord_new_full(-1, -1);
    coord e = coord_new_full(1, -1);

    gdouble r;

    r = coord_degree(a, b);
    g_assert_cmpfloat(r, >, 44.9);
    g_assert_cmpfloat(r, <, 45.1);

    r = coord_degree(a, c);
    g_assert_cmpfloat(r, >, 134.9);
    g_assert_cmpfloat(r, <, 135.1);

    r = coord_degree(a, d);
    g_assert_cmpfloat(r, >, 224.9);
    g_assert_cmpfloat(r, <, 225.1);

    r = coord_degree(a, e);
    g_assert_cmpfloat(r, >, 314.9);
    g_assert_cmpfloat(r, <, 315.1);

    coord_free(a);
    coord_free(b);
    coord_free(c);
    coord_free(d);
    coord_free(e);    
}


int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/coord/create_free", test_coord_create_free);
    g_test_add_func("/coord/hash_equal_copy", test_coord_hash_equal_copy);
    g_test_add_func("/coord/pack_unpack", test_coord_pack_unpack);

    g_test_add_func("/coord/degree/basic", test_coord_degree_basic);
    g_test_add_func("/coord/degree/diagonal", test_coord_degree_diagonal);


    return g_test_run();
}