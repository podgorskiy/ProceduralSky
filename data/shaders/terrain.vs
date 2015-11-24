attribute vec3 a_pos;
attribute vec3 a_color;
attribute vec3 a_normal;
attribute vec2 a_uv;
attribute vec2 a_uv2;
uniform mat4 u_WVP;
uniform mat4 u_W;
varying vec3 v_color;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec2 v_uv2;
varying float v_fogFactor;
uniform vec3 u_eyePosition;

void main() 
{
	vec4 pos = u_WVP * vec4(a_pos, 1.0);
	gl_Position = pos;
	v_color = a_color;
	v_normal = (u_W * vec4(a_normal, 0.0)).xyz;
	v_uv = a_uv;
	v_uv2 = a_uv2;
	float fogDistance = length(u_eyePosition - (u_W * vec4(a_pos, 1.0)).xyz);
	v_fogFactor = clamp((fogDistance-10000.0) / 500000.0, 0.0, 1.0);
}