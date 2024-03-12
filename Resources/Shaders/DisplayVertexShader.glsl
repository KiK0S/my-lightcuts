#version 410 core // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the screen-space position (CPU side: glVertexAttrib 0)

out vec2 fTexCoord;

void main() {
    gl_Position =  vec4 (vPosition, 1.0);; // mandatory to fire rasterization properly
    fTexCoord = vPosition.xy/2.0 + vec2 (0.5);
}