#include "internal/cost_coord_pq.h"
#include "coord.hpp"   // coord_t, coord_hash, coord_equal
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>

// ------------------------ 내부 구조체 정의 ------------------------

struct Entry {
    float cost;
    coord_t* c;

    bool operator>(const Entry& other) const {
        return cost > other.cost;
    }
};

// C의 coord_hash / coord_equal을 C++ STL용으로 포장
struct coord_ptr_hash {
    size_t operator()(const coord_t* c) const {
        return static_cast<size_t>(coord_hash(c));
    }
};

struct coord_ptr_equal {
    bool operator()(const coord_t* a, const coord_t* b) const {
        return coord_equal(a, b);
    }
};

struct s_cost_coord_pq {
    std::priority_queue<Entry, 
        std::vector<Entry>, std::greater<Entry>> heap;

    std::unordered_map<coord_t*, float, 
        coord_ptr_hash, coord_ptr_equal> cost_map;
};

// ------------------------ 생성/해제 ------------------------

cost_coord_pq_t* cost_coord_pq_new() {
    return new s_cost_coord_pq();
}

void cost_coord_pq_free(cost_coord_pq_t* pq) {
    delete pq;
}

// ------------------------ 삽입/조회 ------------------------

void cost_coord_pq_push(cost_coord_pq_t* pq, float cost, coord_t* c) {
    if (!pq || !c) return;
    pq->heap.push({cost, c});
    pq->cost_map[c] = cost;
}

coord_t* cost_coord_pq_peek(cost_coord_pq_t* pq) {
    if (!pq) return nullptr;
    while (!pq->heap.empty()) {
        coord_t* c = pq->heap.top().c;
        float cost = pq->heap.top().cost;
        auto it = pq->cost_map.find(c);
        if (it != pq->cost_map.end() && it->second == cost) {
            return c;
        }
        pq->heap.pop();  // stale entry 제거
    }
    return nullptr;
}

coord_t* cost_coord_pq_pop(cost_coord_pq_t* pq) {
    if (!pq) return nullptr;
    while (!pq->heap.empty()) {
        coord_t* c = pq->heap.top().c;
        float cost = pq->heap.top().cost;
        pq->heap.pop();

        auto it = pq->cost_map.find(c);
        if (it != pq->cost_map.end() && it->second == cost) {
            pq->cost_map.erase(it);
            return c;
        }
        // else: stale entry, skip
    }
    return nullptr;
}

float cost_coord_pq_peek_cost(cost_coord_pq_t* pq) {
    if (!pq) return 0.0f;
    while (!pq->heap.empty()) {
        coord_t* c = pq->heap.top().c;
        float cost = pq->heap.top().cost;
        auto it = pq->cost_map.find(c);
        if (it != pq->cost_map.end() && it->second == cost) {
            return cost;
        }
        pq->heap.pop();  // stale 제거
    }
    return 0.0f;
}

// ------------------------ 검사/삭제 ------------------------

bool cost_coord_pq_is_empty(cost_coord_pq_t* pq) {
    if (!pq) return true;
    while (!pq->heap.empty()) {
        coord_t* c = pq->heap.top().c;
        float cost = pq->heap.top().cost;
        auto it = pq->cost_map.find(c);
        if (it != pq->cost_map.end() && it->second == cost) {
            return false;
        }
        pq->heap.pop();  // stale 제거
    }
    return true;
}

bool cost_coord_pq_contains(cost_coord_pq_t* pq, coord_t* c) {
    if (!pq || !c) return false;
    return pq->cost_map.find(c) != pq->cost_map.end();
}

void cost_coord_pq_update(
    cost_coord_pq_t* pq, float old_cost, float new_cost, coord_t* c) {

    if (!pq || !c) return;
    pq->heap.push({new_cost, c});
    pq->cost_map[c] = new_cost;
}

bool cost_coord_pq_remove(cost_coord_pq_t* pq, float cost, coord_t* c) {
    if (!pq || !c) return false;
    auto it = pq->cost_map.find(c);
    if (it != pq->cost_map.end() && it->second == cost) {
        pq->cost_map.erase(it);
        return true;
    }
    return false;
}

int cost_coord_pq_length(cost_coord_pq_t* pq) {
    if (!pq) return 0;
    return static_cast<int>(pq->cost_map.size());
}

void cost_coord_pq_trim_worst(cost_coord_pq_t* pq, int n) {
    if (!pq || n <= 0) return;

    using pair_type = std::pair<coord_t*, float>;

    // 모든 유효 엔트리를 복사
    std::vector<pair_type> entries(pq->cost_map.begin(), pq->cost_map.end());

    // cost 내림차순으로 정렬 (비용이 가장 큰 것부터)
    std::sort(entries.begin(), entries.end(), 
        [](const pair_type& a, const pair_type& b) {
            return a.second > b.second;
        });

    // n개 제거
    for (int i = 0; i < n && i < static_cast<int>(entries.size()); ++i) {
        pq->cost_map.erase(entries[i].first);
        // 힙에는 여전히 남아 있지만, 다음 peek/pop에서 제거됨
    }
}
