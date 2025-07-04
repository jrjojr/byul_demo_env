#include <glib.h>
#include "internal/core.h"

static void test_equal_float_func(void) {
    g_assert_true(equal_float(1.000001f, 1.000002f));
    g_assert_false(equal_float(1.0f, 1.1f));
}

static void test_compare_float_func(void) {
    gfloat a = 1.0f, b = 2.0f, c = 1.0f;
    g_assert_cmpint(compare_float(&a, &b, NULL), <, 0);
    g_assert_cmpint(compare_float(&b, &a, NULL), >, 0);
    g_assert_cmpint(compare_float(&a, &c, NULL), ==, 0);
}

static void test_hashset_basic_func(void) {
    GHashTable *set = hashset_new();

    g_assert_true(hashset_add(set, "apple"));
    g_assert_true(hashset_contains(set, "apple"));
    g_assert_false(hashset_add(set, "apple")); // 중복
    g_assert_cmpuint(hashset_size(set), ==, 1);
    g_assert_true(hashset_remove(set, "apple"));
    g_assert_cmpuint(hashset_size(set), ==, 0);

    hashset_free(set);
}

static void test_hashset_copy_and_equal_func(void) {
    GHashTable *a = hashset_new();
    hashset_add(a, "x");
    hashset_add(a, "y");

    GHashTable *b = hashset_copy(a);
    g_assert_true(hashset_equal(a, b));

    hashset_add(b, "z");
    g_assert_false(hashset_equal(a, b));

    hashset_free(a);
    hashset_free(b);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/core/equal_float", test_equal_float_func);
    g_test_add_func("/core/compare_float", test_compare_float_func);
    g_test_add_func("/core/hashset_basic", test_hashset_basic_func);

    g_test_add_func("/core/hashset_copy_equal", 
        test_hashset_copy_and_equal_func);

    return g_test_run();
}
