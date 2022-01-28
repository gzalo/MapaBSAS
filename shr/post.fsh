#version 330

in vec2 texCoord;
out vec4 color;
uniform sampler2DMS colorTexture;
uniform sampler2DMS depthTexture;

const float zNear = 1.0f;
const float zFar = 11500.0f;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

#if 0
float getDepth(vec2 coords){
	float z_b = texture(depthTexture, coords).r;
    float z_n = 2.0 * z_b - 1.0;
    float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
	return z_n;
}

vec3 normalFromDepth(float depth, vec2 coords){
	const vec2 offset1 = vec2(0.0,0.001);
	const vec2 offset2 = vec2(0.001,0.0);
	
	float depth1 = getDepth(coords+offset1);
	float depth2 = getDepth(coords+offset2);
	
	vec3 p1 = vec3(offset1, depth1-depth);
	vec3 p2 = vec3(offset2, depth2-depth);
	
	vec3 normal = cross(p1,p2);
	
	return normalize(normal);
}
#endif

const vec3 kernel[8] = vec3[](
    normalize( vec3( 1, 1, 1 ) ) * 0.125f,  
    normalize( vec3( -1,-1,-1 ) ) * 0.250f,  
    normalize( vec3( -1,-1, 1 ) ) * 0.375f,  
    normalize( vec3( -1, 1,-1 ) ) * 0.500f,  
    normalize( vec3( -1, 1 ,1 ) ) * 0.625f,  
    normalize( vec3( 1,-1,-1 ) ) * 0.750f,  
    normalize( vec3( 1,-1, 1 ) ) * 0.875f,  
    normalize( vec3( 1, 1,-1 ) ) * 1.000f  
);

vec4 textureGet(sampler2DMS samp, vec2 coord){
	return 0.25*(texelFetch(samp, ivec2(coord*vec2(1024.0,768.0)), 0)+
		texelFetch(samp, ivec2(coord*vec2(1024.0,768.0)), 1)+
		texelFetch(samp, ivec2(coord*vec2(1024.0,768.0)), 2)+
		texelFetch(samp, ivec2(coord*vec2(1024.0,768.0)), 3));
}

//SSAO basado en http://content.gpwiki.org/D3DBook:Screen_Space_Ambient_Occlusion

void main(){
	ivec2 pos = ivec2(texCoord*vec2(1024,768));
	
    vec3 random = vec3(rand(texCoord.xy),rand(texCoord.xy+vec2(0.23,0.35)),rand(texCoord.xy)+vec2(0.48,0.56))*2-1;
      
    // Specified in either texels or meters  
    float fRadius = 0.5f;  
    float fPixelDepth = 1 - (textureGet(depthTexture, texCoord).r);  
    float fDepth = fPixelDepth * zFar;  
      
    vec3 vKernelScale = vec3(fRadius / fDepth, fRadius / fDepth, fRadius / zFar);  
      
    float fOcclusion = 0;
    
#define NUM_PASSES 4  
    for(int j = 0; j < NUM_PASSES; j++)  
    {  
        vec3 random = vec3( rand(texCoord.xy*(7+j)),
							rand(texCoord.xy*(13+j)),
							rand(texCoord.xy*(17+j))
							);
        random = random * 2 - 1;  
          
        for(int i = 0; i < 8; i++)  
        {  
            vec3 vRotatedKernel = reflect(kernel[i], random) * vKernelScale;  
              
            float fSampleDepth = 1 - (textureGet(depthTexture, vRotatedKernel.xy + texCoord).r);  
            float fDelta = max(fSampleDepth - fPixelDepth + vRotatedKernel.z, 0);  
            float fRange = abs(fDelta) / (vKernelScale.z * 3.0);  
            fOcclusion += mix(fDelta * 500.0, 0.5, clamp(fRange,0.0,1.0));  
        }  
    }  
      
    fOcclusion = fOcclusion / (NUM_PASSES * 8);  
     
	vec3 albedo = 0.25*(texelFetch(colorTexture, pos, 0)+texelFetch(colorTexture, pos, 1)+texelFetch(colorTexture, pos, 2)+texelFetch(colorTexture, pos, 3)).rgb;
	color.rgb =(1.0-fOcclusion)*albedo;
	color.a = 1.0;
}