// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Event.hpp"
#include "WinRender.hpp"
#include "Engine.hpp"
#include "gl.h"
#include "wgl.h"

#include "STDPCH.hpp"

#ifdef EDITOR
#include "CommonRender.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#endif

#include <stdio.h>
#include <windowsx.h>

#ifdef EDITOR
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);
#endif

namespace Temp::Render
{
  namespace
  {
    HWND hWnd;
    HDC hdc;
    HGLRC hglrc;
    Scene::Data* winScene{nullptr};
    Event::Data EventData{};
#ifdef EDITOR
    bool imguiInitialized{false};
#endif

    // Function to center the window on the screen
    void CenterWindow()
    {
      RECT windowRect;
      GetWindowRect(hWnd, &windowRect);

      int screenWidth = GetSystemMetrics(SM_CXSCREEN);
      int screenHeight = GetSystemMetrics(SM_CYSCREEN);

      int windowWidth = windowRect.right - windowRect.left;
      int windowHeight = windowRect.bottom - windowRect.top;

      int xPos = (screenWidth - windowWidth) / 2;
      int yPos = (screenHeight - windowHeight) / 2;

      SetWindowPos(hWnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    void ToggleFullscreen()
    {
      static DWORD prevStyle;
      static WINDOWPLACEMENT prevPlacement = {sizeof(WINDOWPLACEMENT)};

      DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

      if (dwStyle & WS_OVERLAPPEDWINDOW)
      {
        MONITORINFO mi = {sizeof(MONITORINFO)};
        if (GetWindowPlacement(hWnd, &prevPlacement) &&
            GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi))
        {
          prevStyle = dwStyle;
          SetWindowLong(hWnd, GWL_STYLE, prevStyle & ~WS_OVERLAPPEDWINDOW);
          SetWindowPos(hWnd,
                       HWND_TOP,
                       mi.rcMonitor.left,
                       mi.rcMonitor.top,
                       mi.rcMonitor.right - mi.rcMonitor.left,
                       mi.rcMonitor.bottom - mi.rcMonitor.top,
                       SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
      }
      else
      {
        SetWindowLong(hWnd, GWL_STYLE, prevStyle);
        SetWindowPlacement(hWnd, &prevPlacement);
        SetWindowPos(hWnd,
                     NULL,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
      }
    }

    // Function to get the screen size
    void GetScreenSize(int& screenWidth, int& screenHeight)
    {
      screenWidth = GetSystemMetrics(SM_CXSCREEN);
      screenHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    LRESULT CALLBACK WindowProc(HWND hWndLocal, UINT message, WPARAM wParam, LPARAM lParam)
    {
#ifdef EDITOR
      ImGui_ImplWin32_WndProcHandler(hWndLocal, message, wParam, lParam);
      if (!imguiInitialized)
      {
        return DefWindowProc(hWndLocal, message, wParam, lParam);
      }
      auto& io = ImGui::GetIO();
#endif
      switch (message)
      {
        case WM_CREATE:
        {
          RECT clientRect;
          GetClientRect(hWnd, &clientRect);

          // When the window is created, center it on the screen
          int windowWidth = clientRect.right - clientRect.left;
          int windowHeight = clientRect.bottom - clientRect.top;

          int screenWidth, screenHeight;
          GetScreenSize(screenWidth, screenHeight);

          int xPos = (screenWidth - windowWidth) / 2;
          int yPos = (screenHeight - windowHeight) / 2;

          SetWindowPos(hWnd, nullptr, xPos, yPos, windowWidth, windowHeight, SWP_NOZORDER);

          break;
        }
        case WM_CLOSE:
          DestroyWindow(hWndLocal);
          Global::Quit();
          break;
        case WM_DESTROY:
          PostQuitMessage(0);
          Global::Quit();
          break;
        case WM_SETCURSOR:
          // Set the cursor to be the arrow cursor (normal)
          SetCursor(LoadCursor(NULL, IDC_ARROW));
          return TRUE; // Return TRUE to indicate that we handled the message
        case WM_KEYDOWN:
        {
          if (wParam == VK_F11) // Replace VK_F11 with the key you want to use
          {
            ToggleFullscreen();
          }
          auto key = static_cast<Input::KeyboardCode>(wParam);
          if ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_LeftAlt;
          }
          else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_RightAlt;
          }
          else if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_LeftCtrl;
          }
          else if ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_RightCtrl;
          }
          else if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_LeftShift;
          }
          else if ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0)
          {
            key = Input::KeyboardCode::KB_RightShift;
          }
          Global::PushPressKeyQueue(key);
          Event::KeyPressed(EventData, key);
        }
        break;
        case WM_KEYUP:
        {
          auto key = static_cast<Input::KeyboardCode>(wParam);
          if (wParam == VK_MENU && (GetAsyncKeyState(VK_LMENU) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_LeftAlt;
          }
          else if (wParam == VK_MENU && (GetAsyncKeyState(VK_RMENU) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_RightAlt;
          }
          else if (wParam == VK_CONTROL && (GetAsyncKeyState(VK_LCONTROL) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_LeftCtrl;
          }
          else if (wParam == VK_CONTROL && (GetAsyncKeyState(VK_RCONTROL) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_RightCtrl;
          }
          else if (wParam == VK_SHIFT && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_LeftShift;
          }
          else if (wParam == VK_SHIFT && (GetAsyncKeyState(VK_RSHIFT) & 0x8000) == 0)
          {
            key = Input::KeyboardCode::KB_RightShift;
          }
          Global::PushReleaseKeyQueue(key);
          Event::KeyReleased(EventData, key);
        }
        break;
        case WM_MOUSEMOVE:
        {
          int mouseX = GET_X_LPARAM(lParam); // Get the x-coordinate of the cursor
          int mouseY = GET_Y_LPARAM(lParam); // Get the y-coordinate of the cursor
          Event::Hover(*winScene, EventData, (float)mouseX, (float)mouseY);
        }
        break;
        case WM_LBUTTONDOWN:
        {
          // Right mouse button was pressed
          int mouseX = GET_X_LPARAM(lParam); // Get the x-coordinate of the mouse click
          int mouseY = GET_Y_LPARAM(lParam); // Get the y-coordinate of the mouse click

          Event::ButtonPressed(*winScene, EventData, 1, (float)mouseX, (float)mouseY);

          break;
        }
        case WM_LBUTTONUP:
        {
#ifdef EDITOR
          if (io.WantCaptureMouse)
          {
            break;
          }
#endif
          // Right mouse button was pressed
          int mouseX = GET_X_LPARAM(lParam); // Get the x-coordinate of the mouse click
          int mouseY = GET_Y_LPARAM(lParam); // Get the y-coordinate of the mouse click
          Event::ButtonReleased(*winScene, EventData, (float)mouseX, (float)mouseY, 1);

          break;
        }
        case WM_RBUTTONDOWN:
        {
#ifdef EDITOR
          if (io.WantCaptureMouse)
          {
            break;
          }
#endif
          break;
        }
        case WM_RBUTTONUP:
        {
#ifdef EDITOR
          if (io.WantCaptureMouse)
          {
            break;
          }
#endif
          break;
        }
        case WM_SIZE:
        {
          auto& windowWidth = EventData.windowWidth;
          auto& windowHeight = EventData.windowHeight;

          int currentWidth = LOWORD(lParam);
          int currentHeight = HIWORD(lParam);
          if (currentWidth != windowWidth || currentHeight != windowHeight)
          {
            Camera::UpdateCameraAspect(*winScene, (float)currentWidth, (float)currentHeight);
            windowWidth = currentWidth;
            windowHeight = currentHeight;

            Scene::EnqueueRender(
              *winScene,
              [](Scene::Data& scene, void* data) { Event::Resize(scene, EventData, data); },
              nullptr);
          }
        }
        break;
        default:
          return DefWindowProc(hWndLocal, message, wParam, lParam);
      }

      return 0;
    }
  }

  void Initialize(Scene::Data& scene, const char* windowName, int windowX, int windowY)
  {
    winScene = &scene;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "Window";
    RegisterClass(&wc);

    int screenWidth, screenHeight;
    GetScreenSize(screenWidth, screenHeight);

    // Create the window
    hWnd = CreateWindow("Window",
                        windowName,
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                        (screenWidth - windowX) / 2,
                        (screenHeight - windowY) / 2,
                        windowX,
                        windowY,
                        NULL,
                        NULL,
                        GetModuleHandle(NULL),
                        NULL);

    if (hWnd == NULL)
    {
      MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR | MB_OK);
      return;
    }

    hdc = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR)};
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC temp_context = wglCreateContext(hdc);
    assert(temp_context != NULL && "Failed to create initial rendering context");

    wglMakeCurrent(hdc, temp_context);

    int wgl_version = gladLoaderLoadWGL(hdc);
    if (!wgl_version)
    {
      MessageBox(NULL, "Failed to initialize GLAD!", "Error", MB_ICONERROR | MB_OK);
      return;
    }
    printf("Loaded WGL %d.%d\n", GLAD_VERSION_MAJOR(wgl_version), GLAD_VERSION_MINOR(wgl_version));

    // Create an OpenGL 3.3 context using WGL extensions
    const int contextAttribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      3,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      3,
      WGL_CONTEXT_PROFILE_MASK_ARB,
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0 // End of attribute list
    };

    hglrc = wglCreateContextAttribsARB(hdc, NULL, contextAttribs);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp_context);
    wglMakeCurrent(hdc, hglrc);
    if (!hglrc)
    {
      MessageBox(NULL, "Failed to create OpenGL 3.3 context!", "Error", MB_ICONERROR | MB_OK);
      return;
    }

    int gl_version = gladLoaderLoadGL();
    if (!gl_version)
    {
      printf("Unable to load GL.\n");
      return;
    }
    printf("Loaded GL %d.%d\n", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));

    if (!GLAD_GL_VERSION_3_3)
    {
      MessageBox(NULL, "OpenGL 3.3 is not supported!", "Error", MB_ICONERROR | MB_OK);
      return;
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

#ifdef EDITOR
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplWin32_Init(hWnd);
    imguiInitialized = true;
#endif

    CenterWindow();

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    EventData.windowWidth = clientRect.right - clientRect.left;
    EventData.windowHeight = clientRect.bottom - clientRect.top;

    // Vertical Sync | Set to 1 to Enable | 0 to disable
    Event::RenderSetup(EventData);
    Camera::UpdateCameraAspect(scene,
                               (float)EventData.windowWidth,
                               (float)EventData.windowHeight);
    wglSwapIntervalEXT(0);
  }

  void Run(Scene::Data& scene)
  {
    MSG msg = {};

    // PeekMessage checks if there are any messages in the queue
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
      {
        // Exit the loop if a quit message is received
        Global::Quit();
        break;
      }

      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    Event::RenderRun(scene, EventData);
#ifdef EDITOR
    RenderImGui(scene, EventData);
#endif

    SwapBuffers(hdc);

    // TODO: Clean up resources
    // Event::RenderClean();
  }

  void Destroy()
  {
    gladLoaderUnloadGL();

#ifdef EDITOR
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
    DestroyWindow(hWnd);
  }
}