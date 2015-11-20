attribute vec3 a_pos;
attribute vec3 a_color;
attribute vec3 a_normal;
uniform mat4 u_WVP;
uniform mat4 u_W;
varying vec3 v_color;
varying vec3 v_normal;
varying vec3 v_rnormal;

void main() 
{
	vec4 pos = u_WVP * vec4(a_pos, 1.0);
	gl_Position = pos;
	v_color = a_color;
	v_rnormal = a_normal;
	v_normal = (u_W * vec4(a_normal, 0.0)).xyz;
}