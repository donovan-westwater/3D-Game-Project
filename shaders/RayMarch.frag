#version 450
#extension GL_ARB_separate_shader_objects : enable

#define STEPS 64
#define STEP_SIZE 0.01
#define MIN_DISTANCE 0.01
//Was 0.1
//This entire scene exists in "model space" without any transformations
struct EntityRender_S { //Stores the infomation needed by the shader
    vec4 position;
    vec4 rotation;
    vec4 scale;
    vec4 color;
    int id;
    int type;
}EntityRender;

layout(binding = 0) uniform UniformBufferObject {
    
    EntityRender_S renderList[50];
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
vec3 lightVector = vec3(0,5,0); //vec3(0,0,1);
vec3 cameraVector = vec3(0,0,1);
vec3 vDirection = vec3(0,0,0);
int currentEnt = -1;
//***Transformations***//
//Copied from marble marcher
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


//***SDFs for various primatives**//
float sphereSDF(vec3 p) {
    return length(p) - radius;
}
float boxSDF(vec3 p,vec3 b){
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float planeSDF( vec3 p, vec3 n, float h )
{
  // n must be normalized
  return dot(p,n) + h;
}
float sphereDistance(vec3 p) {
    return distance(p, centre) - radius;
}
//***Scene SDF***//
float sceneSDF(vec3 p){
    vec4 pD = vec4(p,1);
    vec3 scale;
    float d = 99999.;
    float prevD = d;
    for(int i = 0;i<50;i++){
        if(ubo.renderList[i].id < 0) continue;
        pD = vec4(p,1);
        //Transforms
        pD.xyz -= ubo.renderList[i].position.xyz;
        rotZ(pD,radians(ubo.renderList[i].rotation.z));
        rotY(pD,radians(ubo.renderList[i].rotation.y));
        rotX(pD,radians(ubo.renderList[i].rotation.x));
        scale = ubo.renderList[i].scale.xyz;
        if(ubo.renderList[i].type == 1) d = min(boxSDF(pD.xyz/scale,vec3(0.25,0.25,0.25))*min(scale.x,min(scale.y,scale.z)),d);
        if(ubo.renderList[i].type == 0) d = min(sphereSDF(pD.xyz/scale)*min(scale.x,min(scale.y,scale.z)),d);
       
        if(abs(d - prevD) > 0.0001) currentEnt = i;
        prevD = d;
    }
    d = min(d,planeSDF(p,vec3(0,1,0),0));
    //Substractions
    for(int i = 0;i<50;i++){
        if(ubo.renderList[i].id < 0) continue;
        pD = vec4(p,1);
        if(ubo.renderList[i].type == 4){
            vec3 c = vec3(5);
            vec3 q = mod(pD.xyz+0.5*c,c)-0.5*c;
            pD.xyz = q;
        }
        //Transforms
        pD.xyz -= ubo.renderList[i].position.xyz;
        rotZ(pD,radians(ubo.renderList[i].rotation.z));
        rotY(pD,radians(ubo.renderList[i].rotation.y));
        rotX(pD,radians(ubo.renderList[i].rotation.x));
        scale = ubo.renderList[i].scale.xyz;
        if(ubo.renderList[i].type == 2) d = max(-boxSDF(pD.xyz/scale,vec3(0.25,0.25,0.25))*min(scale.x,min(scale.y,scale.z)),d);
        if(ubo.renderList[i].type == 3) d = max(-sphereSDF(pD.xyz/scale)*min(scale.x,min(scale.y,scale.z)),d);
        if(ubo.renderList[i].type == 4) d = max(-sphereSDF(pD.xyz/scale)*min(scale.x,min(scale.y,scale.z)),d);
        if(abs(d - prevD) > 0.0001) currentEnt = i;
        prevD = d;
    }

    
    if(abs(d - prevD) > 0.0001) currentEnt = -1;
  
    return d;
}
//***Lighting***//
 vec3 estimateNormal(vec3 p) {
    if(p.y <= -1.0) p.y = 0.;
    return normalize(vec3(
        sceneSDF(vec3(p.x + MIN_DISTANCE, p.y, p.z)) - sceneSDF(vec3(p.x - MIN_DISTANCE, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + MIN_DISTANCE, p.z)) - sceneSDF(vec3(p.x, p.y - MIN_DISTANCE, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + MIN_DISTANCE)) - sceneSDF(vec3(p.x, p.y, p.z - MIN_DISTANCE))
    ));
}

vec4 simpleLambert(vec3 ogPoint, vec3 normal,float specPower) {
    //vec3 viewDirection = inPos-cameraVector;
    vec3 viewDirection = vDirection;
    vec3 lightDir = lightVector;

    float NdotL = max(dot(normal, lightDir), 0);
    vec4 c = vec4(1,1,1,1);
    // Specular
    vec3 h = (lightDir - viewDirection) / 2.;
    float s = pow(dot(normal, h), specPower);
    c = (NdotL+s)*vec4(1,1,1,1);
    c.w = 1;
    return c;
}
vec4 renderSurface(vec3 p){
    vec3 n = estimateNormal(p);
    return simpleLambert(p,n,1);
}
vec4 raymarch(vec4 position, vec4 direction) {
    float total = 0;
    for (int i = 0; i < STEPS; i++) {
        vec3 tmp = position.xyz;
        float distance = sceneSDF(tmp);//sphereDistance(tmp);
        total += distance;
        if (distance < MIN_DISTANCE){
            vec4 retColor = vec4(0.15,0.15,0.15,1);
            if(currentEnt > -1) retColor = ubo.renderList[currentEnt].color;
            currentEnt = -1;
            retColor.w = 1;
            return retColor*renderSurface(tmp);//renderSurface(tmp);
        }
        position += direction * distance;//STEP_SIZE;
        if(total > 50.) break;
    }
    /*
    if(planeSDF(position.xyz,vec3(0,1,0),-1.) <= -1.0){//position.y < -1.){
     	position.y = -1.;   
        return vec4(1,1,1,1)*renderSurface(position.xyz); 
    }
    */
    return vec4(0,0,0,1); //was originally 1
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
    //outColor = vec4(col,1);
    /*
    outColor = vec4(1);
    outColor.w = 1;
    if(ubo.resolution.y == 0) outColor = vec4(0);
    if(ubo.view[3][2] == 0) outColor.x = 0.5;
    if(ubo.renderList[0].position.z == 0) outColor.y = 0.5;
    */
    cameraVector = vec3(ubo.view[3][0],ubo.view[3][1],ubo.view[3][2]);
    vec4 modSpace = vec4(uv.x,-uv.y,1,0);
    vec4 viewDirection =  ubo.view*normalize(modSpace);//normalize(inPos-cameraVector);
    vDirection = viewDirection.xyz;
    outColor = raymarch(vec4(cameraVector.xyz,1),viewDirection);//raymarch(cameraVector,viewDirection);

}
