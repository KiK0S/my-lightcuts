#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include "Ray.hpp"


struct BoundingBox3d {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
    int longest_axis() const {
        float dx = x_max - x_min;
        float dy = y_max - y_min;
        float dz = z_max - z_min;
        if (dx >= dy && dx >= dz) {
            return 0;
        } else if (dy >= dz) {
            return 1;
        } else {
            return 2;
        }
        return -1;
    }

    std::pair<BoundingBox3d, BoundingBox3d> partition() const {
        float dx = x_max - x_min;
        float dy = y_max - y_min;
        float dz = z_max - z_min;
        BoundingBox3d left_box = *this;
        BoundingBox3d right_box = *this;
        
        if (dx >= dy && dx >= dz) {
            left_box.x_max = x_min + dx / 2;
            right_box.x_min = x_min + dx / 2;
        } else if (dy >= dz) {
            left_box.y_max = y_min + dy / 2;
            right_box.y_min = y_min + dy / 2;
        } else {
            left_box.z_max = z_min + dz / 2;
            right_box.z_min = z_min + dz / 2;    
        }

        return std::make_pair(left_box, right_box);
    }

    // https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
    bool hasIntersection( const Ray& ray) const {
        float tx1 = (x_min - ray.origin.x) / ray.direction.x, tx2 = (x_max - ray.origin.x) / ray.direction.x;
        float tmin = std::min( tx1, tx2 ), tmax = std::max( tx1, tx2 );
        float ty1 = (y_min - ray.origin.y) / ray.direction.y, ty2 = (y_max - ray.origin.y) / ray.direction.y;
        tmin = std::max( tmin, std::min( ty1, ty2 ) ), tmax = std::min( tmax, std::max( ty1, ty2 ) );
        float tz1 = (z_min - ray.origin.z) / ray.direction.z, tz2 = (z_max - ray.origin.z) / ray.direction.z;
        tmin = std::max( tmin, std::min( tz1, tz2 ) ), tmax = std::min( tmax, std::max( tz1, tz2 ) );
        return tmax >= tmin && tmax > 0;
    }

    template <typename T> bool contains(T obj) const;

    bool contains(glm::vec3 pos) const {
        return x_min <= pos.x <= x_max &&
                y_min <= pos.y <= y_max &&
                z_min <= pos.z <= z_max;
    }

    bool contains(std::vector<glm::vec3> positions) const {
        glm::vec3 x{0};
        for (auto p : positions) {
            x += p;
        }
        x /= positions.size();
        return contains(x);
    }

    template <typename T> void update(T obj);

    void update(glm::vec3 point) {
        x_min = std::min(x_min, point.x);
        x_max = std::max(x_max, point.x);
        y_min = std::min(y_min, point.y);
        y_max = std::max(y_max, point.y);
        z_min = std::min(z_min, point.z);
        z_max = std::max(z_max, point.z);
    }

    void update(std::vector<glm::vec3> positions) {
        for (auto p : positions) {
            update(p);
        }
    }

    void update(const BoundingBox3d& other) {
        x_min = std::min(x_min, other.x_min);
        x_max = std::max(x_max, other.x_max);
        y_min = std::min(y_min, other.y_min);
        y_max = std::max(y_max, other.y_max);
        z_min = std::min(z_min, other.z_min);
        z_max = std::max(z_max, other.z_max);
    }


};

std::ostream& operator << (std::ostream& out, const BoundingBox3d& box);

struct Node {
    int left_idx = -1;
    int right_idx = -1;
    int block_start = -1;
    int block_size = 0;
    BoundingBox3d box;

    friend std::ostream& operator << (std::ostream& out, const Node& node) {
        out << node.left_idx << ' ' << node.right_idx << ' ' << node.block_start << ' ' << node.block_size << ' ' << node.box;
        return out;
    }
};

template<typename T>
struct BVH {

    BVH(std::vector<T> primitives): primitives(primitives) {
        indices.resize(primitives.size());
        std::iota(indices.begin(), indices.end(), 0);
        tree.resize(0);
    }



    void build() {
        Node root;
        root.box = getBox(primitives.begin(), primitives.end());
        root.block_start = 0;
        root.block_size = primitives.size();
        tree.push_back(root);
        build_rec(0);
    }

    void build_rec(int v) {
        Node cur = tree[v];
        if (cur.block_size <= 2) {
            return;
        }
        std::vector<int> permutation(cur.block_size);
        std::iota(permutation.begin(), permutation.end(), 0);

        int axis = cur.box.longest_axis();
        int cnt_l = cur.block_size / 2;
        int cnt_r = cur.block_size - cnt_l;
        std::nth_element(permutation.begin(), permutation.begin() + cnt_l - 1, permutation.end(), [this, axis, cur](int a, int b){
            glm::vec3 pos_a{0};
            for (auto p : primitives[cur.block_start + a]) {
                pos_a += p;
            }
            pos_a /= primitives[cur.block_start + a].size();
            glm::vec3 pos_b{0};
            for (auto p : primitives[cur.block_start + b]) {
                pos_b += p;
            }
            pos_b /= primitives[cur.block_start + b].size();
            return pos_a[axis] < pos_b[axis];
        });
        std::vector<int> backup_i(indices.begin() + cur.block_start, indices.begin() + cur.block_size + cur.block_start);
        std::vector<T> backup_p(primitives.begin() + cur.block_start, primitives.begin() + cur.block_size + cur.block_start);
        for (int i = 0; i < permutation.size(); i++) {
            primitives[cur.block_start + i] = backup_p[permutation[i]];
            indices[cur.block_start + i] = backup_i[permutation[i]];
        }
        if (cnt_l > 0) {
            Node l_node;
            l_node.block_start = cur.block_start;
            l_node.block_size = cnt_l;
            l_node.box = getBox(primitives.begin() + l_node.block_start, primitives.begin() + l_node.block_start + l_node.block_size);
            cur.left_idx = tree.size();
            tree.push_back(l_node);
            build_rec(cur.left_idx);
        }
        if (cnt_r > 0) {
            Node r_node;
            r_node.block_start = cur.block_start + cnt_l;
            r_node.block_size = cnt_r;
            r_node.box = getBox(primitives.begin() + r_node.block_start, primitives.begin() + r_node.block_start + r_node.block_size);
            cur.right_idx = tree.size();
            tree.push_back(r_node);
            build_rec(cur.right_idx);
        }
        tree[v] = cur;
    }

    BoundingBox3d getBox(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end) {
        assert(begin != end);
        BoundingBox3d box {
            100000,
            -100000,
            100000,
            -100000,
            100000,
            -100000
        };
        while (begin != end) {
            box.update(*begin);
            ++begin;
        }
        return box;
    }



    void checkHit(int v, Ray r, std::function<void(int)> onHit) {
        if (v == -1) return;
        if (!tree[v].box.hasIntersection(r)) {
            return;
        }
        if (tree[v].block_size <= 2) {
            for (int i = tree[v].block_start; i < tree[v].block_start + tree[v].block_size; i++)
                onHit(indices[i]);
            return;
        }
        checkHit(tree[v].left_idx, r, onHit);
        checkHit(tree[v].right_idx, r, onHit);
    }


    std::vector<Node> tree;
    std::vector<T> primitives;
    std::vector<int> indices;
};