#version 450
#extension GL_ARB_separate_shader_objects : enable

#define STEPS 64
#define STEP_SIZE 0.01
#define MIN_DISTANCE 0.1

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 inPos;
layout(location = 3) in vec4 mPos;

layout(location = 0) out vec4 outColor;

float radius = 2; //5
vec3 centre = mPos.xyz;

bool sphereHit(vec3 p) {
    return distance(p, centre) < radius;
}
float sphereDistance(vec3 p) {
    return distance(p, centre) - radius;
}
            
vec4 raymarch(vec3 position, vec3 direction) {
    for (int i = 0; i < STEPS; i++) {
        float distance = sphereDistance(position);
        if (distance < MIN_DISTANCE){
            vec4 retColor = (1-(i / float(STEPS))) * vec4(1,0,0,1);
            retColor.w = 1;
            return retColor; //1-(i / float(STEPS))
        }
        position += direction * STEP_SIZE;
    }
    return vec4(1,1,1,1); //was originally 1
}
 


void main()
{
    vec3 wPos = vec3(fragTexCoord.x,fragTexCoord.y,0);
    vec3 lightVector = vec3(0,0,1);
    float cosTheta = dot( fragNormal,lightVector );
    vec4 baseColor = texture(texSampler, fragTexCoord);
    //outColor = baseColor + baseColor * cosTheta;
    //outColor.w = baseColor.w;
    //New code below
    vec3 viewDirection = normalize(inPos-lightVector);
    outColor = raymarch(inPos,viewDirection);

}
