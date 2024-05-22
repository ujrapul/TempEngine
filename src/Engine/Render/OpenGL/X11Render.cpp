// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "X11Render.hpp"

#include "Logger.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Drawable.hpp"
#include "Engine.hpp"
#include "Event.hpp"
#include "Hoverable.hpp"
#include "Input.hpp"
#include "OpenGLWrapper.hpp"
#include "RenderUtils.hpp"
#include "gl.h"
#include "glx.h"
#include <X11/X.h>
#include <string>

#if defined(EDITOR) || defined(DEBUG)
#include "CommonRender.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_x11.hpp"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <unistd.h>

namespace Temp::Render
{
  namespace
  {
    enum
    {
      MIN_KEYCODE = 8
    };

    // X11 Display and Window variables
    Display* display{nullptr};
    Window window{};
    Event::Data EventData{};

    // GLX variables
    GLXContext context{};
    XVisualInfo* visualInfo{};
    Colormap colormap{};

    void GLAPIENTRY GLMessageCallback(__attribute__((unused)) GLenum source,
                           __attribute__((unused)) GLenum type,
                           __attribute__((unused)) GLuint id,
                           __attribute__((unused)) GLenum severity,
                           __attribute__((unused)) GLsizei length,
                           const GLchar* message,
                           __attribute__((unused)) const void* userParam)
    {
      if (/*type != 33360 &&*/ type != 33361)
      {
        auto final = String("[OpenGL Error](") + String::ToString(type) + ") " + message ;
        Logger::LogErr(final);
      }
    }

    void RenderThread(Scene::Data& scene)
    {
      Event::RenderRun(scene, EventData);
#if defined(EDITOR) || defined(DEBUG)
      RenderImGui(scene, EventData);
#endif

      glXSwapBuffers(display, window);

      // TODO: Clean up resources
      // Event::RenderClean();
    }

    void CreateDisplay(const char* windowName, int windowX, int windowY)
    {
      EventData.windowWidth = windowX;
      EventData.windowHeight = windowY;

      // Open the X11 display
      display = XOpenDisplay(nullptr);
      if (display == nullptr)
      {
        std::cerr << "Unable to open X11 display" << std::endl;
        return;
      }

      // Get the default screen and root window
      int screen = DefaultScreen(display);

      int glx_version = gladLoaderLoadGLX(display, screen);
      if (!glx_version)
      {
        printf("Unable to load GLX.\n");
        return;
      }
      printf("Loaded GLX %d.%d\n",
             GLAD_VERSION_MAJOR(glx_version),
             GLAD_VERSION_MINOR(glx_version));

      Window rootWindow = RootWindow(display, screen);

      // Create an OpenGL-capable visual
      int attribs[] = {GLX_RGBA,
                       GLX_DOUBLEBUFFER,
                       GLX_DEPTH_SIZE,
                       24,
                       GLX_RED_SIZE,
                       8,
                       GLX_GREEN_SIZE,
                       8,
                       GLX_BLUE_SIZE,
                       8,
                       None};

      visualInfo = glXChooseVisual(display, screen, attribs);
      if (visualInfo == nullptr)
      {
        std::cerr << "Unable to find a suitable visual" << std::endl;
        return;
      }

      // Create a colormap for the window
      XSetWindowAttributes windowAttribs;
      windowAttribs.colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
      windowAttribs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                                 KeyReleaseMask | PointerMotionMask | ButtonPressMask |
                                 ButtonReleaseMask;
      colormap = windowAttribs.colormap;

      // Create the window
      window = XCreateWindow(display,
                             rootWindow,
                             0,
                             0,
                             windowX,
                             windowY,
                             0,
                             visualInfo->depth,
                             InputOutput,
                             visualInfo->visual,
                             CWColormap | CWEventMask,
                             &windowAttribs);

      // Set the window title
      XStoreName(display, window, windowName);

      // Set the WM_DELETE_WINDOW protocol on the window
      Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
      XSetWMProtocols(display, window, &wmDeleteWindow, 1);

      // Create the OpenGL context
      context = glXCreateContext(display, visualInfo, nullptr, GL_TRUE);
      if (context == nullptr)
      {
        std::cerr << "Unable to create an OpenGL context" << std::endl;
        return;
      }

      glXMakeCurrent(display, window, context);

      // Map and show the window
      XMapWindow(display, window);
      XFlush(display);

      int gl_version = gladLoaderLoadGL();
      if (!gl_version)
      {
        printf("Unable to load GL.\n");
        return;
      }
      printf("Loaded GL %d.%d\n", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));

      if (!GLAD_GL_VERSION_3_3)
      {
        std::cerr << "OpenGL 3.3 is not supported" << std::endl;
        return;
      }
      GLenum error;
      while ((error = glGetError()) != GL_NO_ERROR)
      {
        Logger::LogErr(String("OpenGL Error: ") + String::ToString(error));
      }

#if defined(EDITOR) || defined(DEBUG)
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
      ImGui_ImplX11_Init(display, window);
#endif

      glDebugMessageControl(GL_DEBUG_SOURCE_API,
                            GL_DEBUG_TYPE_ERROR,
                            GL_DONT_CARE,
                            0,
                            NULL,
                            GL_TRUE);
      glEnable(GL_DEBUG_OUTPUT);
      glDebugMessageCallback(GLMessageCallback, 0);
    }
  }

  void Initialize(Scene::Data& scene, const char* windowName, int windowX, int windowY)
  {
    CreateDisplay(windowName, windowX, windowY);
    // Make the OpenGL context current for the rendering thread
    glXMakeCurrent(display, window, context);
    // Vertical Sync | Set to 1 to Enable | 0 to disable
    glXSwapIntervalEXT(display, window, 0);
    Event::RenderSetup(EventData);
    Camera::UpdateCameraAspect(scene, windowX, windowY);
  }

  void Run(Scene::Data& scene)
  {
    RenderThread(scene);

    while (XPending(display))
    {
      XEvent xev;
      XNextEvent(display, &xev);
#if defined(EDITOR) || defined(DEBUG)
      ImGui_ImplX11_EventHandler(xev);
      // if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
      // {
      //   continue;
      // }
#endif

      switch (xev.type)
      {
        case KeyPress:
        {
          auto key = static_cast<Input::KeyboardCode>(xev.xkey.keycode);
          Global::PushPressKeyQueue(key);
          Event::KeyPressed(EventData, key);
        }
        break;
        case KeyRelease:
        {
          auto key = static_cast<Input::KeyboardCode>(xev.xkey.keycode);
          Global::PushReleaseKeyQueue(key);
          Event::KeyReleased(EventData, key);
        }
        break;
        case ClientMessage:
        {
          // Window close check
          Atom wmProtocols = XInternAtom(display, "WM_PROTOCOLS", False);
          Atom wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
          if (xev.type == ClientMessage && xev.xclient.message_type == wmProtocols)
          {
            Atom actualAtom;
            int actualFormat;
            unsigned long numItems, bytesAfter;
            Atom* data = nullptr;
            if (XGetWindowProperty(display,
                                   xev.xclient.window,
                                   wmProtocols,
                                   0,
                                   1,
                                   False,
                                   AnyPropertyType,
                                   &actualAtom,
                                   &actualFormat,
                                   &numItems,
                                   &bytesAfter,
                                   (unsigned char**)&data) == Success)
            {
              if (numItems > 0 && data[0] == wmDeleteWindow)
              {
                Global::Quit();
              }
              XFree(data);
            }
          }
        }
        break;
        case ConfigureNotify:
        {
          XWindowAttributes windowAttributes;
          XGetWindowAttributes(display, window, &windowAttributes);

          int currentWidth = windowAttributes.width;
          int currentHeight = windowAttributes.height;

          auto& windowWidth = EventData.windowWidth;
          auto& windowHeight = EventData.windowHeight;

          if (currentWidth != windowWidth || currentHeight != windowHeight)
          {
            Camera::UpdateCameraAspect(scene, currentWidth, currentHeight);
            windowWidth = currentWidth;
            windowHeight = currentHeight;

            Scene::EnqueueRender(
              scene,
              [](Scene::Data& scene, void* data) {
                Event::Resize(scene, EventData, data);
              },
              nullptr);
          }
        }
        break;
        case MotionNotify:
        {
          XWindowAttributes windowAttributes;
          XGetWindowAttributes(display, window, &windowAttributes);

          int mouseX = xev.xbutton.x - windowAttributes.x;
          int mouseY = xev.xbutton.y - windowAttributes.y;

          Event::Hover(scene, EventData, mouseX, mouseY);
        }
        break;
        case ButtonPress:
        {
          XWindowAttributes windowAttributes;
          XGetWindowAttributes(display, window, &windowAttributes);

          int mouseX = xev.xbutton.x - windowAttributes.x;
          int mouseY = xev.xbutton.y - windowAttributes.y;
          int button = xev.xbutton.button;

          Temp::Event::ButtonPressed(scene, EventData, button, mouseX, mouseY);

          // std::cout << button << std::endl;
        }
        break;
        case ButtonRelease:
        {
          XWindowAttributes windowAttributes;
          XGetWindowAttributes(display, window, &windowAttributes);

          int mouseX = xev.xbutton.x - windowAttributes.x;
          int mouseY = xev.xbutton.y - windowAttributes.y;
          int button = xev.xbutton.button;

          Temp::Event::ButtonReleased(scene, EventData, mouseX, mouseY, button);
        }
        break;
        default:
          break;
      }
    }
  }

  void Destroy()
  {
    gladLoaderUnloadGL();
    gladLoaderUnloadGLX();

#if defined(EDITOR) || defined(DEBUG)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplX11_Shutdown();
    ImGui::DestroyContext();
#endif

    // Cleanup
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, context);
    XDestroyWindow(display, window);
    XFreeColormap(display, colormap);
    XCloseDisplay(display);
  }
}
