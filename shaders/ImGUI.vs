uniform mat4 u_projMtx;

attribute vec2 a_pos;
attribute vec2 a_uv;
attribute vec4 a_color;

varying vec2 v_uv;
varying vec4 v_color;

void main()
{
	v_uv = a_uv;
	v_color = a_color;
	gl_Position = u_projMtx * vec4(vec3(a_pos, 0.0), 1.0);
}
