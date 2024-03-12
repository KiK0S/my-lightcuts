// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

/// A base class to represent spatially embedded entities in the scene. 
class Transform {
public:
	Transform () : m_translation (0.0), m_rotation (0.0), m_scale (1.0) {}
	virtual ~Transform () {}

	inline const glm::vec3 getTranslation () const { return m_translation; }
	inline void setTranslation (const glm::vec3 & t) { m_translation = t; }
	inline const glm::vec3 getRotation () const { return m_rotation; }
	inline void setRotation (const glm::vec3 & r) { m_rotation = r; }
	inline float getScale () const { return m_scale; }
	inline void setScale (float s) { m_scale = s; }

	inline glm::mat4 computeTransformMatrix () const {
		glm::mat4 id (1.0);
		glm::mat4 sm = glm::scale (id, glm::vec3 (m_scale));
		glm::mat4 rsm = glm::rotate (sm, m_rotation[0], glm::vec3 (1.0, 0.0, 0.0));
		rsm = glm::rotate (rsm, m_rotation[1], glm::vec3 (0.0, 1.0, 0.0));
		rsm = glm::rotate (rsm, m_rotation[2], glm::vec3 (0.0, 0.0, 1.0));
		glm::mat4 trsm = glm::translate (rsm, m_translation);
		return trsm;
	}

private:
	glm::vec3 m_translation;
	glm::vec3 m_rotation;
	float m_scale;
};
