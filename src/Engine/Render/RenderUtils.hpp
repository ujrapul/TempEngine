// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#pragma once

#include "Camera.hpp"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include "gl.h"
#endif
#include "EngineUtils.hpp"
#include "FontLoader.hpp"
#include "Math.hpp"
#include "OpenGLWrapper.hpp"
#include "Shader.hpp"

namespace Temp::Render
{
  struct Square
  {
    // Define the square vertices
    float vertices[20] = {
      // positions      // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
    };

    float simpleVertices[8] = {
      // positions
      0.5f,
      0.5f, // top right
      0.5f,
      -0.5f, // bottom right
      -0.5f,
      -0.5f, // bottom left
      -0.5f,
      0.5f, // top left
    };

    float fontVertices[20] = {
      // positions      // texture coords
      0.5f,  0.5f,  0.0f, 1.0f / 11.f, 0.0f,        // top right
      0.5f,  -0.5f, 0.0f, 1.0f / 11.f, 1.0f / 12.f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f,        1.0f / 12.f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f,        0.0f         // top left
    };

    // Define the indices
    GLuint indices[6] = {
      0,
      1,
      3, // First Triangle
      1,
      2,
      3 // Second Triangle
    };

    GLuint VAO{};
    GLuint VBO{};
    GLuint EBO{};

    GLuint texture1{};
    GLuint texture2{};

    GLuint shaderProgram{};
  };

  struct Cube
  {
    float vertices[180] = {
      // positions           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

    GLuint VAO{};
    GLuint VBO{};

    GLuint texture1{};
    GLuint texture2{};

    GLuint shaderProgram{};
  };

  inline void Initialize(Square& square)
  {
    square.shaderProgram = OpenGLWrapper::CreateShaderProgram(ShaderIdx::TEST);

    // Create vertex array object (VAO)
    square.VAO = OpenGLWrapper::CreateVAO();

    // Create vertex buffer object (VBO)
    square.VBO = OpenGLWrapper::CreateVBO(square.vertices, 20);

    // Create Element Buffer Object (EBO) and copy index data
    square.EBO = OpenGLWrapper::CreateEBO(square.indices, 6);

    // Specify vertex attributes
    // position attribute
    OpenGLWrapper::SetVertexAttribArray(0, 3, 5, 0);
    OpenGLWrapper::SetVertexAttribArray(1, 2, 5, 3);

    // Unbind VAO and VBO and EBO
    // OpenGLWrapper::UnbindBuffers();

    OpenGLWrapper::BindUBOShader(Camera::UBO(), square.shaderProgram, "Matrices", 0);

    // load and create a texture
    // -------------------------
    square.texture1 = OpenGLWrapper::LoadTextureTGA(
      (AssetsDirectory() / "Images" / "ground.tga").c_str(),
      GL_BGRA,
      GL_LINEAR);
    square.texture2 = OpenGLWrapper::LoadTextureTGA(
      (AssetsDirectory() / "Images" / "surpriseface.tga").c_str(),
      GL_BGRA,
      GL_LINEAR);

    // Needs to be called to set variables in the shader!
    OpenGLWrapper::Set1IntShaderProperty(square.shaderProgram, "texture0", 0);
    OpenGLWrapper::Set1IntShaderProperty(square.shaderProgram, "texture1", 1);
  }

  inline void Initialize(Cube& cube)
  {
    cube.shaderProgram = OpenGLWrapper::CreateShaderProgram(ShaderIdx::TEST);

    // Create vertex array object (VAO)
    cube.VAO = OpenGLWrapper::CreateVAO();

    // Create vertex buffer object (VBO)
    cube.VBO = OpenGLWrapper::CreateVBO(cube.vertices, 180);

    // Specify vertex attributes
    // position attribute
    OpenGLWrapper::SetVertexAttribArray(0, 3, 5, 0);
    OpenGLWrapper::SetVertexAttribArray(1, 2, 5, 3);

    // Unbind VAO and VBO and EBO
    OpenGLWrapper::UnbindBuffers();

    // load and create a texture
    // -------------------------
    cube.texture1 = OpenGLWrapper::LoadTextureTGA(
      (AssetsDirectory() / "Images" / "ground.tga").c_str(),
      GL_BGRA,
      GL_LINEAR);
    cube.texture2 = OpenGLWrapper::LoadTextureTGA(
      (AssetsDirectory() / "Images" / "surpriseface.tga").c_str(),
      GL_BGRA,
      GL_LINEAR);

    // Needs to be called to set variables in the shader!
    OpenGLWrapper::Set1IntShaderProperty(cube.shaderProgram, "texture0", 0);
    OpenGLWrapper::Set1IntShaderProperty(cube.shaderProgram, "texture1", 1);
  }

  inline void Draw(const Square& square, float time)
  {
    // create transformations
    Math::Mat4 model{}; // make sure to initialize matrix to identity matrix first
    // model = model.translate(Math::Vec3f(0.5f, -0.5f, 0.0f));
    // model = model.rotateX(Math::ToRadians(55.f));
    model = model.rotate(time * Math::ToRadians(50.0f), Math::Vec3f(0.5, 1.0, 0));
    // // +z is back and -z is front (I think???)
    // view = view.translate(Math::Vec3f(0, 0, 3));
    model = model.scale({1.f});
    // projection = Math::Mat4::perspective(Math::ToRadians(45.f), 1.33333333333, 0.1, 100);
    // projection = Math::Mat4::orthographic(-1, 1, -1, 1, -10, 10);

    OpenGLWrapper::Set4x4MatrixShaderProperty(square.shaderProgram, "model", &model.rows[0][0]);
    // OpenGLWrapper::Set4x4MatrixShaderProperty(square.shaderProgram, "view", &view.rows[0][0]);
    // OpenGLWrapper::Set4x4MatrixShaderProperty(square.shaderProgram, "projection",
    // &projection.rows[0][0]);

    OpenGLWrapper::BindTexture(GL_TEXTURE0, square.texture1);
    OpenGLWrapper::BindTexture(GL_TEXTURE1, square.texture2);

    glUseProgram(square.shaderProgram);

    // Bind the VAO and draw the triangle
    OpenGLWrapper::DrawElements(square.VAO, 6, GL_FILL);
  }

  inline void Draw(const Cube& cube, float)
  {
    // create transformations
    Math::Mat4 model{}; // make sure to initialize matrix to identity matrix first
    model = model.translate(Math::Vec3f(-10.f, -10.f, 0.0f));
    // model = model.rotateX(Math::ToRadians(55.f));
    // model = model.rotate(time * Math::ToRadians(50.0f), Math::Vec3f(0.5, 1.0, 0));
    // +z is back and -z is front (I think???)
    // view = view.translate(Math::Vec3f(0, 0, 3));
    model = model.scale({10.f});
    // projection = Math::Mat4::perspective(Math::ToRadians(45.f), 1.33333333333, 0.1, 100);

    OpenGLWrapper::Set4x4MatrixShaderProperty(cube.shaderProgram, "model", &model.rows[0][0]);
    // OpenGLWrapper::Set4x4MatrixShaderProperty(cube.shaderProgram, "view", &view.rows[0][0]);
    // OpenGLWrapper::Set4x4MatrixShaderProperty(cube.shaderProgram, "projection",
    // &projection.rows[0][0]);

    // OpenGLWrapper::BindTexture(GL_TEXTURE0, cube.texture1);
    // OpenGLWrapper::BindTexture(GL_TEXTURE1, Font::Characters(Font::Type::ARIMO).at('A').texture);

    glUseProgram(cube.shaderProgram);

    // Bind the VAO and draw the triangle
    OpenGLWrapper::DrawArrays(cube.VAO, 180);
  }

  inline void Clean(Square& square)
  {
    OpenGLWrapper::CleanArrays(square.VAO);
    OpenGLWrapper::CleanArrayBuffer(square.VBO);
    OpenGLWrapper::CleanElementBuffer(square.EBO);
    OpenGLWrapper::CleanShader(square.shaderProgram);
  }

  inline void Clean(Cube& cube)
  {
    OpenGLWrapper::CleanArrays(cube.VAO);
    OpenGLWrapper::CleanArrayBuffer(cube.VBO);
    OpenGLWrapper::CleanShader(cube.shaderProgram);
  }
}
