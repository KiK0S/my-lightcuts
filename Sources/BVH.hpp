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
#include "BoundingBox.hpp"

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



    void checkHit(int v, const Ray& r, std::function<void(int)> onHit) {
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