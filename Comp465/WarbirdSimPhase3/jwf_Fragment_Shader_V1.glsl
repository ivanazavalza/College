/* 
jwf_Fragment_Shader_V1.glsl

Very simple fragment shader which takes in from jwf_Vertex_Shader_V1.glsl
and other set uniform structures.

This is my first attempt at making a Fragment Shader.

Jared Fowler
November 11, 2014
*/

# version 330 core

/*Change these values for different effects*/
#define RUBER_RADIUS            2000
#define POINT_LIGHT_MAX_DIST    10000
#define SHININESS_FACTOR        10
#define SPOT_LIGHT_MAX_DIST     4000


/*Input from Vertex Shader*/
in vec4 fColor;
in vec3 fPosition;
in vec3 fNormal;

/*Output to Final Pixel Stages*/
out vec4 fragment_color;

/*Uniform Constants*/

uniform vec4 ToggleLights;

uniform vec3 PointLightLocation;

uniform vec3  SpotLightLocation;
uniform vec3  SpotLightDirection;
uniform float SpotLightAngle;

//The following are object dependent:
uniform vec3 KA;  //Minimum ambient light
uniform vec3 KD;  //Minimum diffuse light (IFF in line of light's site)
uniform vec3 KS;  //Max specular light    (IFF in line of camera's site)



//##############################################################################################
float getDistance(vec3 a, vec3 b){

    return pow( pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2) , .5);
}

//##############################################################################################
vec4 pointLight(vec3 frag_To_PL, vec3 frag_To_CAM)
{
    //Local Variables
    vec3 Norm_frag_To_PL  = normalize(frag_To_PL);
    vec3 Norm_frag_To_CAM = normalize(frag_To_CAM);
    vec3 Norm_SL_Vec      = normalize(Norm_frag_To_PL + Norm_frag_To_CAM);
    float distance        = 0;
    float intensity       = 0;
    vec3 kd               = vec3(0,0,0);
    vec3 ks               = vec3(0,0,0);

    /*This point light will be in charge of diffuse and specular light effects. These light
    properties will take into account attenuation, that is, the light intensity will decrease
    over distance. (We assume there to be particals floating in space which get in the way!)*/

    //Get the distance between the point light and the fragment
    distance = getDistance(fPosition, PointLightLocation);

    //Based upon distance, get the intensity of the light
    intensity = ((POINT_LIGHT_MAX_DIST - distance + RUBER_RADIUS)/POINT_LIGHT_MAX_DIST);  //My awesome equation for intensity.. :p

    //Based upon intensity, modify the passed in KD and KS values

    kd.x = max(max(intensity, 0), KD.x);
    kd.y = max(max(intensity, 0), KD.y);
    kd.z = max(max(intensity, 0), KD.z);

    ks.x = min(max(intensity, 0), KS.x);
    ks.y = min(max(intensity, 0), KS.y);
    ks.z = min(max(intensity, 0), KS.z);

    //Finally, calculate the light effects

    //Crazy Settings?
    if(ToggleLights.w != 0)
        return vec4(1,0,0,1);

    return ( fColor * vec4 (kd * max( dot(Norm_frag_To_PL, fNormal), 0.0), 1.0) ) + vec4(ks * pow( max( dot(Norm_SL_Vec, fNormal), 0.0), SHININESS_FACTOR) , 0);

}

//##############################################################################################
vec4 spotLight(vec3 frag_To_SL, vec3 frag_To_CAM)
{

    //Local Variables
    vec3 Norm_frag_To_SL  = normalize(frag_To_SL);
    vec3 Norm_frag_To_CAM = normalize(frag_To_CAM);
    vec3 Norm_SL_Vec      = normalize(Norm_frag_To_SL + Norm_frag_To_CAM);
    float distance        = 0;
    float intensity       = 0;
    float angle           = 0;
    vec3 kd               = vec3(0,0,0);
    vec3 ks               = vec3(0,0,0);
    
    /*The spot light will add to diffuse and specular light attributes. There will be a certain
    angle, a uniform value, which will represent the outer cone's direction in respect to
    the uniform SpotLigthDirection. The inner cone will be represented by half the outer cone angle.
    Attenuation will exist for both the inner and outer cones, however, the outer cone intensity
    decreases at a higher rate than the inner.*/

    //Get the distance between the spot light and the fragment
    distance = getDistance(fPosition, SpotLightLocation);

    //Based upon distance, get the intensity of the light
    intensity = ((SPOT_LIGHT_MAX_DIST - distance)/SPOT_LIGHT_MAX_DIST);  //My awesome equation for intensity.. :p

    //Based upon intensity, modify the passed in KD and KS values

    kd.x = max(intensity, 0);
    kd.y = max(intensity, 0);
    kd.z = max(intensity, 0);

    ks.x = min(max(intensity, 0), KS.x);
    ks.y = min(max(intensity, 0), KS.y);
    ks.z = min(max(intensity, 0), KS.z);

    //Get the angle between the spot lights forward direction and spot light to fragment
    angle = acos( dot(normalize(SpotLightDirection), -(Norm_frag_To_SL)) );

    //We'll want to take into account negative and positive angles, so just make it positive
    angle = max(angle, -angle);

    //Finally, based upon the angle add light effects:

    if(angle <= (SpotLightAngle / 2)){

        //Crazy Settings?
        if(ToggleLights.w != 0)
            return vec4(0,1,0,1);

        //We are within the inner cone. Minor attenuation.
        return ( fColor * vec4 (kd * max( dot(Norm_frag_To_SL, fNormal), 0.0), 1.0) ) + vec4(ks * pow( max( dot(Norm_SL_Vec, fNormal), 0.0), SHININESS_FACTOR) , 0);
    }

    if(angle <= SpotLightAngle){

        //Crazy Settings?
        if(ToggleLights.w != 0)
            return vec4(0,0,1,1);
    
        //We are within the inner cone. Minor attenuation.
        return ( fColor * vec4 ((kd/4) * max( dot(Norm_frag_To_SL, fNormal), 0.0), 1.0) ) + vec4((ks/4) * pow( max( dot(Norm_SL_Vec, fNormal), 0.0), SHININESS_FACTOR) , 0);
    }

    //Else, the fragment is not effected!
    return vec4(0,0,0,0);

}



//##############################################################################################
void main()
{
    //Local Variables:
    vec3 frag_To_PointLight_Vec;
    vec3 frag_To_SpotLight_Vec;
    vec3 frag_To_Camera_Vec;

    //Assign local varaibles:
    frag_To_PointLight_Vec = (PointLightLocation - fPosition);
    frag_To_SpotLight_Vec  = (SpotLightLocation - fPosition);
    frag_To_Camera_Vec     = (-fPosition);

    //Initialize the frags color with the minimum ambient light
    if(ToggleLights.x != 0)
        fragment_color = (vec4(KA, 1) * fColor);

    //Add point light effects to frag
    if(ToggleLights.y != 0)
        fragment_color += pointLight(frag_To_PointLight_Vec, frag_To_Camera_Vec);

    //Add spot light effects to frag
    if(ToggleLights.z != 0)
        fragment_color += spotLight(frag_To_SpotLight_Vec, frag_To_Camera_Vec);
}