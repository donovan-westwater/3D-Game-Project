#version 450
#extension GL_ARB_separate_shader_objects : enable

#define STEPS 64
#define STEP_SIZE 0.01
#define MIN_DISTANCE 0.1
//Was 0.1
//This entire scene exists in "model space" without any transformations

layout(binding = 0) uniform UniformBufferObject {
    mat4 model; //Useless
    mat4 view; //Camera
    mat4 proj; //Perspective of camera
    vec2 resolution;
} ubo;

//layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 inPos;
layout(location = 3) in vec4 mPos;

layout(location = 0) out vec4 outColor;


//last owrking rad = 0.2
float radius = 0.5;//2; //5
vec3 centre = mPos.xyz;//(ubo.view*mPos).xyz;
vec3 lightVector = vec3(0,1,0); //vec3(0,0,1);
vec3 cameraVector = vec3(0,0,1);
vec3 vDirection = vec3(0,0,0);

bool sphereHit(vec3 p) {
    return distance(p, centre) < radius;
}
float sphereDistance(vec3 p) {
    return distance(p, centre) - radius;
}
 vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sphereDistance(vec3(p.x + MIN_DISTANCE, p.y, p.z)) - sphereDistance(vec3(p.x - MIN_DISTANCE, p.y, p.z)),
        sphereDistance(vec3(p.x, p.y + MIN_DISTANCE, p.z)) - sphereDistance(vec3(p.x, p.y - MIN_DISTANCE, p.z)),
        sphereDistance(vec3(p.x, p.y, p.z  + MIN_DISTANCE)) - sphereDistance(vec3(p.x, p.y, p.z - MIN_DISTANCE))
    ));
}

vec4 simpleLambert(vec3 normal,float specPower) {
    //vec3 viewDirection = inPos-cameraVector;
    vec3 viewDirection = vDirection;
    vec3 lightDir = lightVector;

    float NdotL = max(dot(normal, lightDir), 0);
    vec4 c = vec4(1,0,0,1);
    // Specular
    vec3 h = (lightDir - viewDirection) / 2.;
    float s = pow(dot(normal, h), specPower);
    c = c*(NdotL+s)*vec4(1,1,1,1);
    c.w = 1;
    return c;
}
vec4 renderSurface(vec3 p){
    vec3 n = estimateNormal(p);
    return simpleLambert(n,10);
}
vec4 raymarch(vec4 position, vec4 direction) {
    for (int i = 0; i < STEPS; i++) {
        vec3 tmp = position.xyz;
        float distance = sphereDistance(tmp);//sphereDistance(position);
        if (distance < MIN_DISTANCE){
            //vec4 retColor = (1-(i / float(STEPS))) * vec4(1,0,0,1);
            //retColor.w = 1;
            return renderSurface(tmp);//renderSurface(position);//retColor; //1-(i / float(STEPS))
        }
        position += direction * distance;//STEP_SIZE;
    }
    return vec4(1,1,1,1); //was originally 1
}
 
 //Currently UBO doesnt work, leading to crashes in the main game
void main()
{
    vec3 wPos = vec3(fragTexCoord.x,fragTexCoord.y,0);
   // vec3 lightVector = vec3(0,0,1);
    //vec3 cameraVector = vec3(0,0,1);
    //vec3 surNorm = estimateNormal(inPos);
    //float cosTheta = dot( surNorm,lightVector );
   // vec4 baseColor = texture(texSampler, fragTexCoord);
    //outColor = baseColor + baseColor * cosTheta;
    //outColor.w = baseColor.w;
    //New code below
    vec2 uv = (gl_FragCoord.xy - 0.5*ubo.resolution)/ubo.resolution.y;
    vec3 col = vec3(0,10.0*uv.x,-10.0*uv.y);
    outColor = vec4(col,1);

    cameraVector = vec3(ubo.view[0][3],ubo.view[1][3],ubo.view[2][3]);
    vec4 modSpace = vec4(uv.x,uv.y,4,0);
    vec4 viewDirection = ubo.view*normalize(modSpace);//normalize(inPos-cameraVector);
    vDirection = viewDirection.xyz;
    //outColor = raymarch(vec4(cameraVector.xyz,1),viewDirection);//raymarch(cameraVector,viewDirection);

}
