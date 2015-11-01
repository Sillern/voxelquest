#version 120
#extension GL_EXT_frag_depth : enable

uniform mat4 modelview;
uniform mat4 proj;

uniform vec3 lightVec;

uniform float objectId;
uniform float curTime;
uniform vec3 cameraPos;
uniform float isWire;
uniform vec3 matVal;
uniform vec4 rotationZ;

varying vec3 TexCoord0;

//varying vec3 curColor;
varying vec3 curNormal;
varying vec3 finalVec;
varying vec4 worldPos;

varying vec4 screenPos;
varying float camDis;
uniform float clipDist;

$

const float M_PI = 3.14159265359;

void main() {

    TexCoord0 = gl_MultiTexCoord0.xyz;
    
    vec4 newPos = gl_Vertex;
    
    //newPos.xyz += cameraPos - vec3(5.0,5.0,5.0);
    
    //curColor = gl_Color.rgb;
    curNormal = gl_Normal.xyz;
    
    //newPos.xyz = mix(newPos.xyz,cameraPos,(sin(curTime)+1.0)*0.5);
    
    //newPos.xyz += cameraPos;
    
    
    worldPos = newPos;
    
    mat4 myMat = proj*modelview;// gl_ModelViewProjectionMatrix;
    
    screenPos = myMat * newPos; //
    camDis = distance(cameraPos.xyz,newPos.xyz);
    
    gl_Position = screenPos;
    
}

$

void main() {

    
    
    float rad = 0.98;

    if (isWire == 0.0) {

    }
    else {

        if (
            ( (abs(TexCoord0.x) < rad) && (abs(TexCoord0.y) < rad) ) ||
            ( (abs(TexCoord0.y) < rad) && (abs(TexCoord0.z) < rad) ) ||
            ( (abs(TexCoord0.x) < rad) && (abs(TexCoord0.z) < rad) )
        ) {
            discard;
        }
    }
    
    gl_FragDepthEXT = camDis/clipDist;
    //vec3 newMat = matVal.rgb/255.0;
    
    
    
    // vec2 curTC = vec2(0.0);
    // vec3 newTC = (TexCoord0.xyz+1.0)*0.5;
    // if (abs(newTC.z-0.5) == 0.5) {
    //     curTC = newTC.xy;
    // }
    // else {
    //     if (abs(newTC.y-0.5) == 0.5) {
    //         curTC = newTC.xz;
    //     }
    //     else {
    //         curTC = newTC.yz;
    //     }
    // }
    // vec4 newTex = texture2D(Texture0,curTC.xy);
    // newMat.rgb = newMat.rgb*0.5 + newTex.rgb;
    
    
    
    //camDis = distance(cameraPos.xyz,newVert.xyz);
    float zbVal = 1.0-camDis/clipDist;
    //float zbVal = 1.0-screenPos.z/clipDist;

    //

    gl_FragData[0] = vec4((worldPos.xyz),zbVal);//vec4(heightPacked.rg,matPacked.rg);//vec4(bhr,bhg,3.0/255.0,tex0.a);
    gl_FragData[1] = vec4(
        (curNormal+1.0)*0.5
        //curColor.rgb*clamp(dot(curNormal,-lightVec.xyz),0.0,1.0)
        ,objectId
    );//vec4(resNorm.rgb, (TexCoord0.z+tex1.a)/2.0 ); //(TexCoord0.xyz+1.0)/2.0

}