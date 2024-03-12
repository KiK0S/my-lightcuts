// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "Camera.h"
#include "Mesh.h"
#include "Model.hpp"
#include "LightSource.hpp"

class Scene {
public:
	inline Scene () : m_backgroundColor (0.f, 0.f ,0.f) {}
	virtual ~Scene() {}

	inline const glm::vec3 & backgroundColor () const { return m_backgroundColor; }

	inline void setBackgroundColor (const glm::vec3 & color) { m_backgroundColor = color; }
 
	inline void set (std::shared_ptr<Camera> camera) { m_camera = camera; }

	inline const std::shared_ptr<Camera> camera() const { return m_camera; }

	inline std::shared_ptr<Camera> camera() { return m_camera; }

	inline void add (std::shared_ptr<Model> mesh) { m_meshes.push_back (mesh); }

	inline void add (std::shared_ptr<DirectionalLight> lightSource) { m_lights.push_back (lightSource); }
	inline void add (std::shared_ptr<PointLight> lightSource) { m_plights.push_back (lightSource); }

	inline const std::shared_ptr<Model> mesh (size_t index) const { return m_meshes[index]; }

	inline std::shared_ptr<Model> mesh (size_t index) { return m_meshes[index]; }
	inline std::shared_ptr<DirectionalLight> light (size_t index) { return m_lights[index]; }
	inline std::shared_ptr<PointLight> pLight (size_t index) { return m_plights[index]; }


	inline size_t numOfMeshes() {return m_meshes.size();}
	inline size_t numOfLights() {return m_lights.size();}
	inline size_t numOfPLights() {return m_plights.size();}

	inline void clear () {
		m_camera.reset ();
		m_meshes.clear ();
		m_lights.clear ();
	}

private:
	glm::vec3 m_backgroundColor;
	std::shared_ptr<Camera> m_camera;
	std::vector<std::shared_ptr<DirectionalLight>> m_lights;
	std::vector<std::shared_ptr<PointLight>> m_plights;
	std::vector<std::shared_ptr<Model> > m_meshes;
};