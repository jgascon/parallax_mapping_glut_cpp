
varying vec3 lightDir;

varying vec3 normal;
varying vec3 binormal;
varying vec3 tangent;

varying vec3 vpos;

varying vec4 material_diffuse;
varying vec4 material_ambient;

varying vec2 tex0, tex1;

uniform sampler2D NormalMap;
uniform sampler2D DiffuseMap;

uniform float depth_factor;
uniform int number_lod_iterations;

/*
    Este método recibe estos dos vectores y el mapa de normales.

    p = (u, v, 0.0, 1.0)

    v = (dot(V, tangent) * depth_factor / dot(normal, -V),
        dot(V, binormal) * depth_factor / dot(normal, -V),
        1.0 )

    V: el un vector unitario que va desde el centro de la cámara, situada en el
       origen (0,0,0) del espacio de cámara hasta el último vértice analizado,
       como es una variable varying, se va interpolando de un vértice al siguiente.

    Al final lo que nos devuelve son dos coordenadas de textura nuevas, que 
    son las que se usarán para coger el punto de color y el vector de normal.
*/
void ray_intersect(sampler2D reliefMap, inout vec4 p, inout vec3 v) {
    v /= float(number_lod_iterations);

    vec4 pp = p;
    for(int i = 0; i < number_lod_iterations - 1; ++i) {
        p.w = texture2D(reliefMap, p.xy).w;

        if(p.w > p.z) {
            pp = p;
            p.xyz += v;
        }
    }

    float f = (pp.w - pp.z) / (p.z - pp.z - p.w + pp.w);
    p = mix(pp, p, f);
}



void main() {
    vec3 V = normalize(vpos);
    vec3 v = vec3(dot(V, tangent), dot(V, binormal), dot(normal, -V));
    vec3 scale = vec3(1.0, 1.0, depth_factor);
    v *= scale.z / (scale * v.z);

    vec4 p = vec4(tex0, vec2(0.0, 1.0));
    ray_intersect(NormalMap, p, v);

    vec2 uv = p.xy;
    vec4 diffuse_tex = texture2D(DiffuseMap, uv);
    vec3 normal_tex = texture2D(NormalMap, uv).rgb * 2.0 - 1.0;
    normal_tex.z = sqrt(1.0 - dot(normal_tex.xy, normal_tex.xy));

    normal_tex = normalize(normal_tex.x * tangent +
                           normal_tex.y * binormal +
                           normal_tex.z * normal);

    float intensity = clamp(dot(normal_tex, lightDir), 0.0, 1.0);
    vec4 color = material_diffuse.rgba * diffuse_tex * intensity;
    color += material_ambient.rgba * diffuse_tex;

    gl_FragColor = color;
}







