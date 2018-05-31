//#version 120
varying vec2 TexCoord0;

uniform sampler2D GlyphTexture;
uniform vec4 color;

void main()
{
		gl_FragColor = color * texture2D(GlyphTexture, TexCoord0).aaaa;
}