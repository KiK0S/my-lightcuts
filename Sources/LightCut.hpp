#pragma once
#include "BVH.hpp"
#include "LightSource.hpp"
#include <vector>
#include <set>
#include <queue>

struct LightTree {

    LightTree() {}

    void build(std::vector<PointLight> lights_) {
        this->lights = lights_;
        indices.resize(lights.size());
        std::iota(indices.begin(), indices.end(), 0);
        tree.resize(0);
        for (int i = 0; i < lights.size(); i++) {
            Node node;
            node.block_start = i;
            node.block_size = 1;
            node.box = BoundingBox3d {
                100000,
                -100000,
                100000,
                -100000,
                100000,
                -100000
            };
            node.box.update(lights[i].getTranslation());
            tree.push_back(node);
        }
        std::vector<int> active_clusters = indices;
        auto node = [&](int i) {
            return tree[active_clusters[i]];
        };
        auto score = [&](int i, int j) {
            return glm::length(lights[node(i).block_start].getTranslation() -  lights[node(j).block_start].getTranslation())
                        * (lights[node(i).block_start].intensity + lights[node(j).block_start].intensity);
        };
        while (active_clusters.size() > 1) {
            int cur_i = 0;
            int cur_j = 1;
            double cur_tmp = 1e18;
            for (int i = 0; i < active_clusters.size(); i++) {
                for (int j = i + 1; j < active_clusters.size(); j++) {
                    if (score(i, j) < cur_tmp) {
                        cur_i = i;
                        cur_j = j;
                        cur_tmp = score(i, j);
                    }
                }
            }
            Node united;
            united.left_idx = active_clusters[cur_i];
            united.right_idx = active_clusters[cur_j];
            united.box = node(cur_i).box;
            united.box.update(node(cur_j).box);
            // PointLight newLight(0.5f * (lights[active_clusters[cur_i]].getTranslation() + lights[active_clusters[cur_j]].getTranslation()),
            //                     0.5f * (lights[active_clusters[cur_i]].color + lights[active_clusters[cur_j]].color),
            //                     lights[active_clusters[cur_i]].intensity + lights[active_clusters[cur_j]].intensity);
            united.block_size = 1;
            united.block_start = rand() % 2 == 0 ? node(cur_i).block_start : node(cur_j).block_start;
            // lights.push_back(newLight);
            active_clusters.push_back(tree.size());
            tree.push_back(united);
            active_clusters.erase(active_clusters.begin() + cur_j);
            active_clusters.erase(active_clusters.begin() + cur_i);
        }
    }

    glm::vec3 getLight(PointLight light, glm::vec3 position, glm::vec3 direction, glm::vec3 normal, glm::vec3 albedo) {
        auto dir = position - light.getTranslation();
		auto dirNorm = glm::length(dir);
        return GetLight(dir / dirNorm, light.color, normal, glm::normalize(-position), albedo, 0.1) * light.intensity / dirNorm / dirNorm;
    }

    std::vector<PointLight> getLights(glm::vec3 position, glm::vec3 direction,  glm::vec3 normal, glm::vec3 albedo, glm::mat4 viewMat, bool print = false) {
        int root = tree.size() - 1;
        auto estimate_error = [&](Node node) { 
            glm::vec3 cur_estimate = getLight(lights[node.block_start], position, direction, normal, albedo);
            glm::vec3 child_estimates = getLight(lights[tree[node.left_idx].block_start], position, direction, normal, albedo) + 
                                    getLight(lights[tree[node.right_idx].block_start], position, direction, normal, albedo);
            return glm::length(cur_estimate - child_estimates);
        };
        auto cmp = [&](int i, int j) {
            Node a = tree[i];
            Node b = tree[j];
            
            if (a.left_idx == -1) {
                if (b.left_idx == -1) {
                    return i < j;
                }
                return false;
            }
            if (b.left_idx == -1) {
                return true;
            }
            return estimate_error(a) > estimate_error(b);
        };
        std::set<int, decltype(cmp)> s(cmp);
        s.insert(root);
        while(s.size() < 1000) {
            
            int node = *s.begin();
            if (print) {
                std::cout << node << '\n';
            }
            if (tree[node].left_idx == -1) {
                // std::cout << "found leaf\n";
                break;
            }
            if (estimate_error(tree[node]) < 0.01) {
                break;
            }
            s.erase(node);
            s.insert(tree[node].left_idx);
            s.insert(tree[node].right_idx);
        }
        if (print)
            std::cout << "===\n";
        std::vector<PointLight> res;
        for (int idx : s) {
            if (print)
                std::cout << tree[idx].block_start << ' ';
            res.push_back(lights[tree[idx].block_start]);
        }
        if (print)
            std::cout << '\n';
        return res;
    }

    std::vector<PointLight> getLightsOld(glm::vec3 position, glm::vec3 direction,  glm::vec3 normal, glm::vec3 albedo, glm::mat4 viewMat) {
        int root = tree.size() - 1;
        std::queue<int> q;
        std::vector<PointLight> res;
        q.push(root);
        while(!q.empty()) {
            int v = q.front();
            q.pop();
            Node node = tree[v];
            if (node.left_idx == -1) {
                res.push_back(lights[node.block_start]);
                continue;
            }
            glm::vec3 cur_estimate = getLight(lights[node.block_start], position, direction, normal, albedo);
            glm::vec3 child_estimates = getLight(lights[tree[node.left_idx].block_start], position, direction, normal, albedo) + 
                                        getLight(lights[tree[node.right_idx].block_start], position, direction, normal, albedo);
            if (glm::length(cur_estimate - child_estimates) < 0.00) {
                res.push_back(lights[node.block_start]);
            } else {
                q.push(node.left_idx);
                q.push(node.right_idx);
            }
        }
        return res;
    }

    std::vector<Node> tree;
    std::vector<PointLight> lights;
    std::vector<int> indices;
};