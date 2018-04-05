#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

// Push Constant
layout(push_constant) uniform PushConsts
{
	layout(offset = 0)	vec2 position;
	layout(offset = 8)	float aspect;
	layout(offset = 12)	float zoom;
	layout(offset = 16)	int hexPerLine;
} pushConsts;

// Binding 0
layout(location = 0) in vec3 indxPos;
layout(location = 1) in vec3 indxCol;
layout(location = 2) in vec3 indxRot;
layout(location = 3) in vec3 indxSca;
layout(location = 4) in uint indxLightCount;

// Binding 1
layout(location = 5) in vec3 vertPos;

// Binding 2
layout(location = 6) in vec2 vertUv;
layout(location = 7) in vec3 vertNorm;
layout(location = 8) in vec3 vertCol;

// OUT
layout(location = 0) out vec3 fragColor;

// Descriptor 0
layout (binding = 0) uniform UBO 
{
	mat4 view;
	mat4 projection;
} ubo;

// Descriptor 1


void main()
{
	// ROTATION
	mat3 mx, my, mz;

	// rotate around x
	float s = sin(indxRot.x);
	float c = cos(indxRot.x);

	mx[0] = vec3(c, s, 0.0);
	mx[1] = vec3(-s, c, 0.0);
	mx[2] = vec3(0.0, 0.0, 1.0);
	
	// rotate around y
	s = sin(indxRot.y);
	c = cos(indxRot.y);

	my[0] = vec3(c, 0.0, s);
	my[1] = vec3(0.0, 1.0, 0.0);
	my[2] = vec3(-s, 0.0, c);
	
	// rot around z
	s = sin(indxRot.z);
	c = cos(indxRot.z);	
	
	mz[0] = vec3(1.0, 0.0, 0.0);
	mz[1] = vec3(0.0, c, s);
	mz[2] = vec3(0.0, -s, c);
	
	mat3 rotMat = mz * my * mx;

	mat4 gRotMat;
	s = sin(indxRot.y);
	c = cos(indxRot.y);
	gRotMat[0] = vec4(c, 0.0, s, 0.0);
	gRotMat[1] = vec4(0.0, 1.0, 0.0, 0.0);
	gRotMat[2] = vec4(-s, 0.0, c, 0.0);
	gRotMat[3] = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 locPos = vec3(vertPos.xyz * rotMat);
	vec3 pos = vec3((locPos.xyz) + indxPos);

	gl_Position = ubo.projection * ubo.view * gRotMat * vec4(pos, 1.0);

	fragColor = indxCol;
}