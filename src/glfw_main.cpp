#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#include <chrono>
#include "chip8.h"

#define internal static
#define global_variable static
#define local_persist static

struct glfw_window_dimension
{
    int Width;
    int Height;
};

internal void ResetRom();
internal unsigned long long GetTimeNanos();
global_variable long DeltaTime = 1E9 / 400;
global_variable long CurrentTime = GetTimeNanos();
global_variable long NewTime = 0;
global_variable double FrameTime = 0;

global_variable int DISPLAY_MODIFIER = 10;
global_variable chip8 Processor;
global_variable const char *LoadedRom;

internal unsigned long long
GetTimeNanos()
{
    using namespace std::chrono;
    return (duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())).count();
}

internal glfw_window_dimension
GLFWGetWindowDimension(GLFWwindow *Window)
{
    glfw_window_dimension Dimension;
    glfwGetFramebufferSize(Window, &Dimension.Width, &Dimension.Height);
    return Dimension;
}

internal void
GLFWClearWindow(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

internal void
GLFWUpdateWindow(GLFWwindow *Window)
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
        printf("OpenGL Error: %d\n", error);

    glfwPollEvents();
    glfwSwapBuffers(Window);
}

internal void
GLFWKeyCallback(GLFWwindow *Window, int key, int scancode, int action, int mods)
{
    switch(key)
    {
        case GLFW_KEY_1: { Processor.Key[0x1] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_2: { Processor.Key[0x2] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_3: { Processor.Key[0x3] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_4: { Processor.Key[0xC] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }

        case GLFW_KEY_Q: { Processor.Key[0x4] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_W: { Processor.Key[0x5] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_E: { Processor.Key[0x6] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_R: { Processor.Key[0xD] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }

        case GLFW_KEY_A: { Processor.Key[0x7] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_S: { Processor.Key[0x8] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_D: { Processor.Key[0x9] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_F: { Processor.Key[0xE] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }

        case GLFW_KEY_Z: { Processor.Key[0xA] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_X: { Processor.Key[0x0] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_C: { Processor.Key[0xB] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }
        case GLFW_KEY_V: { Processor.Key[0xF] = (action == GLFW_PRESS || action == GLFW_REPEAT); break; }

        case GLFW_KEY_P: { if(action == GLFW_PRESS) Processor.Paused = !Processor.Paused; break; }
        case GLFW_KEY_ENTER: { ResetRom(); break; }
        case GLFW_KEY_ESCAPE: { exit(0); break; }
    }
}

internal void
GLFWWindowSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
}

internal void
Fatal(const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    vprintf(Format, Args);
    va_end(Args);
    exit(1);
}

internal GLFWwindow*
GLFWCreateWindow(int Width, int Height, const char *Title)
{
    if (!glfwInit())
        Fatal("Could not initialize GLFW!\n");

    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    GLFWwindow *Window = glfwCreateWindow(Width, Height, Title, NULL, NULL);
    if(!Window)
        Fatal("Failed to create window!\n");

    glfwSetFramebufferSizeCallback(Window, GLFWWindowSizeCallback);
    glfwSetKeyCallback(Window, GLFWKeyCallback);

    glfwMakeContextCurrent(Window);
    glfwSwapInterval(0);

    printf("OpenGL %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
        Fatal("Could not initialize GLEW!\n");

    return Window;
}

internal inline void
DrawGraphics()
{
    for(int Y = 0; Y < DISPLAY_HEIGHT; ++Y)
    {
        for(int X = 0; X < DISPLAY_WIDTH; ++X)
        {
            if(Processor.Graphics[(Y * DISPLAY_WIDTH) + X] == 0)
                glColor3f(0.0f ,0.0f ,0.0f);
            else
                glColor3f(1.0f, 1.0f, 1.0f);

            glBegin(GL_QUADS);
            glVertex3f((X * DISPLAY_MODIFIER), (Y * DISPLAY_MODIFIER), 0.0f);
            glVertex3f((X * DISPLAY_MODIFIER), (Y * DISPLAY_MODIFIER) + DISPLAY_MODIFIER, 0.0f);
            glVertex3f((X * DISPLAY_MODIFIER) + DISPLAY_MODIFIER, (Y * DISPLAY_MODIFIER) + DISPLAY_MODIFIER, 0.0f);
            glVertex3f((X * DISPLAY_MODIFIER) + DISPLAY_MODIFIER, (Y * DISPLAY_MODIFIER) + 0.0f, 0.0f);
            glEnd();
        }
    }
}

internal void
ResetRom()
{
    Chip8Initialize(&Processor);
    if(!Chip8LoadRom(&Processor, LoadedRom))
        Fatal("Failed to load rom: %s\n", LoadedRom);
}

int main(int argc, char **argv)
{
    if(argc != 2)
        Fatal("Usage: chip8 /path/to/rom\n");

    LoadedRom = argv[1];
    Chip8Initialize(&Processor);
    if(!Chip8LoadRom(&Processor, LoadedRom))
        Fatal("Failed to load rom: %s\n", LoadedRom);

    glfw_window_dimension Dimension = { DISPLAY_WIDTH * DISPLAY_MODIFIER,
                                        DISPLAY_HEIGHT * DISPLAY_MODIFIER };
    GLFWwindow *Window = GLFWCreateWindow(Dimension.Width, Dimension.Height, "Chip-8 Emulator");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, Dimension.Width, Dimension.Height, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, Dimension.Width, Dimension.Height);

    while(!glfwWindowShouldClose(Window))
    {
        NewTime = GetTimeNanos();
        FrameTime += NewTime - CurrentTime;
        CurrentTime = NewTime;

        while(FrameTime >= DeltaTime)
        {
            if(!Processor.Paused)
            {
                if(Processor.DelayTimer == 0)
                    Chip8DoCycle(&Processor);

                if(Processor.DelayTimer > 0)
                    --Processor.DelayTimer;

                if(Processor.SoundTimer > 0)
                {
                    if(Processor.SoundTimer == 1)
                        printf("Make buzzer sound!\n");

                    --Processor.SoundTimer;
                }

                if(Processor.Draw)
                {
                    GLFWClearWindow();
                    DrawGraphics();
                    Processor.Draw = false;
                }
            }

            GLFWUpdateWindow(Window);
            FrameTime -= DeltaTime;
        }
    }

    glfwTerminate();
    return 0;
}
