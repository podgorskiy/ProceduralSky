attribute vec2 a_pos;

varying vec2 	v_texcoord;


void main()
{
	vec4 screenPos = vec4(vec3(a_pos, 0.0), 1.0);
	v_texcoord = screenPos.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
	gl_Position = screenPos;
}
