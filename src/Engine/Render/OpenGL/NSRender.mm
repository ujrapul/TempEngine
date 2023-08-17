// SPDX-FileCopyrightText: 2023 Ujwal Vujjini
// SPDX-License-Identifier: MIT

#include "NSRender.hpp"
#include "Camera.hpp"
#include "FontLoader.hpp"
#include "OpenGLWrapper.hpp"
#include "Scene.hpp"
#include "Event.hpp"
#include "Input.hpp"
#include "Editor.hpp"

#ifdef EDITOR
#include "CommonRender.hpp"
#include "imgui_impl_osx.h"
#include "imgui_impl_opengl3.h"
#endif

#import <Foundation/Foundation.h>
#import <OpenGL/gl3.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/CGLContext.h>
#import <Cocoa/Cocoa.h>
#include <thread>

using namespace Temp;

namespace
{
  NSOpenGLView* nsOpenGLView{nullptr};
  NSApplication *application{nullptr};
  NSWindow *window{nullptr};
  NSOpenGLContext *openGLContext{nullptr};
  Temp::Scene::Data *nsScene{nullptr};
  Temp::Event::Data EventData{};
  //
  //  void Resize(void *)
  //  {
  //    // TODO: Look into why glViewport here is causing the view to become larger
  //    // Might be something to do with the NSOpenGLView that's taking care of the
  //    // transformations for us.
  //    //    glViewport(windowWidth / 2.f, windowHeight / 2.f, windowWidth, windowHeight);
  //  }
}

@interface TempOpenGLView : NSOpenGLView
@end

@implementation TempOpenGLView

- (instancetype)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format {
  self = [super initWithFrame:frameRect pixelFormat:format];
  if (self) {
    @autoreleasepool {
      NSTrackingAreaOptions options = (NSTrackingMouseMoved | NSTrackingActiveInKeyWindow | NSTrackingActiveAlways | NSTrackingMouseEnteredAndExited);
      auto trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                       options:options
                                                         owner:self
                                                      userInfo:nil];
      [self addTrackingArea:trackingArea];
    }
  }
  return self;
}

- (void)keyDown:(NSEvent *)event {
  // Commented to remove 'ding'/'beep' sound when pressing key
  //#ifdef EDITOR
  //  [super keyDown:event];
  //#endif
}

- (void)mouseMoved:(NSEvent *)event {
  NSPoint mouseLocation = [event locationInWindow];
  NSPoint viewLocation = [nsOpenGLView convertPoint:mouseLocation fromView:nil];
  
  Math::Vec2f windowSize = Temp::Render::GetWindowSize();
  
  int mouseX = viewLocation.x;
  int mouseY = windowSize.y - viewLocation.y;
  
  if (mouseX < 0 || mouseY < 0 || mouseX > windowSize.x || mouseY > windowSize.y)
  {
    return;
  }
  
  Event::Hover(*nsScene, EventData, mouseX, mouseY);
  
  [super mouseMoved:event];
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event {
  return YES;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}

@end

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate

- (void)windowDidResize:(NSNotification *)notification {
  // Perform custom handling for resize event
  NSWindow *window = notification.object;
  [window makeFirstResponder:window.contentView];
  
  NSRect contentRect = [window contentRectForFrameRect:window.frame];
  NSSize contentSize = contentRect.size;
  EventData.windowWidth = contentSize.width;
  EventData.windowHeight = contentSize.height;
  
  Temp::Camera::UpdateCameraAspect(*nsScene, EventData.windowWidth, EventData.windowHeight);
}

- (void)windowDidEnterFullScreen:(NSNotification *)notification {
  EventData.isInFullScreen = true;
}

@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic) const char* windowName;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
  [application stop:nil];
}

- (void)run {
  // Set the OpenGL context for the window
  NSOpenGLPixelFormatAttribute attributes[] = {
    NSOpenGLPFAAccelerated,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
    NSOpenGLPFADepthSize, 24,
    0
  };
  
  // Set the activation policy to regular
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  
  // Activate the application
  [NSApp activateIgnoringOtherApps:YES];
  
  // Add observer for the window close notification
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowWillClose:) name:NSWindowWillCloseNotification object:nil];
  
  NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
  //  NSOpenGLContext *openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
  
  // Create the OpenGL view
  NSRect windowRect = NSMakeRect(0, 0, EventData.windowWidth, EventData.windowHeight);
  nsOpenGLView = [[TempOpenGLView alloc] initWithFrame:windowRect pixelFormat:pixelFormat];
  [nsOpenGLView becomeFirstResponder];
  
  // Create the window and set the content view
  window = [[NSWindow alloc] initWithContentRect:windowRect styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable backing:NSBackingStoreBuffered defer:NO];
  [window setTitle:[NSString stringWithUTF8String:self.windowName]];
  [window setContentView:nsOpenGLView];
  [window setLevel:NSFloatingWindowLevel];
  [window makeKeyAndOrderFront:nil];
  [window makeFirstResponder:nsOpenGLView];
  
  auto *windowDelegate = [[WindowDelegate alloc] init];
  window.delegate = windowDelegate;
  
  // Calculate the center coordinates of the screen
  NSScreen *screen = [NSScreen mainScreen];
  NSRect screenFrame = [screen visibleFrame];
  NSPoint centerPoint = NSMakePoint(NSMidX(screenFrame), NSMidY(screenFrame));
  
  // Calculate the origin coordinates for the window to be centered
  NSRect centeredFrame = NSMakeRect(centerPoint.x - windowRect.size.width / 2,
                                    centerPoint.y - windowRect.size.height / 2,
                                    windowRect.size.width,
                                    windowRect.size.height);
  
  // Set the window's frame to be centered on the screen
  [window setFrame:centeredFrame display:YES];
}

- (void)windowWillClose:(NSNotification *)notification {
  if (EventData.isInFullScreen) {
    EventData.isInFullScreen = false;
    return;
  }
  
  Temp::Global::Quit();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
  // Remove the observer
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
  return YES;
}

@end

void RunMainLoop(float deltaTime)
{
  Temp::Global::Process(deltaTime);
  @autoreleasepool {
    NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
    if (event) {
      switch([event type]) {
        case NSEventTypeKeyUp:
        case NSEventTypeKeyDown:
        case NSEventTypeFlagsChanged:
        {
          unsigned short keyCode = [event keyCode];
          auto key = static_cast<Input::KeyboardCode>(keyCode);
          
          switch([event type]) {
            case NSEventTypeKeyUp:
              Global::PushReleaseKeyQueue(key);
              Event::KeyReleased(EventData, key);
              break;
            case NSEventTypeKeyDown:
              Global::PushPressKeyQueue(key);
              Event::KeyPressed(EventData, key);
              break;
            case NSEventTypeFlagsChanged:
            {
              static NSEventModifierFlags prevModifiers;
              NSEventModifierFlags currModifiers = [event modifierFlags];
              
              if (prevModifiers != currModifiers)
              {
                if ((currModifiers & NSEventModifierFlagCommand) != 0
                    || (currModifiers & NSEventModifierFlagShift) != 0
                    || (currModifiers & NSEventModifierFlagOption) != 0
                    || (currModifiers & NSEventModifierFlagControl) != 0) {
                  Global::PushPressKeyQueue(key);
                  Event::KeyPressed(EventData, key);
                } else {
                  Global::PushReleaseKeyQueue(key);
                  Event::KeyReleased(EventData, key);
                }
              }
              
              prevModifiers = currModifiers;
            }
              break;
            default:
              break;
          }
        }
          break;
        case NSEventTypeLeftMouseDown:
        case NSEventTypeLeftMouseUp:
        case NSEventTypeMouseMoved:
        case NSEventTypeLeftMouseDragged:
        {
          NSPoint mouseLocation = [event locationInWindow];
          NSPoint viewLocation = [nsOpenGLView convertPoint:mouseLocation fromView:nil];
          
          Math::Vec2f windowSize = Temp::Render::GetWindowSize();
          
          int mouseX = viewLocation.x;
          int mouseY = windowSize.y - viewLocation.y;
          
          if (mouseX < 0 || mouseY < 0 || mouseX > windowSize.x || mouseY > windowSize.y)
          {
            break;
          }
          
          switch([event type]) {
            case NSEventTypeLeftMouseDown:
              Event::ButtonPressed(*nsScene, EventData, 1, mouseX, mouseY);
              break;
            case NSEventTypeLeftMouseUp:
              Event::ButtonReleased(*nsScene, EventData, mouseX, mouseY, 1);
              break;
              //            case NSEventTypeMouseMoved:
            case NSEventTypeLeftMouseDragged:
              Event::Hover(*nsScene, EventData, mouseX, mouseY);
              break;
            default:
              break;
          }
        }
          break;
        default:
          break;
      };
      [NSApp sendEvent:event];
    }
  }
}

void MacRunLoop(const char* windowName, int windowX, int windowY)
{
  @autoreleasepool {
    // Create the application instance
    application = [[NSApplication sharedApplication] autorelease];
    EventData.windowWidth = windowX;
    EventData.windowHeight = windowY;
    
    // Create the application delegate
    AppDelegate *appDelegate = [[[AppDelegate alloc] init] autorelease];
    appDelegate.windowName = windowName;
    [application setDelegate:appDelegate];
    
    //     Run the application
    if (![[NSRunningApplication currentApplication] isFinishedLaunching])
    {
      [appDelegate run];
    }
    
    Temp::Global::Start(windowName, EventData.windowWidth, EventData.windowHeight);
    
#ifdef EDITOR
    // Setup Dear ImGui context
    // FIXME: This example doesn't have proper cleanup...
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    
    // Setup Platform/Renderer backends
    ImGui_ImplOSX_Init(nsOpenGLView);
    ImGui_ImplOpenGL3_Init();
#endif
    
    // Disable system vertical sync
    // Vertical sync should only be handled at the application level
    // Otherwise it will slow the main loop as well
    GLint sync = 0;
    CGLContextObj ctx = CGLGetCurrentContext();
    
    CGLSetParameter(ctx, kCGLCPSwapInterval, &sync);
    
    while (Temp::Global::IsActive()) {
      static auto start = std::chrono::high_resolution_clock::now();
      auto stop = std::chrono::high_resolution_clock::now();
      float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count();
      start = stop;
      
      RunMainLoop(deltaTime);
    }
    
#ifdef EDITOR
    ImGui_ImplOSX_Shutdown();
#endif
    Temp::Global::Destroy();
  }
}

namespace Temp::Render
{
  void Initialize(Scene::Data& scene, const char */*windowName*/, int windowX, int windowY)
  {
    // Get the OpenGL context
    openGLContext = nsOpenGLView.openGLContext;
    // Make the OpenGL context current
    [openGLContext makeCurrentContext];
    
    Temp::Event::RenderSetup(EventData);
    Temp::Camera::UpdateCameraAspect(scene, windowX, windowY);
  }
  
  void Run(Scene::Data& scene, const char *windowName, int windowX, int windowY)
  {
    nsScene = &scene;
    MacRunLoop(windowName, windowX, windowY);
  }
  
  // Process Events
  void Run(Scene::Data& scene)
  {
    Temp::Event::RenderRun(scene, EventData);
#ifdef EDITOR
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplOSX_NewFrame(nsOpenGLView);
    
    ImGui::NewFrame();
    
    Temp::Editor::RunImGui(scene, EventData);
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    // Swap buffers
    [openGLContext flushBuffer];
    
    // TODO: Clean up resources
    // Temp::Event::RenderClean();
  }
  
  void Destroy()
  {
  }
  
  Math::Vec2f GetWindowOrigin()
  {
    __block Math::Vec2f origin;
    NSRect windowFrame = [window convertRectToScreen:[window frame]];
    CGFloat windowX = NSMinX(windowFrame);
    CGFloat windowY = NSMinY(windowFrame);
    origin = {(float)windowX, (float)windowY};
    return origin;
  }
  
  Math::Vec2f GetWindowSize()
  {
    __block Math::Vec2f size;
    NSRect contentRect = [window contentRectForFrameRect:window.frame];
    NSSize contentSize = contentRect.size;
    CGFloat width = contentSize.width;
    CGFloat height = contentSize.height;
    size = {(float)width, (float)height};
    return size;
  }
  
  Math::Vec2f GetScreenSize()
  {
    __block Math::Vec2f size;
    NSSize screenSize = [[NSScreen mainScreen] frame].size;
    CGFloat screenWidth = screenSize.width;
    CGFloat screenHeight = screenSize.height;
    size = {(float)screenWidth, (float)screenHeight};
    return size;
  }
}
