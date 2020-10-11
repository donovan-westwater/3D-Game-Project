#version 450
#extension GL_ARB_separate_shader_objects : enable
/*
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
*/
out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;


layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 outPos;
layout(location = 3) out vec4 mCenter;
layout(location = 4) out vec2 uv;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
void main()
{
    //mCenter = ubo.proj * ubo.view * ubo.model * vec4(0,0,0, 1.0);
    //uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    //gl_Position = vec4(uv * 2.0f + -1.0f, 0.0f, 1.0f);
    mCenter = vec4(0.0,0,0,1.0);
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    outPos = gl_Position.xyz;
}

