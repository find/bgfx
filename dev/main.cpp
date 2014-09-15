#include "helpers/common.h"
#include "inc/bgfx.h"

#include "helpers/imgui/imgui.h"
#include <stdio.h>
#include <assert.h>

void* loadfile(char const* filename)
{
    FILE* _file = fopen(filename, "rb");
    assert(_file);
	long int pos = ftell(_file);
	fseek(_file, 0L, SEEK_END);
	long int size = ftell(_file);
	fseek(_file, pos, SEEK_SET);

	void* data = malloc(size);
    assert(fread(data, 1, size, _file) == size);
    fclose(_file);

    return data;
}

int _main_(int, char**)
{
    uint32_t width = 800, height = 600;
    uint32_t debug = BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS;
    uint32_t reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X8;

    bgfx::init();
    bgfx::reset(width, height, reset);

    bgfx::setDebug(debug);

    bgfx::setViewClear(0,
            BGFX_CLEAR_COLOR_BIT | BGFX_CLEAR_DEPTH_BIT,
            0x111111ff, 1.0f, 0);

    void* fontdata = loadfile("assets/font/roboto-regular.ttf");
    imguiCreate(fontdata);
    free(fontdata);

    entry::MouseState mouseState;
    while(!entry::processEvents(width, height, debug, reset, &mouseState)) {
        bgfx::setViewRect(0, 0, 0, width, height);
        
        bgfx::submit(0);
        bgfx::dbgTextPrintf(0, 1, 0x4f, "hello world");
        bgfx::dbgTextPrintf(0, 2, 0x5f, "simple bgfx program");

        imguiBeginFrame(mouseState.m_mx, mouseState.m_my,
              (mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT  : 0)
			| (mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT : 0)
			, 0
			, width
			, height);
        int32_t scrollArea = 0;
        imguiBeginScrollArea("Test", width-210, 100, 200, 400, &scrollArea);
        imguiSeparatorLine();
        imguiLabel("foobar");
        imguiEndScrollArea();
        imguiEndFrame();

        bgfx::frame();
    }

    imguiDestroy();
    bgfx::shutdown();
    return 0;
}
