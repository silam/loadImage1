#version 150

uniform sampler2D texture;
uniform sampler2D nighttexture;
uniform sampler2D spectexture;
uniform sampler2D cloudtexture;

in vec3 position;
in vec3 vN;

in vec4 AmbientDiffuseColor;
in vec4 SpecularColor;
in float SpecularExponent;

uniform vec4 light_position;
uniform vec4 light_color;
uniform vec4 ambient_light;

out vec4  fColor;
in vec2 texCoord;

void main()
{
/*
	float mixfactor;
	vec3 L = normalize( light_position.xyz - position.xyz);
	vec3 E = normalize(-position.xyz);
	vec3 N = normalize(vN);

	vec3 H = normalize(L+E);

	vec4 ctexel = texture2D(texture, texCoord);
	vec4 ntexel = texture2D(nighttexture, texCoord);

	mixfactor = 0;

	if ( dot(L,N) >= 0)
		mixfactor = (1 - abs(dot(L,N)))*0.5;	
	else 
		mixfactor = 1 - (1 - abs(dot(L,N)))*0.5;
	
	vec4 stexel = texture2D(spectexture, texCoord);

	vec4 spec = pow( max (dot(N,H), 0.0), stexel.a*100) *  stexel * light_color  ;
	if(dot(L,N) < 0.0){
		spec = vec4(0,0,0,1);
	
	}



	vec4 texel  = mix(ctexel, ntexel, mixfactor); //texture2D(nighttexture, texCoord);

	vec4 amb = texel * ambient_light;
	vec4 diff = max(dot(L,N), 0.0) *texel * light_color;


	//fColor = amb + diff + spec;
	//fColor = vec4( vN, 1);
	*/

	fColor = texture2D(cloudtexture, texCoord);
}