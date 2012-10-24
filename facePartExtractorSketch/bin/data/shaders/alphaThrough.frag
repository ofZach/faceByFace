#extension GL_ARB_texture_rectangle : enable  
  
varying vec2 texCoord;  
uniform sampler2DRect tex0;



void main()  
{   
    vec4 tex2val = texture2DRect(tex0, texCoord);
    gl_FragColor = tex2val;
	gl_FragColor.a = tex2val.a;
}


