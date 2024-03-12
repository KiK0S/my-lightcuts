#version 410 core // Minimal GL version support expected from the GPU

uniform sampler2D imageTex;

in vec2 fTexCoord;

out vec4 colorResponse; // Shader output: the color response attached to this fragment. here the simlple content of the bounded image texture

void main () {
	colorResponse = vec4 (texture(imageTex, fTexCoord).rgb, 1.0);	
}