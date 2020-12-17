#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    vec2 frame_offset;
} ubo;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 0) out vec2 fragTexCoord;
vec2 vertices[] = vec2[](
    vec2(-1, -1), vec2(-1 + 0.5, -1),
    vec2(-1, -1 + 0.5), vec2(-1 + 0.5, -1 + 0.5)
    );
    /*
    vec2 vertices[] = vec2[](
    vec2(-0.5, -0.5), vec2(0.5, -0.5),
    vec2(-0.5, 0.5), vec2(0.5, 0.5)
    );
    */
void main()
{
    gl_Position = ubo.model*vec4(vertices[gl_VertexIndex], 1.0, 1.0);
    //gl_Position = ubo.model * vec4(inPosition,0.0, 1.0);
    fragTexCoord = inTexCoord + ubo.frame_offset;
}