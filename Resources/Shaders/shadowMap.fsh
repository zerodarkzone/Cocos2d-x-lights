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
	float theta = PI*1.5 + (v_texCoord.s * 2.0 - 1.0) * PI;
	float add = accuracy / resolution.y;
	vec2 pre_coord = vec2(sin(theta), cos(theta)) * 0.5;
	
    for (float r = 0.0; r < 1.0; r+=add) {
               
        //coord which we will sample from occlude map
        vec2 coord = -r * pre_coord + 0.5;

        //sample the occlusion map
        vec4 data = texture2D(CC_Texture0, coord);
        
        //if we've hit an opaque fragment (occluder), then get new distance
        //if the new distance is below the current, then we'll use that for our ray
        if (data.a > THRESHOLD) {
            distance = r;
			break;
        }
    }
    gl_FragColor = vec4(distance / upScale, 0.0, 0.0, 1.0);
}