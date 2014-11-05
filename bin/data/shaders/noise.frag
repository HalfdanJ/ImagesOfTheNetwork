uniform sampler2DRect tex0;
uniform float globalTime;

float rand(vec2 co)
{
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt= dot(co.xy ,vec2(a,b));
    float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}


void main()
{
    vec2 uv = gl_TexCoord[0].xy;// gl_FragCoord.xy;// / iResolution.xy;
    // Flip Y Axis
    //uv.y = -uv.y;
    float a = texture2DRect(tex0, uv).a;
//    a += 0.05;
    float inva = 1./a;
    
    float randomNum = rand(vec2(globalTime+uv.x, globalTime+uv.y));
//    magnitude -= randomNum;
    
    // Set up offset
    /*vec2 offsetRedUV = uv;
    offsetRedUV.x = uv.x + rand(vec2(globalTime*0.03,uv.y*0.42)) * 10.;
    offsetRedUV.x += 0.1*sin(rand(vec2(globalTime*0.2, uv.y)))*magnitude;
    
    vec2 offsetGreenUV = uv;
    offsetGreenUV.x = uv.x + rand(vec2(globalTime*0.004,uv.y*0.002)) * 4.;
    offsetGreenUV.x +=  0.1*sin(globalTime*2.0+uv.y*0.1*(magnitude))*magnitude;
    
    vec2 offsetBlueUV = uv;
    offsetBlueUV.x = uv.y;
    offsetBlueUV.x += rand(vec2(cos(globalTime*0.01),sin(uv.y)));
    
     */
    
    float segX = floor(uv.x/(200.+ rand(vec2(uv.y))*0.1 ));
    float segY = floor(uv.y/(8. + rand(vec2(uv.y))*0.1 ));
    
    vec2 offset = uv;
    offset.x += (rand(vec2(segX,segY))-0.5)*0.8*inva;
    offset.y += (rand(vec2(segY,segX))-0.5)*0.5*inva;
    
    vec2 offsetR = offset;
    offsetR.x += (rand(vec2(segX,segY))-0.5)*0.5*inva;
    offsetR.y += (rand(vec2(segY,segX))-0.5)*0.2*inva;
    
    vec2 offsetG = offset;
    vec2 offsetB = offset;
    
    // Load Texture
    float r = texture2DRect(tex0, offsetR).r*1.8;
    float g = texture2DRect(tex0, offsetG).g*1.8;
    float b = texture2DRect(tex0, offsetB).b*1.8;
    
    gl_FragColor = vec4(r,g,b,1.);
    
    
 //   gl_FragColor = gl_Color * texture2DRect(tex0, gl_TexCoord[0].xy);
}
