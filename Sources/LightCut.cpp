#include "LightCut.hpp"

#include <vector>
#include <set>
#include <memory>
#include "Random.hpp"
#include <queue>

void LightTree::build(std::vector<std::shared_ptr<PointLight>> lights_) {
    this->lights = lights_;
    tree.resize(0);
    for (int i = 0; i < lights.size(); i++) {
        LightCutNode node;
        node.light_idx = i;
        node.intensity = lights[i]->intensity;
        node.box = BoundingBox3d {
            100000,
            -100000,
            100000,
            -100000,
            100000,
            -100000
        };
        node.box.update(lights[i]->getTranslation());
        tree.push_back(node);
    }
    std::vector<int> active_clusters(lights.size());
    std::iota(active_clusters.begin(), active_clusters.end(), 0);
    auto node = [&](int i) {
        return tree[active_clusters[i]];
    };
    auto score = [&](int i, int j) {
        BoundingBox3d bb = node(i).box;
        bb.update(node(j).box);
        auto dx = bb.x_max - bb.x_min;
        auto dy = bb.y_max - bb.y_min;
        auto dz = bb.z_max - bb.z_min;
        return (dx * dx + dy * dy + dz * dz)
                    * (lights[node(i).light_idx]->intensity + lights[node(j).light_idx]->intensity);
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
        LightCutNode united;
        united.left_idx = active_clusters[cur_i];
        united.right_idx = active_clusters[cur_j];
        united.box = node(cur_i).box;
        united.box.update(node(cur_j).box);
        if (rand_between(0, node(cur_i).intensity + node(cur_j).intensity) < node(cur_i).intensity) {
            united.light_idx = node(cur_i).light_idx;
        } else {
            united.light_idx = node(cur_j).light_idx;
        }
        united.intensity = node(cur_i).intensity + node(cur_j).intensity;
        active_clusters.push_back(tree.size());
        tree.push_back(united);
        active_clusters.erase(active_clusters.begin() + cur_j);
        active_clusters.erase(active_clusters.begin() + cur_i);
    }
    // std::queue<int> q;
    // q.push(tree.size() - 1);
    // while (!q.empty()) {
    //     int v = q.front();
    //     q.pop();
    //     // std::cout << v << ' ';
    //     if (tree[v].left_idx == -1) {
    //         // std::cout << '\n';
    //         continue;
    //     }
    //     // std::cout << tree[v].left_idx << ' ' << tree[v].right_idx << '\n';
    //     q.push(tree[v].left_idx);
    //     q.push(tree[v].right_idx);
    // }
}

glm::vec3 LightTree::getLight(std::shared_ptr<PointLight> light, glm::vec3 position, const BRDF& brdf, BRDFArgs& args) {
    auto dir = light->getTranslation() - position;
    auto dirNorm = glm::length(dir);
    args.lightDir = glm::normalize(dir);
    return light->color * light->intensity * brdf(args) / dirNorm / dirNorm;
}

std::shared_ptr<PointLight> LightTree::selectLightNode(LightCutNode node, double rnd) {
    if (node.left_idx == -1) {
        return std::make_shared<PointLight>(*lights[node.light_idx]);
    }
    auto res = std::make_shared<PointLight>(*lights[node.light_idx]);
    res->intensity = node.intensity;
    return res;
    // if (rnd < 0) {
    //     rnd = rand_between(0, node.intensity);
    // }
    // if (rnd < tree[node.left_idx].intensity) {
    //     node.light_idx = tree[node.left_idx].light_idx;
    //     auto res = selectLightNode(tree[node.left_idx], rnd);
    //     res->intensity = node.intensity;
    //     return res;
    // } else {
    //     node.light_idx = tree[node.right_idx].light_idx;
    //     auto res =  selectLightNode(tree[node.right_idx], rnd - tree[node.left_idx].intensity);
    //     res->intensity = node.intensity;
    //     return res;
    // }
}

std::vector<std::shared_ptr<PointLight>> LightTree::getLights(glm::vec3 position, const BRDF& brdf, BRDFArgs& args, bool print) {
    float PI = glm::pi<float>();
    int root = tree.size() - 1;
    auto get_cos_bound = [&](BoundingBox3d bb, glm::vec3 a) {
        bb.x_min -= position[0];
        bb.x_max -= position[0];
        bb.y_min -= position[1];
        bb.y_max -= position[1];
        bb.z_min -= position[2];
        bb.z_max -= position[2];
        auto b = glm::vec3(0.0, 0.0, 1.0);
        glm::vec3 axis = glm::cross(a, b);
        float angle = std::acos(glm::dot(glm::normalize(a), glm::normalize(b)));

        // Construct rotation matrix
        glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

        if ((rotationMatrix * a).z < 0) {
            rotationMatrix = glm::rotate(glm::mat4(1.0f), -angle, axis);
        }
        auto newBB = bb.afterRotation(rotationMatrix);
        

        auto z_max = std::max(newBB.z_max, newBB.z_min);
        if (z_max <= 0) return 0.0f;
        auto x_min = std::min(std::abs(newBB.x_min), std::abs(newBB.x_max));
        if (bb.x_max >= 0 && bb.x_min <= 0) x_min = 0.0;
        auto y_min = std::min(std::abs(newBB.y_min), std::abs(newBB.y_max));
        if (bb.y_max >= 0 && bb.y_min <= 0) y_min = 0.0;
        return z_max / std::sqrt(x_min * x_min + y_min * y_min + z_max * z_max);
    };

    auto estimate_error = [&](LightCutNode node) { 
        if (node.left_idx == -1) {
            return glm::vec3(-1.0f);
        }

        auto light = selectLightNode(node); 
        auto length = glm::length(light->getTranslation() - position);
        float g = 0;
        for (int i = 0; i < 3; i++)  {
            if (position[i] < node.box.min_axis(i)) {
                g += (node.box.min_axis(i) - position[i]) * (node.box.min_axis(i) - position[i]);
            } else if (position[i] > node.box.max_axis(i)) {
                g += (position[i] - node.box.max_axis(i)) * (position[i] - node.box.max_axis(i));
            }
        }
        if (g < 0.01f) {
            return glm::vec3(1e18f);
        }
        g = 1.0 / g;
        glm::vec3 diffuse = brdf.diffuse(args);

        float dot_bound = get_cos_bound(node.box, args.normal);
        glm::vec3 r = glm::dot(args.cameraDir, args.normal) * 2 * args.normal - args.cameraDir;

        float other_dot_bound = get_cos_bound(node.box, r);
        glm::vec3 brdf_diffuse = brdf.material->kd * glm::vec3(brdf.material->albedo) / PI;
        glm::vec3 brdf_specular = brdf.material->ks * glm::vec3(1.0) * 3.0f / PI * other_dot_bound;
        float v = 1.0f;
        glm::vec3 m = dot_bound * (brdf_diffuse + brdf_specular);
        auto res = node.intensity * g * v * m;
        for (int i = 0; i < 3; i++) {
            res[i] = std::abs(res[i]);
        }
        return res;// + glm::vec3(1.0) * glm::dot(r, direction));  

        // float m = ;
        // glm::vec3 cur_estimate = getLight(lights[node.block_start], position, direction, normal, albedo);
        // glm::vec3 child_estimates = getLight(lights[tree[node.left_idx].block_start], position, direction, normal, albedo) + 
        //                         getLight(lights[tree[node.right_idx].block_start], position, direction, normal, albedo);
        // return 1.0 - glm::length(cur_estimate) / glm::length(child_estimates);
    };
    glm::vec3 illumination = getLight(selectLightNode(tree[root]), position, brdf, args);
    float coeff = 0.02;
    auto max_comp = [](glm::vec3 v) {
        return std::max(v[0], std::max(v[1], v[2]));
    };
    auto cmp = [&](int i, int j) {
        LightCutNode a = tree[i];
        LightCutNode b = tree[j];
        
        if (a.left_idx == -1) {
            if (b.left_idx == -1) {
                return i > j;
            }
            return true;
        }
        if (b.left_idx == -1) {
            return false;
        }
        auto ei = max_comp(estimate_error(a));
        auto ej = max_comp(estimate_error(b));
        if (ei == ej) {
            // node with less number is higher
            return i > j;
        }
        return ei > ej;
    };
    std::vector<int> s;
    s.push_back(root);

    while(s.size() < 200) {
        
        std::pop_heap(s.begin(), s.end(), cmp);
        int node = s.back();
        s.pop_back();
        if (tree[node].left_idx == -1) {
            // std::cout << "found leaf\n";
            break;
        }
        auto err_est = estimate_error(tree[node]);
        if (err_est[0] <= coeff * illumination[0] && err_est[1] <= coeff * illumination[1] && err_est[2] <= coeff * illumination[2]) {
            // std::cerr << err_est[0] << ' ' << err_est[1] << ' ' << err_est[2] << std::endl;
            break;
        }
        s.push_back(tree[node].left_idx);
        std::push_heap(s.begin(), s.end(), cmp);
        s.push_back(tree[node].right_idx);
        std::push_heap(s.begin(), s.end(), cmp);
        if (tree[node].light_idx == tree[tree[node].left_idx].light_idx) {
            illumination += getLight(selectLightNode(tree[tree[node].right_idx]), position, brdf, args);
        } else {
            illumination += getLight(selectLightNode(tree[tree[node].left_idx]), position, brdf, args);
        }
    }
    std::vector<std::shared_ptr<PointLight>> res;
    if (print)
        std::cout << s.size();

    for (int idx : s) {
        res.push_back(selectLightNode(tree[idx]));
    }
    return res;
}