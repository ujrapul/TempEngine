// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#ifdef GL_ES
precision mediump float;
#endif

// Reference: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
// NOTE: Be careful when using vec3s or float arrays that are not multiples of 4
// They will be padded to 16 byte offsets
layout(std140) uniform Matrices {
                      // Base alignment // Aligned offset (Starting byte, always multiple of 16)
  mat4 projection;    // 16             // 0   (col1)
                      // 16             // 16  (col2)
                      // 16             // 32  (col3)
                      // 16             // 48  (col4)
                      //
  mat4 view;          // 16             // 64  (col1)
                      // 16             // 80  (col2)
                      // 16             // 96  (col3)
                      // 16             // 128 (col4)
  float scale;        // 4              // 144 (float)
  float resolutionX;  // 4              // 148 (float)
  float resolutionY;  // 4              // 152 (float)
} matrices;

// Reference: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
layout(std140) uniform FontMatrices {
                   // Base alignment // Aligned offset (Starting byte, always multiple of 16)
  mat4 projection; // 16             // 0   (col1)
                   // 16             // 16  (col2)
                   // 16             // 32  (col3)
                   // 16             // 48  (col4)
                   //
  mat4 view;       // 16             // 64  (col1)
                   // 16             // 80  (col2)
                   // 16             // 96  (col3)
                   // 16             // 128 (col4)
  float scale;
  float resolutionX;
  float resolutionY;
} fontMatrices;

uniform float u_time;
uniform int u_numIndices;

float psuedoRand(vec2 co){
  return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 normalizeVec2(vec2 v) {
  float len = length(v);
  if (len > 0.0) {
    return v / len;
  }
  return vec2(0.0); // Return a zero vector if the input is a zero vector
}

#ifdef VERTEX_SHADER
uniform mat4 model;
out vec2 texCoords;
#endif

#ifdef FRAGMENT_SHADER
const float offset = 1.0 / 384.0;
const int width = 3;

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 texCoords;
out vec4 fragColor;

vec4 applyOutlineGlow(sampler2D text, vec2 in_texCoords) {
  // vec4 col = texture2D(text, texCoords);
  // if(col.r > 0.5) {
  //   return vec4(1.0, 1.0, 1.0, col.r);
  // }
  // float a = 0;
  // for(int i = 0; i < width; ++i) {
  //   float currOffset = offset * i;
  //   a +=
  //     texture2D(text, vec2(texCoords.x, texCoords.y)).r +
  //     texture2D(text, vec2(texCoords.x + currOffset, texCoords.y)).r +
  //     texture2D(text, vec2(texCoords.x - currOffset, texCoords.y)).r +
  //     texture2D(text, vec2(texCoords.x, texCoords.y + currOffset)).r +
  //     texture2D(text, vec2(texCoords.x, texCoords.y - currOffset)).r +
  //     texture2D(text, vec2(texCoords.x + currOffset, texCoords.y + currOffset)).r +
  //     texture2D(text, vec2(texCoords.x + currOffset, texCoords.y - currOffset)).r +
  //     texture2D(text, vec2(texCoords.x - currOffset, texCoords.y + currOffset)).r +
  //     texture2D(text, vec2(texCoords.x - currOffset, texCoords.y - currOffset)).r;
  // }
  // if(a > 0.0) {
  //   return vec4(0.0, 0.0, 0.0, 1.0);
  // }
  // return vec4(col.r);

  // return vec4( 1.0 - sobel.rgb, texture(text, texCoords).r );
  return vec4(1.0, 1.0, 1.0, texture(text, in_texCoords).r);
}
#endif
