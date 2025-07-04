#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
    #include "internal/cost_coord_pq.h"
    #include "internal/coord.h"
}

TEST_CASE("cost_coord_pq: basic operations") {
    cost_coord_pq_t* pq = cost_coord_pq_new();
    REQUIRE(pq != nullptr);

    coord_t* c1 = coord_new_full(1, 1);
    coord_t* c2 = coord_new_full(2, 2);
    coord_t* c3 = coord_new_full(3, 3);

    // 삽입
    cost_coord_pq_push(pq, 3.0f, c3);
    cost_coord_pq_push(pq, 1.0f, c1);
    cost_coord_pq_push(pq, 2.0f, c2);

    CHECK(!cost_coord_pq_is_empty(pq));
    CHECK(cost_coord_pq_peek_cost(pq) == doctest::Approx(1.0f));

    // 최소 pop 확인
    coord_t* top = cost_coord_pq_pop(pq);
    CHECK(top->x == 1);
    CHECK(top->y == 1);
    coord_free(top);  // pop된 좌표는 직접 해제

    CHECK(cost_coord_pq_peek_cost(pq) == doctest::Approx(2.0f));

    // contains
    CHECK(cost_coord_pq_contains(pq, c2));
    CHECK(!cost_coord_pq_contains(pq, c1));  // pop 됨

    // 업데이트
    cost_coord_pq_update(pq, 2.0f, 0.5f, c2);  // c2의 cost를 낮춤
    CHECK(cost_coord_pq_peek(pq)->x == 2);

    // 삭제
    CHECK(cost_coord_pq_remove(pq, 0.5f, c2));
    CHECK(!cost_coord_pq_contains(pq, c2));

    // 마지막 pop
    coord_t* last = cost_coord_pq_pop(pq);
    CHECK(last->x == 3);
    coord_free(last);

    CHECK(cost_coord_pq_is_empty(pq));

    // 정리
    // coord_free(c1);
    // coord_free(c2);
    // coord_free(c3);
    cost_coord_pq_free(pq);
}
