// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// #version 330 core

#ifdef VERTEX_SHADER
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// uniform mat4 view;
// uniform mat4 projection;

void main()
{
  gl_Position = matrices.projection * matrices.view * model * vec4(aPos.xy * 100, 0.0, 1.0);
  texCoords = vec2(aTexCoord.x, aTexCoord.y);
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 ourColor;

void main()
{
  // linearly interpolate between both textures (80% container, 20% awesomeface)
  fragColor = mix(texture(texture0, texCoords), texture(texture1, texCoords), 0.2);
  // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texture1, TexCoord).r);
  // vec4 color = vec4(1.0, 1.0, 1.0, 1.0) * sampled;
  // FragColor = mix(texture(texture0, TexCoord), color, 0.5);
  // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
#endif