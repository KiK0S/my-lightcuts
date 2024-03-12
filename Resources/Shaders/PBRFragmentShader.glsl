#version 410 core // Minimal GL version support expected from the GPU

struct LightSource {
	vec3 direction;
	vec3 color;
	float intensity;
};
uniform LightSource lightSources[2];
uniform int lightCnt;
uniform mat4 projectionMat, modelViewMat, normalMat; // Uniform variables, set from the CPU-side main program

struct PointLightSource {
	vec3 position;
	vec3 color;
	float intensity;
};
uniform PointLightSource pointlightSources[2];
uniform int pointLightCnt;

in vec3 fNormal; // Shader input, linearly interpolated by default from the previous stage (here the vertex shader)
in vec3 fPosition;

out vec4 colorResponse; // Shader output: the color response attached to this fragment

float dothemi(vec3 a, vec3 b)
{
	return max(0.01, dot(a, b));
}

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a2 = roughness * roughness;
    float NdotH = dothemi(N, H);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float GeomShlick(vec3 N, vec3 w0, vec3 wi, float roughness) {
    float a0 = dothemi(N, w0);
    float ai = dothemi(N, wi);
    float k = roughness * sqrt(2.0 / PI);
    return a0 * ai / (a0 * (1 - k) + k) / (ai * (1 - k) + k);
}

vec3 GetLight(vec3 l, vec3 c, vec3 n, vec3 v, float roughness) {
	l = normalize(l);
	vec3 h = normalize(l + v);
	// sample albedo
	vec3 diffuse = vec3(0.6, 0.9, 0.4) / PI;
	
	vec3 F0 = vec3(1.0, 0.71, 0.29);
	F0 = mix(F0, diffuse, 0.3);

	vec3 F = FresnelSchlick(max(0.0, dothemi(h, l)), F0);
	float D = DistributionGGX(n, h, roughness);
	float G = GeomShlick(n, l, v, roughness);
	vec3 specular = F * D * G / 4.0 / dothemi(n, l) / dothemi(n, v); 
	return c * dot(n, l) * (diffuse + specular); 
}


void main () {
	vec3 n = normalize (fNormal);
	vec3 v = normalize (-fPosition);
	colorResponse = vec4(0.0, 0.0, 0.0, 0.0);
	for (int i=0; i < lightCnt; i++) {
		LightSource lightSource = lightSources[i];
		colorResponse += lightSource.intensity * vec4(GetLight(lightSource.direction, lightSource.color, n, v, 0.1), 1.0);
	}
	for (int i=0; i < pointLightCnt; i++) {
		PointLightSource lightSource = pointlightSources[i];
		vec3 lightDirection = fPosition - lightSource.position;
		colorResponse += lightSource.intensity * vec4(GetLight(normalize(lightDirection), lightSource.color, n, v, 0.1), 1.0) / dot(lightDirection, lightDirection);
	}
}