#version 330 core

out vec4 FragColor;

uniform sampler2D   texture0;
uniform float iTime;
uniform vec2 iMouse;
uniform vec3 iResolution;
uniform float outlinePercentage; // 0 <= v <= 1

out vec2 v_texcoord;

#define AA 1  //change to 1 to increase performance

#define _Speed 5.0  //disk rotation speed

#define _Steps  5. //disk texture layers
#define _Size 0.3//size of BH



float hash(float x){ return fract(sin(x)*152754.742);}
float hash(vec2 x){	return hash(x.x + hash(x.y));}

float value(vec2 p, float f) //value noise
{
    float bl = hash(floor(p*f + vec2(0.,0.)));
    float br = hash(floor(p*f + vec2(1.,0.)));
    float tl = hash(floor(p*f + vec2(0.,1.)));
    float tr = hash(floor(p*f + vec2(1.,1.)));

    vec2 fr = fract(p*f);
    fr = (3. - 2.*fr)*fr*fr;
    float b = mix(bl, br, fr.x);
    float t = mix(tl, tr, fr.x);
    return  mix(b,t, fr.y);
}

vec4 background(vec3 ray)
{
    vec2 uv = ray.xy;

    if( abs(ray.x) > 0.5)
    uv.x = ray.z;
    else if( abs(ray.y) > 0.5)
    uv.y = ray.z;


    float brightness = value( uv*3., 100.); //(poor quality) "stars" created from value noise
    float color = value( uv*2., 20.);
    brightness = pow(brightness, 256.);

    brightness = brightness*100.;
    brightness = clamp(brightness, 0., 1.);

    vec3 stars = brightness * mix(vec3(1., .6, .2), vec3(.2, .6, 1), color);

    vec4 nebulae = texture(texture0, (uv*1.5 ));
    nebulae.xyz += nebulae.xxx + nebulae.yyy + nebulae.zzz; //average color
    nebulae.xyz *= 0.25;

    nebulae*= nebulae;
    nebulae*= nebulae;
    nebulae*= nebulae;
    nebulae*= nebulae;

    nebulae.xyz += stars;
    return nebulae;
}

vec4 raymarchDisk(vec3 ray, vec3 zeroPos)
{
    //return vec4(1.,1.,1.,0.); //no disk

    vec3 position = zeroPos;
    float lengthPos = length(position.xz);
    float dist = min(1., lengthPos*(1./_Size) *0.5) * _Size * 0.4 *(1./_Steps) /( abs(ray.y) );

    position += dist*_Steps*ray*0.5;

    vec2 deltaPos;
    deltaPos.x = -zeroPos.z*0.01 + zeroPos.x;
    deltaPos.y = zeroPos.x*0.01 + zeroPos.z;
    deltaPos = normalize(deltaPos - zeroPos.xz);

    float parallel = dot(ray.xz, deltaPos);
    parallel /= sqrt(lengthPos);
    parallel *= 0.5;
    float redShift = parallel +0.3;
    redShift *= redShift;

    redShift = clamp(redShift, 0., 1.);

    float disMix = clamp((lengthPos - _Size * 2.)*(1./_Size)*0.24, 0., 1.);
    vec3 insideCol =  mix(vec3(1.0,0.8,0.0), vec3(0.5,0.13,0.02)*0.2, disMix);

    insideCol *= mix(vec3(0.4, 0.2, 0.1), vec3(1.6, 2.4, 4.0), redShift);
    insideCol *= 1.25;
    redShift += 0.12;
    redShift *= redShift;

    vec4 o = vec4(0.);

    for(float i = 0. ; i < _Steps; i++)
    {
        position -= dist * ray ;

        float intensity =clamp( 1. - abs((i - 0.8) * (1./_Steps) * 2.), 0., 1.);
        float lengthPos = length(position.xz);
        float distMult = 1.;

        distMult *=  clamp((lengthPos -  _Size * 0.75) * (1./_Size) * 1.5, 0., 1.);
        distMult *= clamp(( _Size * 10. -lengthPos) * (1./_Size) * 0.20, 0., 1.);
        distMult *= distMult;

        float u = lengthPos + iTime* _Size*0.3 + intensity * _Size * 0.2;

        vec2 xy ;
        float rot = mod(iTime*_Speed, 8192.);
        xy.x = -position.z*sin(rot) + position.x*cos(rot);
        xy.y = position.x*sin(rot) + position.z*cos(rot);

        float x = abs( xy.x/(xy.y));
        float angle = 0.02*atan(x);

        const float f = 70.;
        float noise = value( vec2( angle, u * (1./_Size) * 0.05), f);
        noise = noise*0.66 + 0.33*value( vec2( angle, u * (1./_Size) * 0.05), f*2.);

        float extraWidth =  noise * 1. * (1. -  clamp(i * (1./_Steps)*2. - 1., 0., 1.));

        float alpha = clamp(noise*(intensity + extraWidth)*( (1./_Size) * 10.  + 0.01 ) *  dist * distMult , 0., 1.);

        vec3 col = 2.*mix(vec3(0.3,0.2,0.15)*insideCol, insideCol, min(1.,intensity*2.));
        o = clamp(vec4(col*alpha + o.rgb*(1.-alpha), o.a*(1.-alpha) + alpha), vec4(0.), vec4(1.));

        lengthPos *= (1./_Size);

        o.rgb+= redShift*(intensity*1. + 0.5)* (1./_Steps) * 100.*distMult/(lengthPos*lengthPos);
    }

    o.rgb = clamp(o.rgb - 0.005, 0., 1.);
    return o ;
}


void Rotate( inout vec3 vector, vec2 angle )
{
    vector.yz = cos(angle.y)*vector.yz
    +sin(angle.y)*vec2(-1,1)*vector.zy;
    vector.xz = cos(angle.x)*vector.xz
    +sin(angle.x)*vec2(-1,1)*vector.zx;
}

void mainImage2( out vec4 colOut, in vec2 fragCoord )
{
    colOut = vec4(0.);;

    vec2 fragCoordRot;
    fragCoordRot.x = fragCoord.x*0.985 + fragCoord.y * 0.174;
    fragCoordRot.y = fragCoord.y*0.985 - fragCoord.x * 0.174;
    fragCoordRot += vec2(-0.06, 0.12) * iResolution.xy;

    for( int j=0; j<AA; j++ )
    for( int i=0; i<AA; i++ )
    {
        //setting up camera
        vec3 ray = normalize( vec3((fragCoordRot-iResolution.xy*.5  + vec2(i,j)/(float(AA)))/iResolution.x, 1 ));
        vec3 pos = vec3(0.,0.05,-(20.*iMouse.xy/iResolution.y-10.)*(20.*iMouse.xy/iResolution.y-10.)*.05);
        vec2 angle = vec2(iTime*0.1,.2);
        angle.y = (2.*iMouse.y/iResolution.y)*3.14 + 0.1 + 3.14;
        float dist = length(pos);
        Rotate(pos,angle);
        angle.xy -= min(.3/dist , 3.14) * vec2(1, 0.5);
        Rotate(ray,angle);

        vec4 col = vec4(0.);
        vec4 glow = vec4(0.);
        vec4 outCol =vec4(100.);

        for(int disks = 0; disks< 256; disks++) //steps
        {

            for (int h = 0; h < 6; h++) //reduces tests for exit conditions (to minimise branching)
            {
                vec3 p1 = pos;
                float dotpos = dot(pos,pos);
                float invDist = inversesqrt(dotpos); //1/distance to BH
                float centDist = dotpos * invDist; 	//distance to BH


                // stepDist was originally abs(pos.y /(ray.y)) * 0.9
                // the min(dotpos * 0.2,...) was added to maintain shorter step distances when far from the disk.
                float stepDist = min(dotpos * 0.2, abs(pos.y /(ray.y)) * 0.9);  //conservative distance to disk (y==0), but clamped to a max for more accurate frame dragging (the light could escape the BH with too large of steps)
                float farLimit = centDist * 0.5; //limit step size far from to BH
                float closeLimit = min(centDist*0.1,  0.01*centDist*centDist*(1./_Size)); //limit step size closse to BH
                stepDist = min(stepDist, min(farLimit, closeLimit));

                float invDistSqr = invDist * invDist;
                float bendForce = stepDist * invDistSqr * _Size * 0.625;  //bending force

                // Frame dragging computation below:
                // Compute a vector in the direction of the BH's rotation
                vec3 pos_cross_pole_axis = cross(vec3(0., 1., 0.), pos);
                // Compute sin^2(latitude) of the current ray position
                float sin2_colatitude = length(pos_cross_pole_axis)/length(pos);
                sin2_colatitude = sin2_colatitude * sin2_colatitude;

                // the frame dragging rate is approximately proportional to 1/(r^2 * sin^2(colatitude)) (See: https://en.wikipedia.org/wiki/Frame-dragging)
                vec3 frameDragForce = normalize(pos_cross_pole_axis) * invDistSqr * sin2_colatitude * 0.015;


                ray =  normalize(ray - (bendForce * invDist)*pos);  //bend ray towards BH
                pos = p1 + stepDist * (ray - frameDragForce) ; // Move the ray in the ray direction and move it according to frame dragging
                // This could be acheived more accurately by computing a rotation matrix and using it to rotate the ray position & direction.

                glow += vec4(1.2,1.1,1, 1.0) *(0.01*stepDist * invDistSqr * invDistSqr *clamp( centDist*(2.) - 1.2,0.,1.)); //adds fairly cheap glow
            }

            float dist2 = length(pos);

            if(dist2 < _Size * 0.5) //ray sucked in to BH
            {
                outCol =  vec4( col.rgb * col.a + glow.rgb *(1.-col.a ) ,1.) ;
                break;
            }

            else if(dist2 > _Size * 1000.) //ray escaped BH
            {
                vec4 bg = background (ray);
                outCol = vec4(col.rgb*col.a + bg.rgb*(1.-col.a)  + glow.rgb *(1.-col.a    ), 1.);
                break;
            }

            else if (abs(pos.y) <= _Size * 0.0005 ) //ray hit accretion disk
            {
                vec4 diskCol = raymarchDisk(ray, pos);   //render disk
                //pos.y = 0.;
                pos += abs(_Size * 0.001 /ray.y) * ray;  // skip over disk (keeps rays from getting trapped inside disk)
                col = vec4(diskCol.rgb*(1.-col.a) + col.rgb, col.a + diskCol.a*(1.-col.a));
            }
        }

        //if the ray never escaped or got sucked in
        if(outCol.r == 100.)
        outCol = vec4(col.rgb + glow.rgb *(col.a +  glow.a) , 1.);

        col = outCol;
        col.rgb =  pow( col.rgb, vec3(0.6) );

        colOut += col/float(AA*AA);
    }
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