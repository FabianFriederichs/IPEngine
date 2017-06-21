#version 330 core

//Richtung der direktionalen Lichtquelle
uniform vec3 lightDir;
//Lichtfarbe
uniform vec3 light_color;
//Textur
//uniform sampler2D tex;

//Eingabe vom VertexShader
in struct VertexData
{
	vec2 uv;
	vec3 normal;
} vertexdat;

//Output color
out vec4 color;

void main()
{
	//Textur samplen.
	vec4 texcol = vec4(1.0f, 1.0f, 0.0f, 1.0f);//texture(tex, vertexdat.uv);
	//Diffusen term berechnen
	float nsign = 1.0f - float(!gl_FrontFacing) * 2.0f;
	float diffterm = max(dot(-lightDir, vertexdat.normal * nsign),0.0f);

	//Outpur Farbe berechnen.
	color = vec4((diffterm + 0.4) * vec4(light_color, 1.0f)) * texcol;
}