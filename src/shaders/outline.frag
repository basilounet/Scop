#version 330 core

out vec4 FragColor;

uniform float iTime;
uniform vec3 iResolution;
uniform float outlinePercentage; // 0 <= v <= 1

out vec2 v_texcoord;

const mat2 m = mat2( 0.80,  0.60, -0.60,  0.80 );

#define AA 2

float noise( in vec2 p )
{
    return sin(p.x)*sin(p.y);
}

float fbm4( vec2 p )
{
    float f = 0.0;
    f += 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p );
    return f/0.9375;
}

float fbm6( vec2 p )
{
    float f = 0.0;
    f += 0.500000*(0.5+0.5*noise( p )); p = m*p*2.02;
    f += 0.250000*(0.5+0.5*noise( p )); p = m*p*2.03;
    f += 0.125000*(0.5+0.5*noise( p )); p = m*p*2.01;
    f += 0.062500*(0.5+0.5*noise( p )); p = m*p*2.04;
    f += 0.031250*(0.5+0.5*noise( p )); p = m*p*2.01;
    f += 0.015625*(0.5+0.5*noise( p ));
    return f/0.96875;
}

vec2 fbm4_2( vec2 p )
{
    return vec2(fbm4(p), fbm4(p+vec2(7.8)));
}

vec2 fbm6_2( vec2 p )
{
    return vec2(fbm6(p+vec2(16.8)), fbm6(p+vec2(11.5)));
}

//====================================================================

float func( vec2 q, out vec4 ron )
{
    q += 0.03*sin( vec2(0.27,0.23)*iTime + length(q)*vec2(4.1,4.3));

    vec2 o = fbm4_2( 0.9*q );

    o += 0.04*sin( vec2(0.12,0.14)*iTime + length(o));

    vec2 n = fbm6_2( 3.0*o );

    ron = vec4( o, n );

    float f = 0.5 + 0.5*fbm4( 1.8*q + 6.0*n );

    return mix( f, f*f*f*3.5, f*abs(n.x) );
}

void mainImage2( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    float e = 2.0/iResolution.y;

    vec4 on = vec4(0.0);
    float f = func(p, on);

    vec3 col = vec3(0.0);
    col = mix( vec3(0.2,0.1,0.4), vec3(0.3,0.05,0.05), f );
    col = mix( col, vec3(0.9,0.9,0.9), dot(on.zw,on.zw) );
    col = mix( col, vec3(0.4,0.3,0.3), 0.2 + 0.5*on.y*on.y );
    col = mix( col, vec3(0.0,0.2,0.4), 0.5*smoothstep(1.2,1.3,abs(on.z)+abs(on.w)) );
    col = clamp( col*f*2.0, 0.0, 1.0 );

    #if 0
    // gpu derivatives - bad quality, but fast
    vec3 nor = normalize( vec3( dFdx(f)*iResolution.x, 6.0, dFdy(f)*iResolution.y ) );
    #else
    // manual derivatives - better quality, but slower
    vec4 kk;
    vec3 nor = normalize( vec3( func(p+vec2(e,0.0),kk)-f,
                          2.0*e,
                          func(p+vec2(0.0,e),kk)-f ) );
    #endif

    vec3 lig = normalize( vec3( 0.9, 0.2, -0.4 ) );
    float dif = clamp( 0.3+0.7*dot( nor, lig ), 0.0, 1.0 );
    vec3 lin = vec3(0.70,0.90,0.95)*(nor.y*0.5+0.5) + vec3(0.15,0.10,0.05)*dif;
    col *= 1.2*lin;
    col = 1.0 - col;
    col = 1.1*col*col;

    fragColor = vec4( col, 1.0 );
}

// ======================******============================================******======================

//2d rotation matrix
vec2 r(vec2 v,float t){float s=sin(t),c=cos(t);return mat2(c,-s,s,c)*v;}

// ACES tonemap: https://www.shadertoy.com/view/Xc3yzM
vec3 a(vec3 c)
{
    mat3 m1=mat3(0.59719,0.07600,0.02840,0.35458,0.90834,0.13383,0.04823,0.01566,0.83777);
    mat3 m2=mat3(1.60475,-0.10208,-0.00327,-0.53108,1.10813,-0.07276,-0.07367,-0.00605,1.07602);
    vec3 v=m1*c,a=v*(v+0.0245786)-0.000090537,b=v*(0.983729*v+0.4329510)+0.238081;
    return m2*(a/b);
}

//Xor's Dot Noise: https://www.shadertoy.com/view/wfsyRX
float no(vec3 p)
{
    const float PHI = 1.618033988;
    const mat3 GOLD = mat3(
    -0.571464913, +0.814921382, +0.096597072,
    -0.278044873, -0.303026659, +0.911518454,
    +0.772087367, +0.494042493, +0.399753815);
    return dot(cos(GOLD * p), sin(PHI * p * GOLD));
}

void mainImage1(out vec4 o,in vec2 U){
    vec3 color = vec3(0.0);
    for(int m = 0; m < AA; m++)
    for(int n = 0; n < AA; n++)
    {
        vec2 off = (vec2(float(m), float(n)) + 0.5) / float(AA) - 0.5;
        vec2 u = U + off;
        float i,s,t=iTime;
        vec3 p,l,b,d;p.z=-1.0-.5*sin(t*.1);
        d=normalize(vec3(2.*u-iResolution.xy,iResolution.y));
        for(o*=i;i<10.;i++){
            b=p;
            b.xy=r(sin(b.xy*.25),t*.5+b.z*2.);
            s=.001+abs(no(b*20.)/20.-no(b))*.7;
            s+=abs(p.y*.2+sin(p.z*2.+(abs(p.x)*.5)))*.5;
            p+=d*s;
            l+=(1.+1.5*sin(i+length(p.xy*.1)+2.+vec3(3,1.5,.5)))/s;
        }
        color+=a(l*l/5e2);
    }
    o = vec4(color / float(AA*AA), 1.0);
}

void main()
{
    vec4 color1 = vec4(0.0), color2 = vec4(0.0);
    if (outlinePercentage != 0.0)
        mainImage1(color2, gl_FragCoord.xy);
    if (outlinePercentage != 1.0)
        mainImage2(color1, gl_FragCoord.xy);
    FragColor = mix(color1, color2, outlinePercentage);
//    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}