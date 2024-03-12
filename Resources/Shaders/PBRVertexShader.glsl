#version 410 core // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vNormal;

uniform mat4 projectionMat, modelViewMat, normalMat; // Uniform variables, set from the CPU-side main program

out vec3 fNormal;
out vec3 fPosition;

void main() {
	vec4 p = modelViewMat * vec4 (vPosition, 1.0);
    gl_Position =  projectionMat * p; // mandatory to fire rasterization properly
    fPosition = vec3(p);
    vec4 n = normalMat * vec4 (normalize (vNormal), 1.0);
    fNormal = normalize (n.xyz);
}
