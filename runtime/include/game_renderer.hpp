#pragma once

#include <vector>

#include "game_window.hpp"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <Metal/shared_ptr.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace se {
using GameLibraryId = uint64_t;
using ShaderId = uint64_t;
using PipelineId = uint64_t;

class GameRenderer {
public:
    GameRenderer(GameWindow& window)
        : window_(window)
    {
        renderer_ = SDL_CreateRenderer(window_.window, nullptr, SDL_RENDERER_PRESENTVSYNC);
        if (!renderer_) {
            FATAL("Failed to create renderer");
        }

        swapchain_ = (CA::MetalLayer*)SDL_GetRenderMetalLayer(renderer_);
        device_ = swapchain_->device();
        command_queue_ = MTL::make_owned(device_->newCommandQueue());

        viewport_ = window_.getViewport();
    }

    ~GameRenderer()
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    GameLibraryId addLibrary(dispatch_data_s* library_data)
    {
        NS::Error* err { nullptr };

        MTL::shared_ptr<MTL::Library> library = MTL::make_owned(device_->newLibrary(library_data, &err));
        if (!library)
            FATAL("Failed to load library");

        libraryes_.push_back({ library });
        return libraryes_.size() - 1;
    }

    ShaderId loadShaderFromLibrary(GameLibraryId libraryId, const char* name)
    {
        NS::String* function_name = NS::String::string(name, NS::ASCIIStringEncoding);
        MTL::shared_ptr<MTL::Function> function = MTL::make_owned(libraryes_[libraryId].library->newFunction(function_name));

        shaders_.push_back({ function });
        return shaders_.size() - 1;
    }

    PipelineId createPipeline(ShaderId vertexShader, ShaderId fragmentShader)
    {
        NS::Error* err { nullptr };

        MTL::shared_ptr<MTL::RenderPipelineDescriptor> pipeline_descriptor = MTL::make_owned(MTL::RenderPipelineDescriptor::alloc()->init());
        pipeline_descriptor->setVertexFunction(shaders_[vertexShader].function.get());
        pipeline_descriptor->setFragmentFunction(shaders_[fragmentShader].function.get());

        MTL::RenderPipelineColorAttachmentDescriptor* color_attachment_descriptor = pipeline_descriptor->colorAttachments()->object(0);
        color_attachment_descriptor->setPixelFormat(swapchain_->pixelFormat());

        MTL::shared_ptr<MTL::RenderPipelineState> pipeline = MTL::make_owned(device_->newRenderPipelineState(pipeline_descriptor.get(), &err));

        if (!pipeline)
            FATAL("Failed to create pipeline");

        pipelines_.push_back({ pipeline_descriptor, pipeline });
        return pipelines_.size() - 1;
    }

    void beginFrame()
    {
        drawable_ = swapchain_->nextDrawable();

        render_pass_ = MTL::make_owned(MTL::RenderPassDescriptor::renderPassDescriptor());

        MTL::RenderPassColorAttachmentDescriptor* color_attachment = render_pass_->colorAttachments()->object(0);
        color_attachment->setLoadAction(MTL::LoadAction::LoadActionClear);
        color_attachment->setStoreAction(MTL::StoreAction::StoreActionStore);
        color_attachment->setTexture(drawable_->texture());

        command_buffer_ = MTL::make_owned(command_queue_->commandBuffer());

        encoder_ = MTL::make_owned(command_buffer_->renderCommandEncoder(render_pass_.get()));
        encoder_->setViewport(MTL::Viewport {
            0.0, 0.0,
            (double)viewport_[0], (double)viewport_[1],
            0.0, 1.0 });
    }

    void drawVertices(AAPLVertex* vertices, uint64_t length, PipelineId pipeline)
    {
        encoder_->setRenderPipelineState(pipelines_[pipeline].pipeline.get());

        encoder_->setVertexBytes(&vertices[0], sizeof(AAPLVertex) * length, AAPLVertexInputIndexVertices);
        encoder_->setVertexBytes(&viewport_, sizeof(viewport_), AAPLVertexInputIndexViewportSize);

        NS::UInteger vertex_start = 0, vertex_count = length;
        encoder_->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, vertex_start, vertex_count);
    }

    void endFrame()
    {
        encoder_->endEncoding();

        command_buffer_->presentDrawable(drawable_);
        command_buffer_->commit();

        drawable_->release();
    }

private:
    struct GameLibrary {
        MTL::shared_ptr<MTL::Library> library;
    };

    struct Shader {
        MTL::shared_ptr<MTL::Function> function;
    };

    struct Pipline {
        MTL::shared_ptr<MTL::RenderPipelineDescriptor> descriptor;
        MTL::shared_ptr<MTL::RenderPipelineState> pipeline;
    };

    GameWindow& window_;
    vector_uint2 viewport_;

    SDL_Renderer* renderer_ { nullptr };
    CA::MetalLayer* swapchain_ { nullptr };
    MTL::Device* device_ { nullptr };
    MTL::shared_ptr<MTL::CommandQueue> command_queue_;
    MTL::shared_ptr<MTL::CommandBuffer> command_buffer_;
    MTL::shared_ptr<MTL::RenderPassDescriptor> render_pass_;
    MTL::shared_ptr<MTL::RenderCommandEncoder> encoder_;
    CA::MetalDrawable* drawable_;

    std::vector<GameLibrary> libraryes_;
    std::vector<Shader> shaders_;
    std::vector<Pipline> pipelines_;
};
} // namespace se
