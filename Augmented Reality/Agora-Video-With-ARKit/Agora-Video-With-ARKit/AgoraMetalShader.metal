//
//  AgoraMetalShader.metal
//  Agora-Video-With-ARKit
//
//  Created by GongYuhua on 2017/12/27.
//  Copyright © 2017年 Agora.io All rights reserved.
//

#include <metal_stdlib>

using namespace metal;

kernel void writeRGBFromYUV(texture2d<uint, access::read> yTexture [[texture(0)]],
                            texture2d<uint, access::read> uTexture [[texture(1)]],
                            texture2d<uint, access::read> vTexture [[texture(2)]],
                            texture2d<float, access::write> rgbTexture [[texture(3)]],
                            uint2 yPosition [[thread_position_in_grid]])
{
    float3x3 yuvToRGBTransform = float3x3(float3(+1.0000f, +1.0000f, +1.0000f),
                                          float3(+0.0000f, -0.3441f, +1.7720f),
                                          float3(+1.4020f, -0.7141f, +0.0000f));

    uint2 uvPosition = uint2(yPosition.x / 2, yPosition.y / 2);

    float3 yuvMatrix = float3(yTexture.read(yPosition).r / 255.0,
                              uTexture.read(uvPosition).r / 255.0 - 0.5,
                              vTexture.read(uvPosition).r / 255.0 - 0.5);

    float3 rgbMatrix = yuvToRGBTransform * yuvMatrix;

    rgbTexture.write(float4(float3(rgbMatrix), 1.0), yPosition);
}
