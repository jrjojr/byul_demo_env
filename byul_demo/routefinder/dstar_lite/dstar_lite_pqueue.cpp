#include "internal/dstar_lite_pqueue.h"
#include "internal/coord.h"
#include "internal/dstar_lite_key.h"
#include "internal/core.h"

#include <map>
#include <vector>
#include <algorithm>

#include "internal/coord_hash.h"

typedef struct s_dstar_lite_pqueue {
    std::map<dstar_lite_key_t*, 
        std::vector<coord_t*>, std::less<>> key_to_coords;
        
    coord_hash_t* coord_to_key;
}dstar_lite_pqueue_t;

dstar_lite_pqueue_t* dstar_lite_pqueue_new() {
    auto* q = new dstar_lite_pqueue_t{};
    q->coord_to_key = coord_hash_new();  // 별이아빠님이 제공한 해시 구조체 생성
    return q;
}

void dstar_lite_pqueue_free(dstar_lite_pqueue_t* q) {
    if (!q) return;

    for (auto& pair : q->key_to_coords) {
        delete pair.first;
        for (auto* c : pair.second) {
            coord_free(c);
        }
    }

    coord_hash_free(q->coord_to_key);
    delete q;
}

dstar_lite_pqueue_t* dstar_lite_pqueue_copy(const dstar_lite_pqueue_t* src) {
    if (!src) return nullptr;

    auto* copy = new dstar_lite_pqueue_t{};
    copy->coord_to_key = coord_hash_copy(src->coord_to_key);

    for (const auto& [key, coords] : src->key_to_coords) {
        auto* copied_key = dstar_lite_key_copy(key);
        std::vector<coord_t*> copied_coords;
        for (auto* c : coords)
            copied_coords.push_back(coord_copy(c));

        copy->key_to_coords[copied_key] = std::move(copied_coords);
    }

    return copy;
}

void dstar_lite_pqueue_push(
    dstar_lite_pqueue_t* q,
    const dstar_lite_key_t* key,
    const coord_t* c) {
    if (!q || !key || !c) return;

    auto* new_key = dstar_lite_key_copy(key);
    auto* new_coord = coord_copy(c);

    q->key_to_coords[new_key].push_back(new_coord);
    coord_hash_insert(q->coord_to_key, new_coord, new_key);
}

coord_t* dstar_lite_pqueue_peek(dstar_lite_pqueue_t* q) {
    if (!q || q->key_to_coords.empty()) return nullptr;

    const auto& entry = *q->key_to_coords.begin();
    if (entry.second.empty()) return nullptr;

    return coord_copy(entry.second.front());
}

coord_t* dstar_lite_pqueue_pop(dstar_lite_pqueue_t* q) {
    if (!q || q->key_to_coords.empty()) return nullptr;

    auto it = q->key_to_coords.begin();
    auto& vec = it->second;
    if (vec.empty()) return nullptr;

    coord_t* popped = vec.front();
    coord_hash_remove(q->coord_to_key, popped);
    vec.erase(vec.begin());

    if (vec.empty()) {
        delete it->first;
        q->key_to_coords.erase(it);
    }

    return popped;
}

bool dstar_lite_pqueue_is_empty(dstar_lite_pqueue_t* q) {
    return !q || q->key_to_coords.empty();
}

bool dstar_lite_pqueue_remove(dstar_lite_pqueue_t* q, const coord_t* u) {
    if (!q || !u) return false;

    dstar_lite_key_t* key = static_cast<dstar_lite_key_t*>(
        coord_hash_get(q->coord_to_key, u));

    if (!key) return false;

    auto it = q->key_to_coords.find(key);
    if (it == q->key_to_coords.end()) return false;

    auto& vec = it->second;
    auto found = std::find_if(vec.begin(), vec.end(),
        [&](coord_t* c) { return coord_equal(c, u); });

    if (found != vec.end()) {
        coord_free(*found);
        vec.erase(found);
        coord_hash_remove(q->coord_to_key, u);
        if (vec.empty()) {
            delete key;
            q->key_to_coords.erase(it);
        }
        return true;
    }

    return false;
}

bool dstar_lite_pqueue_remove_full(
    dstar_lite_pqueue_t* q,
    const dstar_lite_key_t* key,
    const coord_t* c) {
    if (!q || !key || !c) return false;

    auto it = q->key_to_coords.find(const_cast<dstar_lite_key_t*>(key));
    if (it == q->key_to_coords.end()) return false;

    auto& vec = it->second;
    auto found = std::find_if(vec.begin(), vec.end(),
        [&](coord_t* item) { return coord_equal(item, c); });

    if (found != vec.end()) {
        coord_free(*found);
        vec.erase(found);
        coord_hash_remove(q->coord_to_key, c);
        if (vec.empty()) {
            delete it->first;
            q->key_to_coords.erase(it);
        }
        return true;
    }

    return false;
}

dstar_lite_key_t* dstar_lite_pqueue_find_key_by_coord(
    dstar_lite_pqueue_t* q, const coord_t* c) {
    if (!q || !c) return nullptr;
    return static_cast<dstar_lite_key_t*>(
        coord_hash_get(q->coord_to_key, c));
}

dstar_lite_key_t* dstar_lite_pqueue_top_key(dstar_lite_pqueue_t* q) {
    if (!q || q->key_to_coords.empty()) return nullptr;
    return dstar_lite_key_copy(q->key_to_coords.begin()->first);
}

bool dstar_lite_pqueue_contains(dstar_lite_pqueue_t* q, const coord_t* u) {
    if (!q || !u) return false;
    return coord_hash_contains(q->coord_to_key, u);
}
