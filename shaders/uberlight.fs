#version 150

// Light parameters
uniform vec3 LightColor;
uniform vec3 LightWeights;

// Surface parameters
uniform vec3 SurfaceWeights;
uniform float SurfaceRoughness;
uniform int AmbientClamping;

// Super ellipse shaping parameters
uniform int BarnShaping;
uniform float SeWidth;
uniform float SeHeight;
uniform float SeWidthEdge;
uniform float SeHeightEdge;
uniform float SeRoundness;

// Distance shaping parameters
uniform float DsNear;
uniform float DsFar;
uniform float DsNearEdge;
uniform float DsFarEdge;

in vec2 v_tex;

in vec3 LCpos;          // Vertex position in light coordinates
in vec3 LCnorm;         // Normal in light coordinates
in vec3 LCcamera;       // Camera position in light coordinates

uniform sampler2D s_tex;

out vec4 FragColor;


float superEllipseShape(vec3 pos)
{
   if (BarnShaping == 0)
       return 1.0;
   else
   {

       // Project the point onto the z = 1.0 plane
      vec2 ppos = pos.xy / pos.z;
      vec2 abspos = abs(ppos);

      float w = SeWidth;
      float W = SeWidth + SeWidthEdge;
      float h = SeHeight;
      float H = SeHeight + SeHeightEdge;

      float exp1 = 2.0 / SeRoundness;
      float exp2 = -SeRoundness / 2.0;

      float inner = w * h * pow(pow(h * abspos.x, exp1) +
                                pow(w * abspos.y, exp1), exp2);
      float outer = W * H * pow(pow(H * abspos.x, exp1) +
                                pow(W * abspos.y, exp1), exp2);

      return 1.0 - smoothstep(inner, outer, 1.0);
   }
}
float distanceShape(vec3 pos)
{
   float depth;

   depth = abs(pos.z);

   float dist = smoothstep(DsNear - DsNearEdge, DsNear, depth) *
                (1.0 - smoothstep(DsFar, DsFar + DsFarEdge, depth));
   return dist;
}

void main()
{
      vec3 tmpLightColor = LightColor;
      
      vec3 N = normalize(LCnorm);
      vec3 L = -normalize(LCpos);
      vec3 V = normalize(LCcamera-LCpos);
      vec3 H = normalize(L + V);
      
      vec3 tmpColor = tmpLightColor;

      float attenuation = 1.0;
      attenuation *= superEllipseShape(LCpos);
      attenuation *= distanceShape(LCpos);

      float ndotl = dot(N, L);
      float ndoth = dot(N, H);

      vec3 litResult;

      litResult[0] = AmbientClamping == 1 ? max(ndotl, 0.0) : 1.0;
      litResult[1] = max(ndotl, 0.0);
      litResult[2] = litResult[1] * max(ndoth, 0.0) * SurfaceRoughness;

      litResult *= SurfaceWeights * LightWeights;

      vec4 SurfaceColorTmp = texture(s_tex, v_tex);
      vec3 SurfaceColor = vec3(SurfaceColorTmp.x, SurfaceColorTmp.y, SurfaceColorTmp.z);

      vec3 ambient = tmpLightColor * SurfaceColor * litResult[0];
      vec3 diffuse = tmpColor * SurfaceColor * litResult[1];
      vec3 specular = tmpColor * litResult[2];      

      FragColor = vec4(attenuation * (ambient + diffuse + specular), 1.0);

}
