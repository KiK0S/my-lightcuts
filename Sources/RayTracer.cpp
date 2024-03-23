// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "RayTracer.h"

#include "Console.h"
#include "Camera.h"
#include "BRDF.hpp"
#include "LightCut.hpp"
#include "BVH.hpp"
#include <random>
#include <sstream>

RayTracer::RayTracer(bool useLightCuts, bool renderPreview) : 
	m_imagePtr (std::make_shared<Image>()), useLightCuts(useLightCuts), renderPreview(renderPreview) {}

RayTracer::~RayTracer() {}

void RayTracer::init (const std::shared_ptr<Scene> scenePtr) {
}

std::vector<BVH<std::vector<glm::vec3>>> bvh; 
std::vector<glm::vec3> framePos;
std::vector<glm::vec3> frameNormals;


void initBVH(const std::shared_ptr<Scene> scenePtr) {
	bvh.clear();
	auto camera = scenePtr->camera();
	for (int i = 0; i < scenePtr->numOfMeshes(); i++) {
		auto mesh = scenePtr->mesh(i)->mesh;
		framePos = mesh->vertexPositions();
		frameNormals = mesh->vertexNormals();

		auto triangles = mesh->triangleIndices();
		std::vector<std::vector<glm::vec3>> triPos;
		for (int i = 0; i < triangles.size(); i++) {
			triPos.push_back({framePos[triangles[i][0]], framePos[triangles[i][1]], framePos[triangles[i][2]]});
		}
		bvh.emplace_back(triPos);
		std::cout << "before build" << std::endl;
		bvh.back().build();
		std::cout << "after build" << std::endl;
	}
}

RayHit raySceneIntersectionBVH(Ray ray, const std::shared_ptr<Scene> scenePtr) {
	RayHit hit;
	ray.normalize();
	auto camera = scenePtr->camera();
	for (int i = 0; i < scenePtr->numOfMeshes(); i++) {
		auto mesh = scenePtr->mesh(i)->mesh;
		auto triangles = mesh->triangleIndices();
		
		
		bvh[i].checkHit(0, ray, [&](int idx){
			glm::vec3 p0 = framePos[triangles[idx][0]];
			glm::vec3 p1 = framePos[triangles[idx][1]];
			glm::vec3 p2 = framePos[triangles[idx][2]];

			glm::vec3 n0 = frameNormals[triangles[idx][0]];
			glm::vec3 n1 = frameNormals[triangles[idx][1]];
			glm::vec3 n2 = frameNormals[triangles[idx][2]];
			float t;
			if (rayTriangleIntersect(ray, p0, p1, p2, t) ) {
				RayHit new_hit;
				new_hit.brdf = BRDF(scenePtr->mesh(i)->material);
				glm::vec3 pos = ray.origin + ray.direction * t;
				glm::vec3 uvw = computeBarycentricCoordinates(pos, p0, p1, p2);
				new_hit.normal = n0 * uvw[0] + n1 * uvw[1] + n2 * uvw[2];
				new_hit.normal /= glm::length(new_hit.normal);
				new_hit.ray = ray;
				new_hit.t = t;
				hit.update(new_hit);
			}
		});
	}
	return hit;
}

LightTree lightCutTree;

void initLightCuts(const std::shared_ptr<Scene> scenePtr) {
	std::vector<std::shared_ptr<PointLight>> pls;
	for (int i = 0; i < scenePtr->numOfPLights(); i++) {
		pls.push_back(scenePtr->pLight(i));
	}
	lightCutTree.build(pls);
}


glm::vec3 GetPointLightNative(const std::shared_ptr<Scene> scenePtr, Ray ray, RayHit hit) {
	glm::vec3 res{0};
	for (int i = 0; i < scenePtr->numOfPLights(); i++) {
		auto light = scenePtr->pLight(i);
		glm::vec3 pos = ray.origin + ray.direction * hit.t;
		auto dir = pos - light->getTranslation();
		auto dirNorm = glm::length(dir);
		RayHit light_hit = raySceneIntersectionBVH(Ray{pos + dir * 0.001f, +dir}, scenePtr);
		if (light_hit.t == -1 || light_hit.t >= dirNorm - 0.001f) {
			res += hit.brdf(BRDFArgs{hit.normal, glm::normalize(-ray.direction), dir / dirNorm}) * light->color * light->intensity / dirNorm / dirNorm;
		}
	}
	return res;
}

glm::vec3 GetPointLightCuts(const std::shared_ptr<Scene> scenePtr, Ray ray, RayHit hit, bool print = false) {
	glm::vec3 pos = ray.origin + ray.direction * hit.t;
	glm::vec3 res{0};
	auto brdfArgs = BRDFArgs{hit.normal, glm::normalize(-ray.direction), glm::vec3{0.0f}};
	auto lights = lightCutTree.getLights(pos, hit.brdf, brdfArgs, print);
	for (auto light : lights) {
		auto dir = pos - light->getTranslation();
		auto dirNorm = glm::length(dir);
		RayHit light_hit = raySceneIntersectionBVH(Ray{pos + dir * 0.001f, +dir}, scenePtr);
		if (light_hit.t == -1 || light_hit.t >= dirNorm - 0.001f) {
			res += hit.brdf(BRDFArgs{hit.normal, glm::normalize(-ray.direction), dir / dirNorm}) * light->color * light->intensity / dirNorm / dirNorm;
		}
	}
	return res;
}

void RayTracer::render (const std::shared_ptr<Scene> scenePtr) {
	size_t width = m_imagePtr->width();
	size_t height = m_imagePtr->height();
	if (renderPreview) {
		width /= 4;
		height /= 4;
	}
	std::chrono::high_resolution_clock clock;
	Console::print ("Start ray tracing at " + std::to_string (width) + "x" + std::to_string (height) + " resolution...");
	std::chrono::time_point<std::chrono::high_resolution_clock> before = clock.now();
	m_imagePtr->clear (scenePtr->backgroundColor ());
 
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> w_dist(0, width);
    std::uniform_int_distribution<int> h_dist(0, height);
	initBVH(scenePtr);
	if (useLightCuts)
		initLightCuts(scenePtr);
	for (int w = 0; w < width - 1; w++) {
		for (int h = 0; h < height - 1; h++) {
			auto camera = scenePtr->camera();
			Ray ray = camera->rayAt((w + 0.5) / width, (h + 0.5) / height);
			RayHit hit = raySceneIntersectionBVH(ray, scenePtr);
			if (hit.t != -1) {
				(*m_imagePtr)(w, h) = glm::vec3(0.0, 0.0, 0.0);//hit.material->albedo * hit.material->ka;
				for (int i = 0; i < scenePtr->numOfLights(); i++) {
					auto light = scenePtr->light(i);
					glm::vec3 pos = ray.origin + ray.direction * hit.t;
					RayHit light_hit = raySceneIntersectionBVH(Ray{pos + light->direction * 0.1f, light->direction}, scenePtr);
					if (light_hit.t == -1) {
						(*m_imagePtr)(w, h) += hit.brdf(BRDFArgs{hit.normal, glm::normalize(-ray.direction), light->direction}) * light->color * light->intensity;
					}
				}
				if (useLightCuts)
					(*m_imagePtr)(w, h) += GetPointLightCuts(scenePtr, ray, hit);
				else
					(*m_imagePtr)(w, h) += GetPointLightNative(scenePtr, ray, hit);
			}

		}
	}
	std::chrono::time_point<std::chrono::high_resolution_clock> after = clock.now();
	double elapsedTime = (double)std::chrono::duration_cast<std::chrono::milliseconds>(after - before).count();
	Console::print ("Ray tracing executed in " + std::to_string(elapsedTime) + "ms");
}



