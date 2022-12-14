uniform float alpha_value;
uniform float time;

void main(void)
{
	vec4 color;
	color.x = 1.0;
	color.y = 1.0;
	color.z = 0.0;
	color.w =  alpha_value * ((sin(time * 5.0) + 1.0) / 2.0 );
	gl_FragColor = color;
}