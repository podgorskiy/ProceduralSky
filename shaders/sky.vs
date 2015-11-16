attribute vec2 a_pos;

uniform vec3 	u_celestialBodyDirection;
uniform mat4 	u_cameraTransform;
uniform vec2	u_projectionParameters;
uniform vec3 	u_skyDirection;
uniform vec3 	u_skyLuminanceXYZ;
uniform float 	u_Exposition;

varying vec3 	v_skyColor;
varying vec4 	v_screenPos;
varying	vec3 	v_viewDir;
varying vec2 	v_texcoord;

vec3 Perez(vec3 A,vec3 B,vec3 C,vec3 D, vec3 E,float costeta, float cosgamma)
{
	float _1_costeta = 1.0 / costeta;
	float cos2gamma = cosgamma * cosgamma;
	float gamma = acos(cosgamma);
	vec3 f = (vec3(1.0) + A * exp(B * _1_costeta)) * (vec3(1.0) + C *exp(D * gamma) + E * cos2gamma);
	return f;
}

void main()
{
	v_screenPos = vec4(vec3(a_pos, 0.0), 1.0);
	gl_Position = v_screenPos;
	
	vec3 viewDir = vec3(v_screenPos.x / u_projectionParameters.x, v_screenPos.y / u_projectionParameters.y, -1.0);
	
	vec3 lightDir = normalize(u_celestialBodyDirection);
	vec3 skyDir = normalize(u_skyDirection);

	v_viewDir =  (u_cameraTransform * vec4(viewDir,0.0)).xyz;
	v_viewDir.z = abs(v_viewDir.z);
	v_viewDir = normalize(v_viewDir);

	vec3 A = vec3(-0.297800, -0.294200, -1.105600);
	vec3 B = vec3(-0.132200, -0.180800, -0.283300);
	vec3 C = vec3(0.211700, 0.194400, 5.279700);
	vec3 D = vec3(-1.027100, -1.741900, -2.335900);
	vec3 E = vec3(0.038600, 0.031100, 0.236300);
					
	float costeta = dot(v_viewDir, skyDir);
	float cosgamma = clamp(dot(v_viewDir, lightDir), -0.9999, 0.9999);
	float cosgammas = dot(skyDir, lightDir);
	
	vec3 P = Perez(A,B,C,D,E, costeta, cosgamma);			
	vec3 P0 = Perez(A,B,C,D,E, 1.0, cosgammas);			
        
	vec3 skyColorxyY = vec3(u_skyLuminanceXYZ.x / (u_skyLuminanceXYZ.x+u_skyLuminanceXYZ.y + u_skyLuminanceXYZ.z),			
							u_skyLuminanceXYZ.y / (u_skyLuminanceXYZ.x+u_skyLuminanceXYZ.y + u_skyLuminanceXYZ.z),			
							u_skyLuminanceXYZ.y);			
    
	vec3 Yp = skyColorxyY * P / P0;			
				
	vec3 skyColorXYZ = vec3(Yp.x * Yp.z / Yp.y,Yp.z, (1.0 - Yp.x- Yp.y)*Yp.z/Yp.y);			
        
	mat3 m = mat3(			
  		3.240479, -0.969256, 0.055648, 			
   		-1.53715, 1.875991, -0.204043, 			
   		-0.49853, 0.041556, 1.057311  			
	);			
    
	v_skyColor = pow(m * (skyColorXYZ / u_Exposition) , vec3(1.0/2.2));
}
