#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
	vec4 gl_Position;
};

layout(push_constant) uniform PushConsts
{
	layout(offset = 0)	vec2 position;
	layout(offset = 8)	float aspect;
	layout(offset = 12)	float zoom;
	layout(offset = 16)	int hexPerLine;
} pushConsts;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertUv;
layout(location = 2) in vec3 vertNorm;
layout(location = 3) in vec3 vertCol;

layout(location = 0) out vec3 fragColor;

layout (binding = 0) uniform UBO 
{
	mat4 view;
	mat4 projection;
} ubo;

void main()
{
	gl_Position = 
	ubo.projection * 
	ubo.view * 
	vec4(vertPos.x, vertPos.y, vertPos.z, 1.0);

	fragColor = vertCol;
}