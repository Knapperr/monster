// Not exactly sure how this works... but i need to use it
float4 ps(PS_INPUT input) : SV_TARGET
{

        // the pixel is the uv * textureSize; i wonder if texture size is because he is loading 32x32
        // could be the full thing though 
        //  
        //
      float2 Pixel = input.uv*TextureSize;
      // seam not too sure its the 
      float2 Seam = floor(Pixel + 0.5);
      float2 DuDv = fwidth(Pixel);
      Pixel = Seam + clamp((Pixel - Seam)/DuDv, -0.5, 0.5);
      float2 ModifiedTextureCoordinate = Pixel/TextureSize;
      float4 tex = texture0.Sample(sampler0, float4(ModifiedTextureCoordinate, input.textureID));
      tex = float4(tex[0], tex[1], tex[2], input.alpha*tex[3]);
      return tex;
}

