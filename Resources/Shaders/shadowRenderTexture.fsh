#ifdef GL_ES
precision lowp float;
#endif

#define PI 3.14
varying vec2 v_texCoord;
varying vec4 v_color;

uniform sampler2D u_texture2;
uniform vec2 resolution;                                             

uniform float softShadows;                                           

//sample from the distance map
float sample(vec2 coord, float r) {
    return step(r, texture2D(u_texture2, coord).r);
}                                                                    

void main(void) {
    //rectangular to polar
    vec2 norm = v_texCoord.st * 2.0 - 1.0;
    float theta = atan(norm.y, norm.x);

    /* example: directional light! */
    /*if (theta < -0.7 || theta > 0.3) {
        gl_FragColor = vec4(0,0,0,0);
        return;
    }
     */

    float r = length(norm);
    float coord = (theta + PI) / (2.0*PI);

    //the tex coord to sample our 1D lookup texture2D
    //always 0.0 on y axis
    vec2 tc = vec2(coord, 0.0);
	vec2 tc2 =  v_texCoord;
	vec4 n_color =  vec4(texture2D(CC_Texture0, tc2).rgb, 1.0);

    //the center tex coord, which gives us hard shadows
    float center = sample(tc, r);

    //we multiply the blur amount by our distance from center
    //this leads to more blurriness as the shadow fades away
    float blur = (1./resolution.x)  * smoothstep(0., 1., r);

    //now we use a simple gaussian blurriness
    float sum = 0.0;

    sum += sample(vec2(tc.x - 4.0*blur, tc.y), r) * 0.05;
    sum += sample(vec2(tc.x - 3.0*blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x - 2.0*blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x - 1.0*blur, tc.y), r) * 0.15;

    sum += center * 0.16;
	//sum += center*0.08;

    sum += sample(vec2(tc.x + 1.0*blur, tc.y), r) * 0.15;
    sum += sample(vec2(tc.x + 2.0*blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x + 3.0*blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x + 4.0*blur, tc.y), r) * 0.05;

    //sum of 1.0 -> in light, 0.0 -> in shadow
    float lit = mix(center, sum, softShadows);
    //multiply the summed amount by our distance, which
    //gives us a radial falloff
    //then multiply by vertex (light) color
    gl_FragColor = n_color * v_color * vec4(vec3(1.0),lit * smoothstep(1.0, 0.0, r));
	//gl_FragColor = n_color * v_color * vec4(lit * smoothstep(1.0, 0.0, r));
}                                                                   




