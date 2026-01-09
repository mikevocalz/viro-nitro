#import "MetalDevice.h"

#ifdef __APPLE__

#import <simd/simd.h>

namespace kimoyooju {

// ============================================================================
// MetalTexture Implementation
// ============================================================================

MetalTexture::MetalTexture(id<MTLTexture> texture, uint32_t width, uint32_t height, TextureFormat format)
    : texture_(texture), width_(width), height_(height), format_(format) {
    [texture_ retain];
}

MetalTexture::~MetalTexture() {
    if (texture_) {
        [texture_ release];
        texture_ = nil;
    }
}

void MetalTexture::bind(uint32_t unit) {
    // Metal doesn't have explicit texture binding like OpenGL
    // Textures are set directly on the encoder when drawing
}

void MetalTexture::upload(const void* data, uint32_t width, uint32_t height, TextureFormat format) {
    if (!texture_ || !data) return;
    
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    NSUInteger bytesPerRow = width * 4; // Assuming RGBA8
    
    [texture_ replaceRegion:region
                mipmapLevel:0
                  withBytes:data
                bytesPerRow:bytesPerRow];
    
    width_ = width;
    height_ = height;
    format_ = format;
}

void MetalTexture::generateMipmaps() {
    // Mipmaps are generated via command encoder in Metal
    // This requires a blit command encoder
}

// ============================================================================
// MetalBuffer Implementation
// ============================================================================

MetalBuffer::MetalBuffer(id<MTLBuffer> buffer, size_t size, BufferType type)
    : buffer_(buffer), size_(size), type_(type) {
    [buffer_ retain];
}

MetalBuffer::~MetalBuffer() {
    if (buffer_) {
        [buffer_ release];
        buffer_ = nil;
    }
}

void MetalBuffer::upload(const void* data, size_t size, size_t offset) {
    if (!buffer_ || !data) return;
    
    void* contents = [buffer_ contents];
    if (contents) {
        memcpy(static_cast<uint8_t*>(contents) + offset, data, size);
    }
}

void* MetalBuffer::map() {
    if (!buffer_) return nullptr;
    return [buffer_ contents];
}

void MetalBuffer::unmap() {
    // Metal shared buffers don't need explicit unmapping
    // For managed buffers, we would call didModifyRange
}

// ============================================================================
// MetalShader Implementation
// ============================================================================

MetalShader::MetalShader(id<MTLRenderPipelineState> pipeline, const std::string& name)
    : pipeline_(pipeline), name_(name) {
    [pipeline_ retain];
}

MetalShader::~MetalShader() {
    if (pipeline_) {
        [pipeline_ release];
        pipeline_ = nil;
    }
}

void MetalShader::bind() {
    // Pipeline state is set on the encoder, not here
}

void MetalShader::setUniform(const std::string& name, float value) {
    // Uniforms in Metal are set via buffer data
}

void MetalShader::setUniform(const std::string& name, const float* matrix4x4) {
    // Uniforms in Metal are set via buffer data
}

void MetalShader::setUniform(const std::string& name, const float* vec, uint32_t count) {
    // Uniforms in Metal are set via buffer data
}

// ============================================================================
// MetalFramebuffer Implementation
// ============================================================================

MetalFramebuffer::MetalFramebuffer(uint32_t width, uint32_t height)
    : width_(width), height_(height) {
    renderPassDescriptor_ = [MTLRenderPassDescriptor new];
}

MetalFramebuffer::~MetalFramebuffer() {
    if (renderPassDescriptor_) {
        [renderPassDescriptor_ release];
        renderPassDescriptor_ = nil;
    }
}

void MetalFramebuffer::bind() {
    // Framebuffer is bound when creating render command encoder
}

void MetalFramebuffer::unbind() {
    // Nothing to do
}

void MetalFramebuffer::resize(uint32_t width, uint32_t height) {
    width_ = width;
    height_ = height;
}

void MetalFramebuffer::attachColor(std::shared_ptr<Texture> texture, uint32_t index) {
    auto metalTex = std::dynamic_pointer_cast<MetalTexture>(texture);
    if (metalTex && renderPassDescriptor_) {
        renderPassDescriptor_.colorAttachments[index].texture = metalTex->getTexture();
        renderPassDescriptor_.colorAttachments[index].loadAction = MTLLoadActionClear;
        renderPassDescriptor_.colorAttachments[index].storeAction = MTLStoreActionStore;
        
        if (index >= colorAttachments_.size()) {
            colorAttachments_.resize(index + 1);
        }
        colorAttachments_[index] = texture;
    }
}

void MetalFramebuffer::attachDepth(std::shared_ptr<Texture> texture) {
    auto metalTex = std::dynamic_pointer_cast<MetalTexture>(texture);
    if (metalTex && renderPassDescriptor_) {
        renderPassDescriptor_.depthAttachment.texture = metalTex->getTexture();
        renderPassDescriptor_.depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor_.depthAttachment.storeAction = MTLStoreActionDontCare;
        renderPassDescriptor_.depthAttachment.clearDepth = 1.0;
        depthAttachment_ = texture;
    }
}

// ============================================================================
// MetalDevice Implementation
// ============================================================================

MetalDevice::MetalDevice()
    : device_(nil)
    , commandQueue_(nil)
    , metalLayer_(nil)
    , currentCommandBuffer_(nil)
    , currentEncoder_(nil)
    , currentDrawable_(nil)
    , depthStencilState_(nil)
    , depthTexture_(nil)
    , initialized_(false)
    , viewportWidth_(0)
    , viewportHeight_(0)
    , currentVertexStride_(0) {
    
    clearColor_ = MTLClearColorMake(0.1, 0.1, 0.15, 1.0);
    viewport_ = {0, 0, 0, 0, 0, 1};
}

MetalDevice::~MetalDevice() {
    shutdown();
}

bool MetalDevice::initialize(void* layer) {
    if (initialized_) {
        return true;
    }
    
    @autoreleasepool {
        // Get or create Metal device
        device_ = MTLCreateSystemDefaultDevice();
        if (!device_) {
            NSLog(@"Failed to create Metal device");
            return false;
        }
        [device_ retain];
        
        // Create command queue
        commandQueue_ = [device_ newCommandQueue];
        if (!commandQueue_) {
            NSLog(@"Failed to create command queue");
            return false;
        }
        
        // Configure metal layer
        metalLayer_ = (__bridge CAMetalLayer*)layer;
        metalLayer_.device = device_;
        metalLayer_.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer_.framebufferOnly = YES;
        
        // Create depth stencil state
        MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthDesc.depthCompareFunction = MTLCompareFunctionLessEqual;
        depthDesc.depthWriteEnabled = YES;
        depthStencilState_ = [device_ newDepthStencilStateWithDescriptor:depthDesc];
        [depthDesc release];
        
        initialized_ = true;
        NSLog(@"Metal device initialized: %@", device_.name);
        
        return true;
    }
}

void MetalDevice::shutdown() {
    if (!initialized_) {
        return;
    }
    
    @autoreleasepool {
        if (depthTexture_) {
            [depthTexture_ release];
            depthTexture_ = nil;
        }
        
        if (depthStencilState_) {
            [depthStencilState_ release];
            depthStencilState_ = nil;
        }
        
        if (commandQueue_) {
            [commandQueue_ release];
            commandQueue_ = nil;
        }
        
        if (device_) {
            [device_ release];
            device_ = nil;
        }
        
        metalLayer_ = nil;
        initialized_ = false;
    }
}

std::shared_ptr<Texture> MetalDevice::createTexture(uint32_t width, uint32_t height, TextureFormat format) {
    if (!device_) return nullptr;
    
    @autoreleasepool {
        MTLTextureDescriptor* desc = [[MTLTextureDescriptor alloc] init];
        desc.width = width;
        desc.height = height;
        desc.mipmapLevelCount = 1;
        desc.storageMode = MTLStorageModeShared;
        desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
        
        switch (format) {
            case TextureFormat::R8:
                desc.pixelFormat = MTLPixelFormatR8Unorm;
                break;
            case TextureFormat::RG8:
                desc.pixelFormat = MTLPixelFormatRG8Unorm;
                break;
            case TextureFormat::RGBA8:
                desc.pixelFormat = MTLPixelFormatRGBA8Unorm;
                break;
            case TextureFormat::RGB8:
                desc.pixelFormat = MTLPixelFormatRGBA8Unorm; // Metal doesn't have RGB8
                break;
            case TextureFormat::DEPTH24:
            case TextureFormat::DEPTH32F:
                desc.pixelFormat = MTLPixelFormatDepth32Float;
                desc.storageMode = MTLStorageModePrivate;
                desc.usage = MTLTextureUsageRenderTarget;
                break;
        }
        
        id<MTLTexture> texture = [device_ newTextureWithDescriptor:desc];
        [desc release];
        
        if (!texture) return nullptr;
        
        auto result = std::make_shared<MetalTexture>(texture, width, height, format);
        [texture release]; // MetalTexture retains it
        
        return result;
    }
}

std::shared_ptr<Buffer> MetalDevice::createBuffer(size_t size, BufferType type) {
    if (!device_) return nullptr;
    
    @autoreleasepool {
        id<MTLBuffer> buffer = [device_ newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];
        if (!buffer) return nullptr;
        
        auto result = std::make_shared<MetalBuffer>(buffer, size, type);
        [buffer release]; // MetalBuffer retains it
        
        return result;
    }
}

id<MTLLibrary> MetalDevice::compileShaderLibrary(const std::string& source) {
    if (!device_) return nil;
    
    @autoreleasepool {
        NSString* sourceStr = [NSString stringWithUTF8String:source.c_str()];
        NSError* error = nil;
        
        MTLCompileOptions* options = [[MTLCompileOptions alloc] init];
        options.languageVersion = MTLLanguageVersion2_4;
        
        id<MTLLibrary> library = [device_ newLibraryWithSource:sourceStr
                                                       options:options
                                                         error:&error];
        [options release];
        
        if (error) {
            NSLog(@"Shader compilation error: %@", error.localizedDescription);
            return nil;
        }
        
        return library;
    }
}

std::shared_ptr<Shader> MetalDevice::createShaderFromFunctions(
    id<MTLFunction> vertexFunc,
    id<MTLFunction> fragmentFunc,
    const std::string& name) {
    
    if (!device_ || !vertexFunc || !fragmentFunc) return nullptr;
    
    @autoreleasepool {
        MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.vertexFunction = vertexFunc;
        pipelineDesc.fragmentFunction = fragmentFunc;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.colorAttachments[0].blendingEnabled = YES;
        pipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        pipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        
        // Vertex descriptor
        MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
        
        // Position (float3)
        vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
        vertexDesc.attributes[0].offset = 0;
        vertexDesc.attributes[0].bufferIndex = 0;
        
        // Normal (float3)
        vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
        vertexDesc.attributes[1].offset = 12;
        vertexDesc.attributes[1].bufferIndex = 0;
        
        // UV (float2)
        vertexDesc.attributes[2].format = MTLVertexFormatFloat2;
        vertexDesc.attributes[2].offset = 24;
        vertexDesc.attributes[2].bufferIndex = 0;
        
        // Layout (stride = 32 bytes)
        vertexDesc.layouts[0].stride = 32;
        vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        
        pipelineDesc.vertexDescriptor = vertexDesc;
        [vertexDesc release];
        
        NSError* error = nil;
        id<MTLRenderPipelineState> pipeline = [device_ newRenderPipelineStateWithDescriptor:pipelineDesc
                                                                                      error:&error];
        [pipelineDesc release];
        
        if (error) {
            NSLog(@"Pipeline creation error: %@", error.localizedDescription);
            return nullptr;
        }
        
        auto result = std::make_shared<MetalShader>(pipeline, name);
        [pipeline release];
        
        return result;
    }
}

std::shared_ptr<Shader> MetalDevice::createShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    // For Metal, we expect Metal Shading Language source
    // This combines vertex and fragment into a single library
    std::string combinedSource = vertexSrc + "\n" + fragmentSrc;
    
    @autoreleasepool {
        id<MTLLibrary> library = compileShaderLibrary(combinedSource);
        if (!library) return nullptr;
        
        id<MTLFunction> vertexFunc = [library newFunctionWithName:@"vertexMain"];
        id<MTLFunction> fragmentFunc = [library newFunctionWithName:@"fragmentMain"];
        
        [library release];
        
        if (!vertexFunc || !fragmentFunc) {
            if (vertexFunc) [vertexFunc release];
            if (fragmentFunc) [fragmentFunc release];
            return nullptr;
        }
        
        auto result = createShaderFromFunctions(vertexFunc, fragmentFunc, "shader");
        
        [vertexFunc release];
        [fragmentFunc release];
        
        return result;
    }
}

std::shared_ptr<Framebuffer> MetalDevice::createFramebuffer(uint32_t width, uint32_t height) {
    return std::make_shared<MetalFramebuffer>(width, height);
}

void MetalDevice::beginFrame() {
    if (!metalLayer_ || !commandQueue_) return;
    
    @autoreleasepool {
        currentDrawable_ = [metalLayer_ nextDrawable];
        if (!currentDrawable_) return;
        
        currentCommandBuffer_ = [commandQueue_ commandBuffer];
        
        // Create render pass descriptor for main framebuffer
        MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
        passDesc.colorAttachments[0].texture = currentDrawable_.texture;
        passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
        passDesc.colorAttachments[0].storeAction = MTLStoreActionStore;
        passDesc.colorAttachments[0].clearColor = clearColor_;
        
        // Ensure depth texture exists and matches size
        CGSize drawableSize = metalLayer_.drawableSize;
        if (!depthTexture_ || 
            depthTexture_.width != (NSUInteger)drawableSize.width ||
            depthTexture_.height != (NSUInteger)drawableSize.height) {
            
            if (depthTexture_) {
                [depthTexture_ release];
            }
            
            MTLTextureDescriptor* depthDesc = [MTLTextureDescriptor 
                texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                             width:(NSUInteger)drawableSize.width
                                            height:(NSUInteger)drawableSize.height
                                         mipmapped:NO];
            depthDesc.storageMode = MTLStorageModePrivate;
            depthDesc.usage = MTLTextureUsageRenderTarget;
            
            depthTexture_ = [device_ newTextureWithDescriptor:depthDesc];
        }
        
        passDesc.depthAttachment.texture = depthTexture_;
        passDesc.depthAttachment.loadAction = MTLLoadActionClear;
        passDesc.depthAttachment.storeAction = MTLStoreActionDontCare;
        passDesc.depthAttachment.clearDepth = 1.0;
        
        currentEncoder_ = [currentCommandBuffer_ renderCommandEncoderWithDescriptor:passDesc];
        [currentEncoder_ setDepthStencilState:depthStencilState_];
        [currentEncoder_ setFrontFacingWinding:MTLWindingCounterClockwise];
        [currentEncoder_ setCullMode:MTLCullModeBack];
    }
}

void MetalDevice::endFrame() {
    if (currentEncoder_) {
        [currentEncoder_ endEncoding];
        currentEncoder_ = nil;
    }
}

void MetalDevice::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    viewport_.originX = x;
    viewport_.originY = y;
    viewport_.width = width;
    viewport_.height = height;
    viewport_.znear = 0.0;
    viewport_.zfar = 1.0;
    
    viewportWidth_ = width;
    viewportHeight_ = height;
    
    if (currentEncoder_) {
        [currentEncoder_ setViewport:viewport_];
    }
}

void MetalDevice::setClearColor(float r, float g, float b, float a) {
    clearColor_ = MTLClearColorMake(r, g, b, a);
}

void MetalDevice::clear(bool color, bool depth, bool stencil) {
    // In Metal, clearing happens at render pass start via load action
    // Already configured in beginFrame
}

void MetalDevice::setVertexBuffer(std::shared_ptr<Buffer> buffer, uint32_t stride) {
    currentVertexBuffer_ = std::dynamic_pointer_cast<MetalBuffer>(buffer);
    currentVertexStride_ = stride;
    
    if (currentEncoder_ && currentVertexBuffer_) {
        [currentEncoder_ setVertexBuffer:currentVertexBuffer_->getBuffer()
                                  offset:0
                                 atIndex:0];
    }
}

void MetalDevice::setIndexBuffer(std::shared_ptr<Buffer> buffer) {
    currentIndexBuffer_ = std::dynamic_pointer_cast<MetalBuffer>(buffer);
}

void MetalDevice::drawArrays(uint32_t vertexCount, uint32_t firstVertex) {
    if (!currentEncoder_) return;
    
    [currentEncoder_ drawPrimitives:MTLPrimitiveTypeTriangle
                        vertexStart:firstVertex
                        vertexCount:vertexCount];
}

void MetalDevice::drawIndexed(uint32_t indexCount, uint32_t firstIndex) {
    if (!currentEncoder_ || !currentIndexBuffer_) return;
    
    [currentEncoder_ drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                indexCount:indexCount
                                 indexType:MTLIndexTypeUInt32
                               indexBuffer:currentIndexBuffer_->getBuffer()
                         indexBufferOffset:firstIndex * sizeof(uint32_t)];
}

bool MetalDevice::swapBuffers() {
    if (!currentCommandBuffer_ || !currentDrawable_) return false;
    
    @autoreleasepool {
        [currentCommandBuffer_ presentDrawable:currentDrawable_];
        [currentCommandBuffer_ commit];
        
        currentCommandBuffer_ = nil;
        currentDrawable_ = nil;
        
        return true;
    }
}

} // namespace kimoyooju

#endif // __APPLE__
