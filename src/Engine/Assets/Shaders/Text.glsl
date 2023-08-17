// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// #version 330 core

#ifdef VERTEX_SHADER
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

void main() {
  gl_Position = fontMatrices.projection * fontMatrices.view * model * vec4(vertex.xy, 10.0, 1.0);
  texCoords = vertex.zw;
}
#endif

#ifdef FRAGMENT_SHADER
precision mediump float;

uniform bool u_useOutline = false;
uniform vec4 u_color = vec4(1.0);
uniform vec4 u_outline_color = vec4(1.0f, 0.77f, 0.0f, 1.0f);
uniform float u_thickness = 0.5;
uniform float u_softness = 0.1;
uniform float u_outline_thickness = 0.5;
uniform float u_outline_softness = 0.1;

void main() {
  // fragColor = vec4(1.0, 1.0, 1.0, texture(texture0, texCoords).r);
  // fragColor = vec4(1.0, 1.0, 1.0, 1.0);
  // fragColor = applyOutlineGlow(texture0, texCoords);

  float alpha = texture(texture0, texCoords).r;

  float outline = 0;
  if(u_useOutline) {
    outline = 1.0 - smoothstep(u_outline_thickness - u_outline_softness, u_outline_thickness + u_outline_softness, alpha);
  }
  alpha = smoothstep(1.0 - u_thickness - u_softness, 1.0 - u_thickness + u_softness, alpha);
  // Discarding transparent fragments
  if(alpha <= 0 || alpha > 1.0) {
    discard;
  }
  fragColor = vec4(mix(applyOutlineGlow(texture0, texCoords).rgb, u_outline_color.rgb, outline), alpha);
}
#endif
