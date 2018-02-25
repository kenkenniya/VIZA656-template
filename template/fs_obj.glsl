#version 430

#define PI 3.1415926

in VS_OUT{
	 vec3 FragPos;
	 vec3 Normal;
	 vec4 ShadowCoor;
} fs_in;

in vec2 TexCoord;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform mat4 ModelMatrix;

uniform sampler2D ShadowMap;  //shadow map texture

uniform samplerCube skybox;

 /*light in eye space*/
vec3 light_po_eye = vec3(ModelMatrix * vec4(light_position,1));

/*function to calculate shadow*/
float CalculationShadow(vec4 ShadowCoord)
{
	//transform to [0,1] range
	vec3 ProCoords = ShadowCoord.xyz/ShadowCoord.w;
	ProCoords = ProCoords*0.5 + 0.5;

	/*calculate bias based depth map resolution and slope*/
   vec3 light_direc = normalize(light_po_eye - fs_in.FragPos);
   float bias = max(0.05 * (1.0 - dot(fs_in.Normal, light_direc)), 0.005);

	/* compare whether in shadow or not */
	float visibility = 1.0;
	vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
    for(int y = -1; y <= 1; ++y)
        {
        float pcfDepth = texture(ShadowMap, ProCoords.xy + vec2(x, y) * texelSize).r; 
        visibility += ProCoords.z - bias > pcfDepth ? 0.0 : 0.5;        
        }    
     }

	visibility /= 9.0;

	if (ProCoords.z > 1.0f)
	visibility = 0.0; 

	return visibility;
}


void main()
{
   vec4 amb_color = vec4(0.1,0.2,0.2,1);   //ambient color
   vec4 diff_color = vec4(0.9,0.5,0.5,1);  //diffuse color
   vec4 spec_color =  vec4(1,1,1,1);  //specular color

   float diff_coeff = 0.9;	 //diffuse coefficient
   float spec_coeff = 0.9;	 //specular coefficient

   /*diffuse reflection*/		 
   vec3 light_direc = normalize(light_po_eye - fs_in.FragPos);
   
   /*specular reflection*/
   vec3 view_vec = normalize( fs_in.FragPos - camera_position);
   vec3 reflec_vec = reflect(-light_direc,fs_in.Normal);

  /*compute lumpsum spec*/
  float specmax=0.90;
  float specmin=0.30;
  float spec = smoothstep(specmin,specmax,dot(reflec_vec,-view_vec) );
   
   /*compute lumpsum diffuse*/
   float diff = 0.5*dot(light_direc,fs_in.Normal) + 0.5;	
   diff = mix(CalculationShadow(fs_in.ShadowCoor),diff,0.5);

   /*combine ambient, specular and diffuse color*/
   vec4 lighting = mix(amb_color, diff_color, diff_coeff*diff );
   lighting = mix(lighting, spec_color,spec_coeff*spec );

   /*output the final color*/
   gl_FragColor = lighting;


}