attribute vec3 a_pos;

uniform float u_time;

uniform mat4 u_WVP;
uniform mat4 u_W;

varying vec2 vCoord0;
varying vec2 vCoord1;

varying	vec3 v_worldPos;
varying vec4 v_screenPosition;

uniform vec2 WaveSpeed1;
uniform float WaterNormalTiling1;

uniform vec2 WaveSpeed2;
uniform float WaterNormalTiling2;
varying float v_fogFactor;
uniform vec3 u_eyePosition;

void main(void)
{	
	v_worldPos = (u_W * vec4(a_pos.xyz, 1.0)).xyz;
	v_screenPosition = u_WVP * vec4(a_pos.xyz, 1.0);

	//vCoord0 = v_worldPos.xy * WaterNormalTiling1 + u_time * WaveSpeed1;
	//vCoord1 = v_worldPos.xy * WaterNormalTiling2 + u_time * WaveSpeed2;
	vCoord0 = v_worldPos.xy * 0.001 + u_time * vec2(0.005, -0.01);
	vCoord1 = v_worldPos.xy * 0.002 + u_time * vec2(0.04, -0.008);
	
	float fogDistance = length(u_eyePosition - (u_W * vec4(a_pos, 1.0)).xyz);
	v_fogFactor = clamp((fogDistance-10000.0) / 500000.0, 0.0, 1.0);
	
	gl_Position = v_screenPosition;
}
