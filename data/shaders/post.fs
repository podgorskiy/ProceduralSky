precision highp float;
varying vec2 	v_texcoord;

uniform sampler2D u_texture;

void main() 
{
	vec3 diffuse = texture2D(u_texture, v_texcoord).rgb;
	vec3 diffuse2 = texture2D(u_texture, v_texcoord + vec2(-0.5/1280.0, -0.3/756.0)).rgb;
	vec3 diffuse3 = texture2D(u_texture, v_texcoord + vec2(0.5/1280.0, -0.3/756.0)).rgb;
	vec3 diffuse4 = texture2D(u_texture, v_texcoord + vec2(0, 0.5/756.0)).rgb;
	gl_FragColor = vec4((diffuse * 3.0 + diffuse2 + diffuse3 + diffuse4)/6.0, 1.0);
}