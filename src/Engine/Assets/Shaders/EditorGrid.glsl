// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

// #version 330 core

#ifdef VERTEX_SHADER
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

out vec3 vPosition;

void main()
{
  gl_Position = matrices.projection * matrices.view * vec4(vertex.xy * 999999999, -1.0, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER

// Grid line color with transparency (alpha channel)
const vec4 gridColor = vec4(0.5, 0.5, 0.5, 0.4); // Fully opaque white color with 50% transparency

// Center line color (red)
const vec4 centerLineColor = vec4(0.0, 0.0, 1.0, 1.0); // Fully opaque blue color

// Grid spacing and thickness
float gridSpacing = 50.0 * 1.f / matrices.scale; // Adjust this value to change grid spacing
const float gridThickness = 2.0;                 // Adjust this value to change grid line thickness

void main()
{
  // Extract the camera position (translation) from the view matrix
  vec3 cameraPosition = matrices.view[3].xyz;

  vec4 fragCoord = gl_FragCoord;

  // Calculate the absolute X and Y positions in the grid space, including camera offset
  float x = abs(fragCoord.x - matrices.resolutionX / 2 -
                cameraPosition.x * 1.f /
                  matrices.scale); // matrices.view[3][0] contains camera X offset
  float y = abs(fragCoord.y - matrices.resolutionY / 2 -
                cameraPosition.y * 1.f /
                  matrices.scale); // matrices.view[3][1] contains camera Y offset

  // Calculate the distance from the fragment to the nearest grid line in both X and Y directions
  float distToXLine = mod(x, gridSpacing);
  float distToYLine = mod(y, gridSpacing);

  // Combine the distances to create the grid effect
  float grid = 1.0 - step(gridThickness, distToXLine) * step(gridThickness, distToYLine);

  // Check if the fragment is on the center X or Y line and lies on the origin (0, 0)
  bool centerXLine = x >= 0 && x <= gridThickness;
  bool centerYLine = y >= 0 && y <= gridThickness;

  // Highlight the center line going through the origin with red color
  fragColor = grid == 1.0 ? ((centerXLine || centerYLine) ? centerLineColor : gridColor)
                          : vec4(0.0);
}
#endif