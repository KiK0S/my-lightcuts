// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <random>
#include <cmath>
#include <algorithm>
#include <limits>
#include <memory>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Image.h"
#include "Scene.h"
#include "LightCut.hpp"

using namespace std;

class RayTracer {
public:
	
	RayTracer(bool useLightCuts = false, bool renderPreview = false, bool lightCutsSampling = false);
	virtual ~RayTracer();

	inline void setResolution (int width, int height) { m_imagePtr = make_shared<Image> (width, height); }
	inline std::shared_ptr<Image> image () { return m_imagePtr; }
	void init (const std::shared_ptr<Scene> scenePtr);
	void render (const std::shared_ptr<Scene> scenePtr);
	void initLightCuts(const std::shared_ptr<Scene> scenePtr);
	glm::vec3 GetPointLightCuts(const std::shared_ptr<Scene> scenePtr, Ray ray, RayHit hit, bool print = false);

	bool useLightCuts;
	bool renderPreview;
	bool lightCutsSampling;
	int sumLightsPerRay = 0;
	int cntLightsPerRay = 0;

private:
	std::shared_ptr<Image> m_imagePtr;
};
