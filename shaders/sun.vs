attribute vec3 a_pos;
uniform mat4 u_WVP;

void main() 
{
	vec4 pos = u_WVP * vec4(a_pos, 1.0);
	gl_Position = pos;
}