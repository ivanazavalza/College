/* 
jwf_Fragment_Shader_V1.glsl

Very simple fragment shader which takes in from jwf_Vertex_Shader_V1.glsl
and other set uniform structures.

This is my first attempt at making a Fragment Shader.

Jared Fowler
November 11, 2014
*/

# version 330 core

in vec4 fColor;
in vec3 fPosition;
in vec3 fNormal;

out vec4 fragment_color;

uniform vec4 PointLightLocation;
uniform vec3 KA;  //Minimum ambient light for the object
uniform vec3 KD;  //Minimum diffuse light for the object (unless not in sight of light)
uniform vec3 KS;  //Max specular light quality


//##############################################################################################

vec4 pointLight(){

    //if(PointLightLocation == NULL)
    //    return;

    vec3 n = normalize(fNormal);
    vec3 at = vec3(PointLightLocation) - fPosition;
    vec3 s = normalize(at);
    vec3 v = normalize(vec3(-fPosition));
    vec3 h = normalize(v + s);


    /*Point Light Idea:
        There will be a minimum generated ambient light, which will depend on the uniform KA value.
        (The idea is that every object will have its own KA value) If KA is NULL, we'll set the value
        to some minimum value.  Diffuse will be an equation of distance and angle, the same with specular.
        If no max specular is passed in, the value will be set to 0, the same with diffuse.
    */

    

    //Local variables
    vec3 ka = vec3(.2,.2,.2);
    vec3 kd = vec3(0,0,0);
    vec3 ks = vec3(0,0,0);
    float dist = pow( pow(at.x, 2) + pow(at.y, 2) + pow(at.z, 2) , .5);

    //Ambient lights
    ka = KA;

    //Diffuse Lights
    float temp = ((10000 - dist + 2000)/10000);

    kd.x = max(temp, KD.x);
    kd.y = max(temp, KD.y);
    kd.z = max(temp, KD.z);

    //Specular Lights

    ks.x = min(temp, KS.x);
    ks.y = min(temp, KS.y);
    ks.z = min(temp, KS.z);



    return fColor * 
           vec4 (ka + kd * max(dot(s,fNormal),0.0), 1.0) +
           vec4 (ks * pow(max(dot(h,n),0.0), 2) , 1.0);
}

//##############################################################################################
void main()
{
    fragment_color = pointLight();

    //fragment_color = fColor;

}