#ifdef GL_ES
precision lowp float;
#endif

#define PI 3.14

varying vec2 v_texCoord;
varying vec4 v_color;

uniform vec2 resolution;

//for debugging; use a constant value in final release
uniform float upScale;

uniform float accuracy;

//alpha threshold for our occlusion map
const float THRESHOLD = 0.75;


void main(void) {

    float distance = 1.0;

    for (float y=0.0; y<resolution.y; y+=accuracy) {
        //rectangular to polar filter
        vec2 norm = vec2(v_texCoord.s, y/resolution.y) * 2.0 - 1.0;
        float theta = PI*1.5 + norm.x * PI;
        float r = (1.0 + norm.y) * 0.5;

        //coord which we will sample from occlude map
        vec2 coord = vec2(-r * sin(theta), -r * cos(theta))/2.0 + 0.5;

        //sample the occlusion map
        vec4 data = texture2D(CC_Texture0, coord);

        //the current distance is how far from the top we've come
        float dst = y / resolution.y / upScale;

        //if we've hit an opaque fragment (occluder), then get new distance
        //if the new distance is below the current, then we'll use that for our ray
        float caster = data.a;
        if (caster > THRESHOLD) {
            distance = dst;
			break;
        }
    }
    gl_FragColor = vec4(distance,0.0, 0.0, 1.0);
}