precision highp float;
varying vec3 v_skyColor;
void main() 
{
	gl_FragColor = vec4(v_skyColor, 1.0);
}