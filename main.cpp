#include "triangle_types.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <Metal/shared_ptr.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL.h>

#include <iostream>

namespace {

#include "triangle_metallib.h"

}

namespace {

const AAPLVertex triangleVertices[] = {
    // 2D positions,    RGBA colors
    { {  250,  -250 }, { 1, 0, 0, 1 } },
    { { -250,  -250 }, { 0, 1, 0, 1 } },
    { {    0,   250 }, { 0, 0, 1, 1 } },
};

const vector_uint2 viewport = {
    640, 480
};

}

int
main(int argc, char **argv) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("SDL Metal", -1, -1, viewport[0], viewport[1], SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    NS::Error *err;

    auto swapchain = (CA::MetalLayer*)SDL_RenderGetMetalLayer(renderer);
    auto device = swapchain->device();

    auto name = device->name();
    std::cerr << "device name: " << name->utf8String() << std::endl;

    auto library_data = dispatch_data_create(
        &triangle_metallib[0], triangle_metallib_len,
        dispatch_get_main_queue(),
        ^{ });

    auto library = MTL::make_owned(device->newLibrary(library_data, &err));

    if (!library) {
        std::cerr << "Failed to create library" << std::endl;
        std::exit(-1);
    }

    auto vertex_function_name = NS::String::string("vertexShader", NS::ASCIIStringEncoding);
    auto vertex_function = MTL::make_owned(library->newFunction(vertex_function_name));

    auto fragment_function_name = NS::String::string("fragmentShader", NS::ASCIIStringEncoding);
    auto fragment_function = MTL::make_owned(library->newFunction(fragment_function_name));

    auto pipeline_descriptor = MTL::make_owned(MTL::RenderPipelineDescriptor::alloc()->init());
    pipeline_descriptor->setVertexFunction(vertex_function.get());
    pipeline_descriptor->setFragmentFunction(fragment_function.get());

    auto color_attachment_descriptor = pipeline_descriptor->colorAttachments()->object(0);
    color_attachment_descriptor->setPixelFormat(swapchain->pixelFormat());

    auto pipeline = MTL::make_owned(device->newRenderPipelineState(pipeline_descriptor.get(), &err));

    if (!pipeline) {
        std::cerr << "Failed to create pipeline" << std::endl;
        std::exit(-1);
    }

    auto queue = MTL::make_owned(device->newCommandQueue());

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

        auto drawable = swapchain->nextDrawable();

        auto pass = MTL::make_owned(MTL::RenderPassDescriptor::renderPassDescriptor());

        auto color_attachment = pass->colorAttachments()->object(0);
        color_attachment->setLoadAction(MTL::LoadAction::LoadActionClear);
        color_attachment->setStoreAction(MTL::StoreAction::StoreActionStore);
        color_attachment->setTexture(drawable->texture());

        //
        auto buffer = MTL::make_owned(queue->commandBuffer());

        //
        auto encoder = MTL::make_owned(buffer->renderCommandEncoder(pass.get()));

        encoder->setViewport(MTL::Viewport {
            0.0f, 0.0f,
            (double)viewport[0], (double)viewport[1],
            0.0f, 1.0f
         });

        encoder->setRenderPipelineState(pipeline.get());

        encoder->setVertexBytes(&triangleVertices[0], sizeof(triangleVertices), AAPLVertexInputIndexVertices);
        encoder->setVertexBytes(&viewport, sizeof(viewport), AAPLVertexInputIndexViewportSize);

        NS::UInteger vertex_start = 0, vertex_count = 3;
        encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, vertex_start, vertex_count);

        encoder->endEncoding();

        buffer->presentDrawable(drawable);
        buffer->commit();

        drawable->release();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}