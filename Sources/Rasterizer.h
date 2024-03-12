// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <glad/glad.h>
#include <string>

#include "Scene.h"
#include "Mesh.h"
#include "Image.h"
#include "ShaderProgram.h"

class Rasterizer {
public:

	inline Rasterizer () {}

	virtual ~Rasterizer () {}

	/// OpenGL context, shader pipeline initialization and GPU ressources (vertex buffers, textures, etc)
	void init (const std::string & basepath, const std::shared_ptr<Scene> scenePtr);
	void setResolution (int width, int height);
	void updateDisplayedImageTexture (std::shared_ptr<Image> imagePtr);
	void initDisplayedImage ();
	/// Loads and compile the programmable shader pipeline
	void loadShaderProgram (const std::string & basePath);
	void render (std::shared_ptr<Scene> scenePtr);
	void display (std::shared_ptr<Image> imagePtr);
	void clear ();

private:
	GLuint genGPUBuffer (size_t elementSize, size_t numElements, const void * data);
	GLuint genGPUVertexArray (GLuint posVbo, GLuint ibo, bool hasNormals, GLuint normalVbo);
	GLuint toGPU (std::shared_ptr<Model> meshPtr);
	void initScreeQuad ();
	void draw (size_t meshId, size_t triangleCount);

	/// Pointer to GPU shader pipeline i.e., set of shaders structured in a GPU program
	std::shared_ptr<ShaderProgram> m_pbrShaderProgramPtr; // A GPU program contains at least a vertex shader and a fragment shader
	std::shared_ptr<ShaderProgram> m_displayShaderProgramPtr; // Full screen quad shader program, for displaying 2D color images
	GLuint m_displayImageTex; // Texture storing the image to display in non-rasterization mode
	GLuint m_screenQuadVao;  // Full-screen quad drawn when displaying an image (no scene rasterization) 

	std::vector<GLuint> m_vaos;
	std::vector<GLuint> m_posVbos;
	std::vector<GLuint> m_normalVbos;
	std::vector<GLuint> m_ibos;
};