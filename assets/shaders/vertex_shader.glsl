#version 330

uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;
uniform mat4 mModel_InverseTranspose;

uniform int colourMode;
uniform vec4 colourOverride;
uniform int hasVertexColour;

uniform vec2 yMin_yMax;
uniform vec3 seed;

layout (location=0) in vec4 vPos;
layout (location=1) in vec4 vNorm;
layout (location=2) in vec4 vCol;
layout (location=3) in vec2 vTextCoords;

out vec4 vertColor;
out vec4 vertNormal;
out vec4 vertWorldPosition;
out vec2 vertTextCoords;

float hash13(vec3 p){
    p = fract(p * 0.1);
    p += dot(p, p.yzx + 35.0);
    return fract((p.x + p.y) * p.z);
}
vec3 randColour(vec3 p, vec3 s){	
    return vec3(
        hash13(p + s.xxx),
        hash13(p + s.yyy),
        hash13(p + s.zzz)
    );
}

void main() {
    gl_Position = (mProj * mView * mModel) * vec4(vPos.xyz, 1.0);

	vertWorldPosition = mModel * vec4(vPos.xyz, 1.0);
	vertNormal        = mModel_InverseTranspose * vec4(vNorm.xyz, 0.0);
	vertNormal.xyz    = normalize(vertNormal.xyz);
	vertTextCoords    = vTextCoords;

	if(colourMode == 0){ // Solid
		vertColor = colourOverride;
	} else if (colourMode == 1) { // Random
		vertColor = vec4(randColour(vPos.xyz, seed), 1.0);
	} else if (colourMode == 2) { // Gradient
		float range = yMin_yMax.y - yMin_yMax.x;
		float normalizedY = clamp(((range == 0.0) ? 0.0 : (vPos.y - yMin_yMax.x) / range), 0.0, 1.0);

		const vec3 colors[5] = vec3[5](
			vec3(1.0f, 0.0f, 0.0f),  // Red 
			vec3(1.0f, 0.5f, 0.0f),  // Orange
			vec3(1.0f, 1.0f, 0.0f),  // Yellow
			vec3(0.0f, 1.0f, 0.0f),  // Green
			vec3(0.0f, 0.0f, 1.0f)   // Blue 
		);

		float scaled = normalizedY * 4.0;
		int bandIndex = int(floor(scaled));
		bandIndex = clamp(bandIndex, 0, 3);

		float localT = scaled - float(bandIndex);

		vec3 color1 = colors[bandIndex];
		vec3 color2 = colors[bandIndex + 1];
		vertColor = vec4(mix(color1, color2, localT), 1.0);
	} else if (colourMode == 3 && hasVertexColour == 1) { // PLY Colour 
		vertColor = vCol;
	} else vertColor = vec4(1.0);
};