#include "internal/route.h"
#include "internal/core.h"
#include <math.h>

#define EPSILON 1e-6f

static const int ROUTE_DIRECTION_VECTORS[9][2] = {
    {  0,  0 },  // UNKNOWN
    {  1,  0 },  // RIGHT
    {  1, -1 },  // TOP_RIGHT
    {  0, -1 },  // TOP
    { -1, -1 },  // TOP_LEFT
    { -1,  0 },  // LEFT
    { -1,  1 },  // DOWN_LEFT
    {  0,  1 },  // DOWN
    {  1,  1 },  // DOWN_RIGHT
};


route route_new(void) {
    return route_new_full(0.0f);
}

route route_new_full(gfloat cost) {
    route p = g_malloc0(sizeof(route_t));
    if (!p) return NULL;

    p->coords = NULL;
    p->cost = cost;
    p->success = FALSE;

    p->visited_order = NULL;

    p->visited_count = g_hash_table_new_full(
        (GHashFunc) coord_hash, 
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free, 
        NULL
    );

    return p;
}

void route_free(const route p) {
    if (!p) return;
    if (p->coords) g_list_free_full(p->coords, (GDestroyNotify) coord_free);
    if (p->visited_count) g_hash_table_destroy(p->visited_count);
    if (p->visited_order) {
        g_list_free_full(p->visited_order, (GDestroyNotify) coord_free);
    }

    g_free(p);
}

guint route_hash(const route p) {
    return (guint)(guintptr)p;
}

gboolean route_equal(const route a, const route b) {
    if (!a || !b) return FALSE;
    return g_direct_equal(a, b);    
}

route route_copy(const route p) {
    if (!p) return NULL;
    route c = route_new();
    // c->coords = p->coords;
    c->coords = g_list_copy_deep(p->coords,
        (GCopyFunc) coord_copy, NULL);
    c->cost = p->cost;
    c->success = p->success;
    // if (p->visited) c->visited = p->visited;
    if (p->visited_count) {
        // 이미 new 시점에 c->visited는 메모리 할당되어있다
        g_hash_table_destroy(c->visited_count);
        c->visited_count = NULL;
        c->visited_count = g_hash_table_copy_deep(p->visited_count, 
            (GHashFunc) coord_hash,
            (GEqualFunc) coord_equal,
            (GCopyFunc) coord_copy, 
            (GDestroyNotify) coord_free,
            NULL, NULL);
        }
    
    // if (p->visited_order) c->visited_order = p->visited_order;
    if (p->visited_order) {
        c->visited_order = g_list_copy_deep(p->visited_order,
        (GCopyFunc) coord_copy, NULL);
    }

    return c;
}

void route_set_cost(route p, gfloat cost) {
    if (p) p->cost = cost;
}

gfloat route_get_cost(const route p) {
    return p ? p->cost : 0.0f;
}

void route_set_success(route p, gboolean success) {
    if (p) p->success = success;
}

gboolean route_get_success(const route p) {
    return p ? p->success : FALSE;
}

GList* route_get_coords(const route p) {     
    return p ? p->coords : NULL;
}

void route_set_coords(const route p, const GList* coords) {
    if (!p || !coords) return;
    if (p->coords) {
        g_list_free_full(p->coords, (GDestroyNotify) coord_free);
    }
    p->coords = g_list_copy_deep((GList*)coords,
        (GCopyFunc) coord_copy, NULL);
}

GHashTable* route_get_visited_count(const route p) {
    return p ? p->visited_count : NULL;
}

void route_set_visited_count(route p, const GHashTable* visited_count) {
    if (!p) return;
    if (p->visited_count) g_hash_table_destroy(p->visited_count);
    // p->visited = visited;
    p->visited_count = g_hash_table_copy_deep((GHashTable*)visited_count,
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GCopyFunc) coord_copy,
        (GDestroyNotify) coord_free,
        NULL, NULL);
}

GList* route_get_visited_order(const route p) {
    return p ? p->visited_order : NULL;    
}

void route_set_visited_order(route p, const GList* visited_order) {
    if (!p || !visited_order) return;
    if (p->visited_order) {
        g_list_free_full(p->visited_order, (GDestroyNotify) coord_free);
    }
    p->visited_order = g_list_copy_deep((GList*)visited_order, 
        (GCopyFunc) coord_copy, NULL);
}

gboolean route_add_coord(route p, const coord c) {
    if (!p || !c) return FALSE;

    coord copy = coord_copy(c);
    if (!copy) return FALSE;

    p->coords = g_list_append(p->coords, copy);
    return TRUE;
}

void route_clear_coords(route p) {
    if (!p || !p->coords) return;
    g_list_free_full(p->coords, (GDestroyNotify)coord_free);
    p->coords = NULL;
}

gboolean route_add_visited(route p, const coord c) {
    if (!p || !c) return FALSE;

    p->visited_order = g_list_append(p->visited_order, coord_copy(c));

    gpointer val = g_hash_table_lookup(p->visited_count, c);
    gint count = val ? GPOINTER_TO_INT(val) : 0;
    count++;

    gboolean result = g_hash_table_replace(
        p->visited_count,
        coord_copy(c),
        GINT_TO_POINTER(count)
    );

    return result;
}

void route_clear_visited(route p) {
    if (!p || !p->visited_count) return;
    if (p->visited_order)
        g_list_free_full(p->visited_order, (GDestroyNotify)coord_free);
    if (p->visited_count) g_hash_table_destroy(p->visited_count);
}

// void route_add_from_came_from(route p, GHashTable* came_from, 
//     const coord goal) {

//     GList* rev_route = reconstruct_route(came_from, goal);
//     for (GList* l = rev_route; l != NULL; l = l->next) {
//         route_add_coord(p, (coord)l->data);
//     }
//     g_list_free(rev_route);
// }

gint route_length(const route p) {
    if (!p || !p->coords) return 0;
    return g_list_length(p->coords);
}

coord route_get_last(const route p) {
    if (!p || !p->coords) return NULL;
    return (coord)g_list_last(p->coords)->data;
}

void route_append(route dest, const route src) {
    if (!dest || !src || !src->coords) return;

    for (GList* l = src->coords; l; l = l->next) {
        coord c = (coord)l->data;
        // route_add_coord(dest, coord_copy(c));
        route_add_coord(dest, c);
    }
}

void route_append_nodup(route dest, const route src) {
    if (!dest || !src || !src->coords) return;

    GList* tail = dest->coords;
    coord last = tail ? (coord)g_list_last(tail)->data : NULL;

    for (GList* l = src->coords; l; l = l->next) {
        coord c = (coord)l->data;

        if (last && coord_equal(last, c)) {
            continue; // 중복 방지
        }

        coord c_copy = coord_copy(c);
        dest->coords = g_list_append(dest->coords, c_copy);
        last = c_copy;
    }
}

void route_print(const route p) {
    g_print("최종 경로 : ");
    for (GList* l = route_get_coords(p); l; l = l->next) {
            coord c = (coord)l->data;
            if (l->next) {
                g_print("(%d, %d) -> ", c->x, c->y);
            } else {
                g_print("(%d, %d)", c->x, c->y);
            }
        }    
    g_print("\n");    
}

coord route_look_at(route p, int index) {
    if (!p || route_length(p) < 2) return NULL;

    int len = route_length(p);
    if (index < 0 || index >= len) return NULL;

    coord prev = NULL;
    coord curr = route_get_coord_at(p, index);
    coord next = NULL;

    if (index >= 1)
        prev = route_get_coord_at(p, index - 1);

    if (index < len - 1)
        next = route_get_coord_at(p, index + 1);

    int vx = 0;
    int vy = 0;

    if (prev) {
        vx += (curr->x - prev->x);
        vy += (curr->y - prev->y);
    }

    if (next) {
        vx += (next->x - curr->x);
        vy += (next->y - curr->y);
    }

    // 정규화 없이 단순 방향 정수 근사
    if (vx > 1) vx = 1;
    if (vx < -1) vx = -1;
    if (vy > 1) vy = 1;
    if (vy < -1) vy = -1;

    if (vx == 0 && vy == 0)
        return NULL;

    return coord_new_full(vx, vy);
}

// route_dir_t route_get_direction_by_coord(const coord dxdy) {
//     if (!dxdy) return ROUTE_DIR_UNKNOWN;

//     for (int i = 1; i <= 8; ++i) {
//         if (dxdy->x == ROUTE_DIRECTION_VECTORS[i][0] &&
//             dxdy->y == ROUTE_DIRECTION_VECTORS[i][1]) {
//             return (route_dir_t)i;
//         }
//     }

//     return ROUTE_DIR_UNKNOWN;
// }

// route_dir_t route_get_direction_by_coord(const coord dxdy) {
//     if (!dxdy || (dxdy->x == 0 && dxdy->y == 0))
//         return ROUTE_DIR_UNKNOWN;

//     gfloat best_score = -G_MAXFLOAT;
//     int best_dir = 0;

//     for (int i = 1; i <= 8; ++i) {
//         int vx = ROUTE_DIRECTION_VECTORS[i][0];
//         int vy = ROUTE_DIRECTION_VECTORS[i][1];

//         gfloat score = dxdy->x * vx + dxdy->y * vy; // 내적 (cos 유사도와 유사)

//         if (score > best_score) {
//             best_score = score;
//             best_dir = i;
//         }
//     }

//     return (route_dir_t)best_dir;
// }

route_dir_t route_get_direction_by_coord(const coord dxdy) {
    if (!dxdy || (dxdy->x == 0 && dxdy->y == 0))
        return ROUTE_DIR_UNKNOWN;

    int nx = (dxdy->x > 0) ? 1 : (dxdy->x < 0) ? -1 : 0;
    int ny = (dxdy->y > 0) ? 1 : (dxdy->y < 0) ? -1 : 0;

    for (int i = 1; i <= 8; ++i) {
        if (ROUTE_DIRECTION_VECTORS[i][0] == nx &&
            ROUTE_DIRECTION_VECTORS[i][1] == ny) {
            return (route_dir_t)i;
        }
    }

    return ROUTE_DIR_UNKNOWN;
}

route_dir_t route_get_direction_by_index(route p, int index) {
    coord dxdy = route_look_at(p, index);
    if (!dxdy) return ROUTE_DIR_UNKNOWN;
    route_dir_t result = route_get_direction_by_coord(dxdy);
    coord_free(dxdy);
    return result;
}

gboolean route_has_changed(
    route p, const coord from, const coord to, gfloat threshold_deg) {

    if (!p || !from || !to) return FALSE;

    float dx = to->x - from->x;
    float dy = to->y - from->y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 1e-6f) return FALSE;  // 정지 상태

    float curr_vec[2] = { dx / len, dy / len };

    // 내부 벡터 히스토리 누적 or 평균 벡터 사용
    float avg_x = p->avg_vec_x;  // route 구조체에 누적 필드 있다고 가정
    float avg_y = p->avg_vec_y;

    float dot = curr_vec[0] * avg_x + curr_vec[1] * avg_y;
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    float angle_rad = acosf(dot);
    float angle_deg = angle_rad * 180.0f / M_PI;

    return angle_deg > threshold_deg;
}

gboolean route_has_changed_with_angle(
    route p,
    const coord from,
    const coord to,
    gfloat angle_threshold_deg,
    gfloat* out_angle_deg
) {
    if (!p || !from || !to || !out_angle_deg)
        return FALSE;

    float dx = to->x - from->x;
    float dy = to->y - from->y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length < EPSILON) {
        *out_angle_deg = 0.0f;
        return FALSE; // 정지 상태
    }

    float curr_vec_x = dx / length;
    float curr_vec_y = dy / length;

    // 평균 벡터가 아직 유효하지 않다면 (처음 입력)
    if (p->vec_count == 0) {
        p->avg_vec_x = curr_vec_x;
        p->avg_vec_y = curr_vec_y;
        p->vec_count = 1;

        *out_angle_deg = 0.0f;
        return FALSE; // 첫 입력은 비교하지 않음
    }

    // 평균 벡터 정규화
    float avg_length = sqrtf(
        p->avg_vec_x * p->avg_vec_x + p->avg_vec_y * p->avg_vec_y);

    if (avg_length < EPSILON) {
        *out_angle_deg = 0.0f;
        return FALSE;
    }

    float avg_vec_x = p->avg_vec_x / avg_length;
    float avg_vec_y = p->avg_vec_y / avg_length;

    // 내적(dot product) 계산
    float dot = curr_vec_x * avg_vec_x + curr_vec_y * avg_vec_y;
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    float angle_rad = acosf(dot);
    float angle_deg = angle_rad * 180.0f / (float)M_PI;
    *out_angle_deg = angle_deg;

    gboolean changed = (angle_deg > angle_threshold_deg);

    // 평균 벡터 갱신 (단순 누적 방식)
    p->avg_vec_x += curr_vec_x;
    p->avg_vec_y += curr_vec_y;
    p->vec_count += 1;

    return changed;
}

void route_update_average_vector(
    route p,
    const coord from,
    const coord to
) {
    if (!p || !from || !to)
        return;

    float dx = to->x - from->x;
    float dy = to->y - from->y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length < EPSILON)
        return; // 정지 상태, 무시

    float curr_vec_x = dx / length;
    float curr_vec_y = dy / length;

    // 평균 벡터 누적 (normalize는 나중에 필요 시)
    p->avg_vec_x += curr_vec_x;
    p->avg_vec_y += curr_vec_y;
    p->vec_count += 1;
}

coord route_get_coord_at(route p, int index) {
    if (!p || index < 0)
        return NULL;

    GList* node = g_list_nth(p->coords, index);
    if (!node)
        return NULL;

    return (coord)(node->data);
}

void route_update_average_vector_by_index(
    route p, int index_from, int index_to) {

    coord from = route_get_coord_at(p, index_from);
    coord to = route_get_coord_at(p, index_to);
    if (!from || !to) return;

    int dx = to->x - from->x;
    int dy = to->y - from->y;
    gfloat length = sqrtf(dx * dx + dy * dy);
    if (length == 0) return;

    p->avg_vec_x = (
        (p->avg_vec_x * p->vec_count) + (dx / length)) / (p->vec_count + 1);

    p->avg_vec_y = (
        (p->avg_vec_y * p->vec_count) + (dy / length)) / (p->vec_count + 1);

    p->vec_count += 1;
}

gboolean route_has_changed_by_index(
    route p, int index_from, int index_to, gfloat angle_threshold_deg) {

    gfloat angle = 0.0f;
    return route_has_changed_with_angle_by_index(
        p, index_from, index_to, angle_threshold_deg, &angle);
}

gboolean route_has_changed_with_angle_by_index(
    route p,
    int index_from,
    int index_to,
    gfloat angle_threshold_deg,
    gfloat* out_angle_deg) {

    coord from = route_get_coord_at(p, index_from);
    coord to = route_get_coord_at(p, index_to);
    if (!from || !to || !out_angle_deg) return FALSE;

    int dx = to->x - from->x;
    int dy = to->y - from->y;
    gfloat length = sqrtf(dx * dx + dy * dy);
    if (length == 0) {
        *out_angle_deg = 0.0f;
        return FALSE;
    }

    gfloat vec_x = dx / length;
    gfloat vec_y = dy / length;

    gfloat dot = (vec_x * p->avg_vec_x) + (vec_y * p->avg_vec_y);
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    gfloat angle_rad = acosf(dot);
    *out_angle_deg = angle_rad * (180.0f / (gfloat)G_PI);
    return (*out_angle_deg > angle_threshold_deg);
}

gfloat route_calc_average_dir(route p, int history) {
    if (!p || history <= 0) return 0.0f;

    int len = route_length(p);
    if (len < 2) return 0.0f;

    int base_index = (len > history) ? (len - history - 1) : 0;
    int latest_index = len - 1;

    coord base = route_get_coord_at(p, base_index);
    coord latest = route_get_coord_at(p, latest_index);
    if (!base || !latest) return 0.0f;

    int dx = latest->x - base->x;
    int dy = latest->y - base->y;

    if (dx == 0 && dy == 0) return 0.0f; // 정지 상태

    float angle_rad = atan2f((float)dy, (float)dx);
    float angle_deg = angle_rad * (180.0f / (float)M_PI);

    return angle_deg;
}

route_dir_t route_calc_average_facing(route p, int history) {
    if (!p || history < 1) return ROUTE_DIR_UNKNOWN;

    int len = route_length(p);
    if (len < 2) return ROUTE_DIR_UNKNOWN;

    int last = len - 1;
    int from = last - history;
    if (from < 0) from = 0;

    coord c_from = route_get_coord_at(p, from);
    coord c_to   = route_get_coord_at(p, last);

    if (!c_from || !c_to) return ROUTE_DIR_UNKNOWN;

    int dx = c_to->x - c_from->x;
    int dy = c_to->y - c_from->y;

    // 방향 정규화
    if (dx > 1) dx = 1;
    if (dx < -1) dx = -1;
    if (dy > 1) dy = 1;
    if (dy < -1) dy = -1;

    for (int i = 1; i <= 8; ++i) {
        if (dx == ROUTE_DIRECTION_VECTORS[i][0] &&
            dy == ROUTE_DIRECTION_VECTORS[i][1]) {
            return (route_dir_t)i;
        }
    }

    return ROUTE_DIR_UNKNOWN;
}

route_dir_t calc_direction(const coord start, const coord goal) {
    if (!start || !goal) return ROUTE_DIR_UNKNOWN;

    int dx = goal->x - start->x;
    int dy = goal->y - start->y;

    if (dx > 1) dx = 1;
    if (dx < -1) dx = -1;
    if (dy > 1) dy = 1;
    if (dy < -1) dy = -1;

    for (int i = 1; i <= 8; ++i) {
        if (dx == ROUTE_DIRECTION_VECTORS[i][0] &&
            dy == ROUTE_DIRECTION_VECTORS[i][1]) {
            return (route_dir_t)i;
        }
    }

    return ROUTE_DIR_UNKNOWN;
}

coord direction_to_coord(route_dir_t dir) {
    if (dir < ROUTE_DIR_UNKNOWN || dir > ROUTE_DIR_DOWN_RIGHT)
        return NULL;

    return coord_new_full(ROUTE_DIRECTION_VECTORS[dir][0],
                          ROUTE_DIRECTION_VECTORS[dir][1]);
}

void route_insert(route p, int index, const coord c) {
    if (!p || index < 0 || index > g_list_length(p->coords)) return;
    coord cpy = coord_copy(c);
    p->coords = g_list_insert(p->coords, cpy, index);
}

void route_remove_at(route p, int index) {
    if (!p || index < 0 || index >= g_list_length(p->coords)) return;
    GList* nth = g_list_nth(p->coords, index);
    if (nth) {
        g_free(nth->data);
        p->coords = g_list_delete_link(p->coords, nth);
    }
}

void route_remove_value(route p, const coord c) {
    if (!p) return;
    for (GList* l = p->coords; l; l = l->next) {
        if (coord_equal(l->data, c)) {
            g_free(l->data);
            p->coords = g_list_delete_link(p->coords, l);
            break;
        }
    }
}

gboolean route_contains(const route p, const coord c) {
    if (!p) return FALSE;
    for (GList* l = p->coords; l; l = l->next)
        if (coord_equal(l->data, c)) return TRUE;
    return FALSE;
}

gint route_find(const route p, const coord c) {
    if (!p) return -1;
    int i = 0;
    for (GList* l = p->coords; l; l = l->next, ++i)
        if (coord_equal(l->data, c)) return i;
    return -1;
}

void route_slice(route p, int start, int end) {
    if (!p || start < 0 || end <= start) return;
    int len = g_list_length(p->coords);
    if (end > len) end = len;

    GList* new_list = NULL;
    for (int i = start; i < end; ++i) {
        coord c = g_list_nth_data(p->coords, i);
        if (c) new_list = g_list_append(new_list, coord_copy(c));
    }

    g_list_free_full(p->coords, g_free);
    p->coords = new_list;
}