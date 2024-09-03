attribute vec3 external_tangent;
varying vec3 lightDir;

varying vec3 normal;
varying vec3 binormal;
varying vec3 tangent;

varying vec3 vpos;
varying vec4 material_diffuse;
varying vec4 material_ambient;

varying vec2 tex0, tex1;

void main() {
    tex0 = vec2(gl_MultiTexCoord1);
    tex1 = vec2(gl_MultiTexCoord2);

    normal   = normalize(gl_NormalMatrix * gl_Normal);
    tangent  = normalize(gl_NormalMatrix * external_tangent);
    binormal = normalize(cross(tangent, normal));

    vpos = (gl_ModelViewMatrix * gl_Vertex).xyz;
    lightDir = normalize(vec3(gl_LightSource[0].position));

    material_diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    material_ambient  = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    material_ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    gl_Position = ftransform();
}