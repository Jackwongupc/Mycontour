varying vec4 worldPos;
void main()
{
	gl_FragColor = vec4(worldPos.z, 0.0f, 0.0f, 1.0f);

}