#version 430

in VS_OUT{
	 vec3 FragPos;
	 vec3 Normal;
	 vec4 ShadowCoor;
	 vec2 TexCoord;
} fs_in;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform mat4 ModelMatrix;

uniform sampler2D ShadowMap;

 /*light in eye space*/
vec3 light_po_eye = vec3(ModelMatrix * vec4(light_position,1));

float CalculationShadow(vec4 ShadowCoord)
{
	//transform to [0,1] range
	vec3 ProCoords = ShadowCoord.xyz/ShadowCoord.w;
	ProCoords = ProCoords*0.5 + 0.5;

	/*calculate bias based depth map resolution and slope*/
   vec3 light_direc = normalize(light_po_eye - fs_in.FragPos);
   float bias = max(0.002 * (1.0 - dot(fs_in.Normal, light_direc)), 0.0002);

	/* compare whether in shadow or not */
	float visibility = 0.0;
	vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
    for(int y = -1; y <= 1; ++y)
        {
        float pcfDepth = texture(ShadowMap, ProCoords.xy + vec2(x, y) * texelSize).r; 
        visibility += ProCoords.z - bias > pcfDepth ? 0.0 : 0.2;        
        }    
     }

	visibility /= 9.0;

	if (ProCoords.z > 1.0f)
	visibility = 1.0; 

	return visibility;
}


void main()
{
  vec4 amb_color = vec4(0.1,0.1,0.1,1);
  vec4 diff_color = vec4(0.4,0.4,0,1);

  /*diffuse reflection*/
  vec3 light_direc = normalize(light_po_eye - fs_in.FragPos);

  /*compute lumpsum diffuse*/
  float diff = dot(light_direc,fs_in.Normal);	
  diff = (CalculationShadow(fs_in.ShadowCoor)) * diff;

   /*compute light intensity considering shadow*/	
   vec4 lighting =  vec4((1- diff) * amb_color + diff * diff_color) ;

   /*output final color*/
   gl_FragColor = lighting;

}			         
