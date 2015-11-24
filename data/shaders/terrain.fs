precision highp float;
uniform vec3 u_sunDirection;
uniform vec3 u_sunLuminance;
uniform vec3 u_skyLuminance;

uniform sampler2D u_texture;
uniform sampler2D u_lightmap;

varying vec3 v_color;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec2 v_uv2;
varying float v_fogFactor;

void main() 
{
	vec3 diffuse = texture2D(u_texture, v_uv).rgb;
	vec3 light = texture2D(u_lightmap, v_uv2).rgb;
	diffuse*=diffuse;
	light*=light;
	//vec3 skyDirection = vec3(0.0, 0.0, 1.0);
	//vec3 normal = normalize(v_normal);
	//float diffuseSun = max(0.0, dot(normal, normalize(u_sunDirection)));
	//float diffuseSky = 1.0 + 0.5 * dot(normal, skyDirection);
	//vec3 color = diffuseSun * u_sunLuminance + diffuseSky * u_skyLuminance + vec3(0.01);
	//color *= diffuse;
	//color *= 0.5;
	vec3 color = light * diffuse * 1.3;
	color = mix(color, (u_skyLuminance + u_sunLuminance)*0.3, v_fogFactor);
	gl_FragColor = vec4(sqrt(color), 1.0);
}