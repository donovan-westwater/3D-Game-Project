#version 450
#extension GL_ARB_separate_shader_objects : enable

#define STEPS 128
#define STEP_SIZE 0.01
#define MIN_DISTANCE 0.1
//Was 0.1
//This entire scene exists in "model space" without any transformations
struct EntityRender_S { //Stores the infomation needed by the shader
    vec4 position;
    vec4 rotation;
    vec4 scale;
    vec4 color;
    int id;
}EntityRender;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model; //Useless
    mat4 view; //Camera
    mat4 proj; //Perspective of camera
    vec2 resolution;
    EntityRender_S renderList[50];
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
vec3 lightVector = vec3(0,-1,0); //vec3(0,0,1);
vec3 cameraVector = vec3(0,0,1);
vec3 vDirection = vec3(0,0,0);
int currentEnt = -1;
//***Transformations***
//Copied from Marble Marcher!
void rotX(inout vec4 z, float s, float c) {
	z.yz = vec2(c*z.y + s*z.z, c*z.z - s*z.y);
}
void rotY(inout vec4 z, float s, float c) {
	z.xz = vec2(c*z.x - s*z.z, c*z.z + s*z.x);
}
void rotZ(inout vec4 z, float s, float c) {
	z.xy = vec2(c*z.x + s*z.y, c*z.y - s*z.x);
}
void rotX(inout vec4 z, float a) {
	rotX(z, sin(a), cos(a));
}
void rotY(inout vec4 z, float a) {
	rotY(z, sin(a), cos(a));
}
void rotZ(inout vec4 z, float a) {
	rotZ(z, sin(a), cos(a));
}


//***SDFs for various primatives
float sphereSDF(vec3 p) {
    return length(p) - radius;
}
float sphereDistance(vec3 p) {
    return distance(p, centre) - radius;
}
//***Scene SDF***
float sceneSDF(vec3 p){
    vec4 pD = vec4(p,1);
    vec3 scale;
    float d = 99999;
    float prevD = 0;
    for(int i = 0;i<50;i++){
        if(ubo.renderList[i].id < 0) continue;
        //Transforms
        pD.xyz -= ubo.renderList[i].position.xyz;
        rotZ(pD,radians(ubo.renderList[i].rotation.z));
        scale = ubo.renderList[i].scale.xyz;
        switch(i){
            case 0: //Sphere
                d = min(sphereSDF(pD.xyz/scale)*min(scale.x,min(scale.y,scale.z)),d);
                break;
            case 1: //Box
                break;
            case 2: //Other
                break;
        }
        if(abs(d - prevD) > 0.0001) currentEnt = i;
        prevD = d;
    }
    d = min(d,sphereDistance(p));
    return d;
}
//***Lighting***
 vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sphereDistance(vec3(p.x + MIN_DISTANCE, p.y, p.z)) - sphereDistance(vec3(p.x - MIN_DISTANCE, p.y, p.z)),
        sphereDistance(vec3(p.x, p.y + MIN_DISTANCE, p.z)) - sphereDistance(vec3(p.x, p.y - MIN_DISTANCE, p.z)),
        sphereDistance(vec3(p.x, p.y, p.z  + MIN_DISTANCE)) - sphereDistance(vec3(p.x, p.y, p.z - MIN_DISTANCE))
    ));
}

vec4 simpleLambert(vec3 ogPoint, vec3 normal,float specPower) {
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
    return simpleLambert(p,n,0.5);
}
vec4 raymarch(vec4 position, vec4 direction) {
    for (int i = 0; i < STEPS; i++) {
        vec3 tmp = position.xyz;
        float distance = sceneSDF(tmp);//sphereDistance(tmp);
        if (distance < MIN_DISTANCE){
            vec4 retColor = vec4(1,0,0,1);
            if(currentEnt > -1) retColor = ubo.renderList[currentEnt].color;
            currentEnt = -1;
            //retColor.w = 1;
            return retColor;//renderSurface(tmp);
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
    //vec3 col = vec3(0,10.0*uv.x,-10.0*uv.y);
    //outColor = vec4(col,1);
    //UBO is still being worked on;
    cameraVector = vec3(ubo.view[3][0],ubo.view[3][1],ubo.view[3][2]);
    vec4 modSpace = vec4(uv.x,uv.y,1,0);
    vec4 viewDirection = ubo.view*normalize(modSpace);//normalize(inPos-cameraVector);
    vDirection = viewDirection.xyz;
    outColor = raymarch(vec4(cameraVector.xyz,1),viewDirection);//raymarch(cameraVector,viewDirection);

}
