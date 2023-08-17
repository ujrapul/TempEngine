// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// #version 330 core

#ifdef VERTEX_SHADER
layout(location = 0) in vec2 vertex; // <vec2 pos>

void main() {
  gl_Position = matrices.projection * matrices.view * model * vec4(vertex, 1.0, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
void main() {
  fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
#endif
