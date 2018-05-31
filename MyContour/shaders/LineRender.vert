uniform sampler2D terrainTexture;

uniform vec3 minCoord;
uniform vec3 maxCoord;
uniform float terrainScale;

varying mat4 mvp;

//in vec3 vertexArr;

void main()
{
	//vec2 texcoord = (vertexArr.xy - minCoord.xy) / (maxCoord.xy - minCoord.xy);
	vec2 texcoord = (gl_Vertex.xy - minCoord.xy) / (maxCoord.xy - minCoord.xy);

	//处理等值线在边界处下沉的问题
	if(texcoord.x <= 0.01)
		texcoord.x = texcoord.x + 0.01;
	if(texcoord.x >= 0.99)
      		texcoord.x = texcoord.x - 0.01; 
	if(texcoord.y <= 0.01)
		texcoord.y = texcoord.y + 0.01;
	if(texcoord.y >= 0.99)
      		texcoord.y = texcoord.y - 0.01; 
	
	vec4 position;
	position.x = gl_Vertex.x;
	position.y = gl_Vertex.y;
	position.z = texture2D(terrainTexture, texcoord).r/terrainScale + 30.0;
	position.w = 1.0;

	mvp = gl_ModelViewProjectionMatrix;
	gl_Position = gl_ModelViewProjectionMatrix * position;
	//gl_Position = mvp * position;
}