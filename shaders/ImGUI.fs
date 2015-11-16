uniform sampler2D u_texture;

varying vec2 v_uv;
varying vec4 v_color;

void main()
{
	gl_FragColor = v_color * texture2D(u_texture, v_uv);
}
