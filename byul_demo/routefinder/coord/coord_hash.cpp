#include "internal/coord.hpp"
#include "internal/coord_list.h"
#include "internal/coord_hash.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>

struct CoordHash {
    size_t operator()(const coord_t& c) const {
        return coord_hash(&c);  // ✅ C 함수 기반 해시 사용
    }
};

struct CoordEqual {
    bool operator()(const coord_t& a, const coord_t& b) const {
        return coord_equal(&a, &b);  // ✅ C 함수 기반 비교 사용
    }
};

std::unordered_map<coord_t, void*, CoordHash, CoordEqual> data;


struct s_coord_hash {
    std::unordered_map<coord_t, void*, CoordHash, CoordEqual> data;
};

coord_hash_t* coord_hash_new() {
    return new s_coord_hash();
}

void coord_hash_free(coord_hash_t* hash) {
    delete hash;
}

int coord_hash_length(const coord_hash_t* hash) {
    return hash ? static_cast<int>(hash->data.size()) : 0;
}

bool coord_hash_is_empty(const coord_hash_t* hash) {
    return !hash || hash->data.empty();
}

void* coord_hash_get(const coord_hash_t* hash, const coord_t* key) {
    if (!hash || !key) return nullptr;
    auto it = hash->data.find(*key);
    return it != hash->data.end() ? it->second : nullptr;
}

bool coord_hash_contains(const coord_hash_t* hash, const coord_t* key) {
    return hash && key && hash->data.find(*key) != hash->data.end();
}

void coord_hash_set(coord_hash_t* hash, const coord_t* key, void* value) {
    if (hash && key) hash->data[*key] = value;
}

bool coord_hash_insert(coord_hash_t* hash, const coord_t* key, void* value) {
    if (!hash || !key) return false;
    return hash->data.insert({*key, value}).second;
}

bool coord_hash_replace(coord_hash_t* hash, const coord_t* key, void* value) {
    if (!hash || !key) return false;
    hash->data[*key] = value;
    return true;
}

bool coord_hash_remove(coord_hash_t* hash, const coord_t* key) {
    return hash && key && hash->data.erase(*key) > 0;
}

void coord_hash_clear(coord_hash_t* hash) {
    if (hash) hash->data.clear();
}

void coord_hash_remove_all(coord_hash_t* hash) {
    coord_hash_clear(hash);
}

coord_hash_t* coord_hash_copy(const coord_hash_t* original) {
    if (!original) return nullptr;
    coord_hash_t* copy = coord_hash_new();
    copy->data = original->data;
    return copy;
}

bool coord_hash_equal(const coord_hash_t* a, const coord_hash_t* b) {
    return a && b && a->data == b->data;
}

coord_list_t* coord_hash_keys(const coord_hash_t* h) {
    if (!h) return nullptr;
    coord_list_t* list = coord_list_new();
    for (const auto& [k, _] : h->data) coord_list_push_back(list, &k);
    return list;
}

coord_list_t* coord_hash_to_list(const coord_hash_t* hash) {
    if (!hash) return nullptr;
    coord_list_t* list = coord_list_new();
    for (const auto& [key, _] : hash->data) {
        coord_list_push_back(list, coord_copy(&key));
    }
    return list;
}

void** coord_hash_values(const coord_hash_t* hash, int* out_count) {
    if (!hash || !out_count) return nullptr;
    int n = static_cast<int>(hash->data.size());
    void** result = static_cast<void**>(malloc(sizeof(void*) * n));
    int i = 0;
    for (const auto& [key, val] : hash->data) {
        result[i++] = val;
    }
    *out_count = n;
    return result;
}

void coord_hash_foreach(
    coord_hash_t* hash, coord_hash_func func, void* user_data) {

    if (!hash || !func) return;
    for (auto& [key, value] : hash->data) {
        func(&key, value, user_data);
    }
}

void coord_hash_export(const coord_hash_t* hash, 
    coord_list_t* keys_out, void** values_out, int* count_out) {
        
    if (!hash || !keys_out || !values_out || !count_out) return;
    int i = 0;
    for (const auto& [key, val] : hash->data) {
        coord_list_push_back(keys_out, &key);
        values_out[i++] = val;
    }
    *count_out = static_cast<int>(hash->data.size());
}

typedef struct s_coord_hash_iter {
    const std::unordered_map<coord_t, void*, CoordHash, CoordEqual>* map;
    std::unordered_map<coord_t, void*, CoordHash, CoordEqual>::const_iterator it;
    std::unordered_map<coord_t, void*, CoordHash, CoordEqual>::const_iterator end;
} coord_hash_iter_t;

coord_hash_iter_t* coord_hash_iter_new(const coord_hash_t* hash) {
    if (!hash) return nullptr;

    auto* iter = new coord_hash_iter_t;
    iter->map = &hash->data;
    iter->it = hash->data.begin();
    iter->end = hash->data.end();
    return iter;
}

bool coord_hash_iter_next(coord_hash_iter_t* iter, coord_t** key_out, void** val_out) {
    if (!iter || iter->it == iter->end) return false;

    if (key_out) *key_out = const_cast<coord_t*>(&iter->it->first);
    if (val_out) *val_out = iter->it->second;

    ++(iter->it);
    return true;
}

void coord_hash_iter_free(coord_hash_iter_t* iter) {
    delete iter;
}
