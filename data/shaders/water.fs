precision highp float;
uniform sampler2D WaterNormal1;
uniform sampler2D WaterNormal2;
//uniform sampler2D WaterBuffer;

uniform vec3 u_sunDirection;
uniform vec3 u_sunLuminance;
uniform vec3 u_skyLuminance;

uniform vec4 Light0Ambientcolor;

varying vec2 vCoord0;
varying vec2 vCoord1;

varying vec3 v_worldPos;
varying vec4 v_screenPosition;

varying float v_fogFactor;

uniform vec3 u_waterColor;
uniform float u_waterSpecular;

uniform vec3 u_eyePosition;

void main()
{
	vec3 skyDirection = vec3(0.0, 0.0, 1.0);
	
	float Glossiness = 100.0;

	vec2 baseUV = v_screenPosition.xy / v_screenPosition.w * vec2(0.5, -0.5) + vec2(0.5);
	
	vec3 NormalMap = texture2D(WaterNormal1, vCoord0).xyz + texture2D(WaterNormal2, vCoord1).xyz - 1.0;
		
	vec3 ViewDirection = normalize(u_eyePosition - v_worldPos);
	vec3 normal = normalize(NormalMap);
	float NDotL = dot(normal, u_sunDirection);
	float specPower = pow(max(0.0, dot(((2.0 * NDotL) * normal) - u_sunDirection, ViewDirection)), Glossiness);

	
	vec2 distortedUV = baseUV + NormalMap.xy * 0.05;
	
	//vec4 undistorted = texture2D(WaterBuffer, baseUV);
	//vec4 distorted = texture2D(WaterBuffer, distortedUV);
	
	vec3 refractionColor = u_skyLuminance;//mix(distorted.xyz, undistorted.xyz, distorted.a);
		
	vec3 color = refractionColor * u_waterColor;
	color *= color;
	
	float NDotL2 = 0.5 + 0.5 * dot(normal, skyDirection);
	
	//color.rgb *= Light0Ambientcolor.rgb + u_sunLuminance.rgb * abs(NDotL) + u_skyLuminance.rgb * NDotL2;
	color.rgb *= u_sunLuminance.rgb * abs(NDotL) + u_skyLuminance.rgb * NDotL2;
	color.rgb += u_sunLuminance.rgb * specPower * u_waterSpecular;
		
	color = mix(color, (u_skyLuminance + u_sunLuminance)*0.3, v_fogFactor);
	color.rgb = sqrt(color.rgb);
		
	//gl_FragColor = vec4(vec3(specPower) + color * 0.0000000001,1.0);
	gl_FragColor = vec4(color,1.0);
}
