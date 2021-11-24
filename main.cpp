#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL.h>

#include <iostream>

int
main(int argc, char **argv) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("SDL Metal", -1, -1, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    auto swapchain = (CA::MetalLayer*)SDL_RenderGetMetalLayer(renderer);
    auto device = swapchain->device();

    auto name = device->name();
    std::cerr << "device name: " << name->utf8String() << std::endl;

    auto queue = device->newCommandQueue();

    auto color = MTL::ClearColor::Make(0, 0, 0, 1);

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
            }
        }

        color.red = (color.red > 1.0) ? 0 : color.red + 0.01;

        auto drawable = swapchain->nextDrawable();

        auto pass = MTL::RenderPassDescriptor::renderPassDescriptor();

        auto color_attachment = pass->colorAttachments()->object(0);
        color_attachment->setClearColor(color);
        color_attachment->setLoadAction(MTL::LoadAction::LoadActionClear);
        color_attachment->setStoreAction(MTL::StoreAction::StoreActionStore);
        color_attachment->setTexture(drawable->texture());

        auto buffer = queue->commandBuffer();

        auto encoder = buffer->renderCommandEncoder(pass);
        encoder->endEncoding();
        encoder->release();

        buffer->presentDrawable(drawable);
        buffer->commit();
        buffer->release();

        pass->release();
    }

    queue->release();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}