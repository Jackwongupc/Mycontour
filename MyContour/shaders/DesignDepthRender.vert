varying vec4 worldPos;
void main()
{
	worldPos = vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z, 1.0);
	gl_Position = ftransform();
}