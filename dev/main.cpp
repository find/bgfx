#include "helpers/common.h"
#include "inc/bgfx.h"

#include "helpers/imgui/imgui.h"
#include <stdio.h>
#include <assert.h>

int _main_(int, char**)
{
    uint32_t width = 800, height = 600;
    uint32_t debug = BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS;
    uint32_t reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X8;

    bgfx::init();
    bgfx::reset(width, height, reset);

    bgfx::setDebug(debug);

    imguiCreate();

    entry::MouseState mouseState;
    float rgb[3] = {0.3f, 0.3f, 0.3f};
    bool colorwheelActivated = true;
    int32_t scrollArea = 0;
    while(!entry::processEvents(width, height, debug, reset, &mouseState)) {
        auto encodeColor = [&rgb]()->uint32_t{
            return uint32_t(rgb[0]*255)<<24|uint32_t(rgb[1]*255)<<16|uint32_t(rgb[2]*255)<<8|0xff;
        };
        bgfx::setViewClear(0,
                BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                encodeColor(), 1.0f, 0);

        bgfx::setViewRect(0, 0, 0, width, height);
        
        bgfx::submit(0, BGFX_INVALID_HANDLE);
        bgfx::dbgTextPrintf(0, 1, 0x4f, "hello world");
        bgfx::dbgTextPrintf(0, 2, 0x5f, "simple bgfx program");

        imguiBeginFrame(mouseState.m_mx, mouseState.m_my,
              (mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT  : 0)
            | (mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT : 0)
            , 0
            , width
            , height);
        imguiBeginScrollArea("Test", width-350, 50, 330, 400, &scrollArea);
        imguiSeparatorLine();
        imguiLabel("foobar");
        if(imguiButton("say hi"))
            fprintf(stdout, "hi there\n");
        imguiColorWheel("bg color", rgb, colorwheelActivated);
        imguiSeparatorLine();
        if(imguiButton("quit"))
            break;
        imguiEndScrollArea();
        imguiEndFrame();

        bgfx::frame();
    }

    imguiDestroy();
    bgfx::shutdown();
    return 0;
}
