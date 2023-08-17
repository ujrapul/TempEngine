// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// #version 330 core

#ifdef VERTEX_SHADER
layout(location = 0) in vec2 vertex; // <vec3 pos, vec2 tex>
layout(location = 1) in vec2 in_texCoords; // <vec3 pos, vec2 tex>

void main() {
  gl_Position = matrices.projection * matrices.view * model * vec4(vertex, 0.0, 1.0);
  texCoords = in_texCoords;
}
#endif

#ifdef FRAGMENT_SHADER

void main()
{
  fragColor = texture(texture0, texCoords);
}
#endif