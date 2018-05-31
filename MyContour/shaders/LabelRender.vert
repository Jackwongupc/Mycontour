uniform sampler2D terrainTexture;

uniform vec3 minCoord;
uniform vec3 maxCoord;

varying vec2 TexCoord0;

void main()
{
		vec2 texcoord = (gl_Vertex.xy - minCoord.xy)/(maxCoord.xy - minCoord.xy);
		
		//�����ֵ�߱�ע�ڱ߽紦�³�������
		if(texcoord.x <= 0.02)
			texcoord.x = texcoord.x + 0.02;
		if(texcoord.x >= 0.98)
      			texcoord.x = texcoord.x - 0.02; 
		if(texcoord.y <= 0.02)
			texcoord.y = texcoord.y + 0.02;
		if(texcoord.y >= 0.98)
      			texcoord.y = texcoord.y - 0.02; 

		vec4 position;
		position.x = gl_Vertex.x;
		position.y = gl_Vertex.y;
		position.z = texture2D(terrainTexture, texcoord).r + 200.0f;
		position.w = 1.0;
		
		TexCoord0 = gl_MultiTexCoord0.xy;
		gl_Position = gl_ModelViewProjectionMatrix * position;

}