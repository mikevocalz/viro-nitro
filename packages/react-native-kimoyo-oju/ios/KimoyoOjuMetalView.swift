import UIKit
import MetalKit
import ARKit

/// Custom Metal view for Kimoyo Oju rendering on iOS
public class KimoyoOjuMetalView: MTKView, MTKViewDelegate, ARSessionDelegate {
    
    // MARK: - Properties
    
    private var engine: UnsafeMutableRawPointer?
    private var arSession: ARSession?
    private var isAREnabled: Bool = false
    
    public var onSurfaceCreated: ((Int, Int) -> Void)?
    public var onSurfaceChanged: ((Int, Int) -> Void)?
    public var onSurfaceDestroyed: (() -> Void)?
    public var onARFrame: ((ARFrame) -> Void)?
    public var onTouchEvent: ((CGPoint, Int) -> Bool)?
    
    // MARK: - Initialization
    
    public override init(frame: CGRect, device: MTLDevice?) {
        super.init(frame: frame, device: device ?? MTLCreateSystemDefaultDevice())
        setup()
    }
    
    public required init(coder: NSCoder) {
        super.init(coder: coder)
        self.device = MTLCreateSystemDefaultDevice()
        setup()
    }
    
    private func setup() {
        guard let device = self.device else {
            fatalError("Metal is not supported on this device")
        }
        
        // Configure Metal view
        colorPixelFormat = .bgra8Unorm
        depthStencilPixelFormat = .depth32Float
        framebufferOnly = true
        preferredFramesPerSecond = 60
        
        // Enable depth buffer
        clearColor = MTLClearColor(red: 0.1, green: 0.1, blue: 0.15, alpha: 1.0)
        clearDepth = 1.0
        
        delegate = self
        
        // Initialize native engine
        engine = kimoyooju_native_init(Unmanaged.passUnretained(self.layer as! CAMetalLayer).toOpaque())
        
        print("KimoyoOjuMetalView initialized with device: \(device.name)")
    }
    
    deinit {
        if let engine = engine {
            kimoyooju_native_destroy(engine)
        }
        arSession?.pause()
    }
    
    // MARK: - MTKViewDelegate
    
    public func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
        let width = Int(size.width)
        let height = Int(size.height)
        
        if let engine = engine {
            kimoyooju_native_surface_changed(engine, Int32(width), Int32(height))
        }
        
        onSurfaceChanged?(width, height)
    }
    
    public func draw(in view: MTKView) {
        guard let engine = engine else { return }
        
        // Process AR frame if AR is enabled
        if isAREnabled, let frame = arSession?.currentFrame {
            // Pass AR camera matrices to engine
            let viewMatrix = frame.camera.viewMatrix(for: .portrait)
            let projMatrix = frame.camera.projectionMatrix(for: .portrait,
                                                           viewportSize: bounds.size,
                                                           zNear: 0.01,
                                                           zFar: 1000)
            
            withUnsafePointer(to: viewMatrix) { viewPtr in
                withUnsafePointer(to: projMatrix) { projPtr in
                    kimoyooju_native_set_ar_matrices(engine,
                                                 viewPtr,
                                                 projPtr)
                }
            }
            
            onARFrame?(frame)
        }
        
        // Draw frame
        kimoyooju_native_draw_frame(engine)
    }
    
    // MARK: - ARSessionDelegate
    
    public func session(_ session: ARSession, didUpdate frame: ARFrame) {
        // Frame updates are processed in draw()
    }
    
    public func session(_ session: ARSession, cameraDidChangeTrackingState camera: ARCamera) {
        print("AR tracking state: \(camera.trackingState)")
    }
    
    public func session(_ session: ARSession, didFailWithError error: Error) {
        print("AR session error: \(error.localizedDescription)")
    }
    
    // MARK: - Public API
    
    public func enableAR(configuration: ARWorldTrackingConfiguration? = nil) {
        guard ARWorldTrackingConfiguration.isSupported else {
            print("ARKit not supported on this device")
            return
        }
        
        let config = configuration ?? {
            let c = ARWorldTrackingConfiguration()
            c.planeDetection = [.horizontal, .vertical]
            c.isLightEstimationEnabled = true
            return c
        }()
        
        if arSession == nil {
            arSession = ARSession()
            arSession?.delegate = self
        }
        
        arSession?.run(config)
        isAREnabled = true
        
        print("AR enabled")
    }
    
    public func disableAR() {
        arSession?.pause()
        isAREnabled = false
        print("AR disabled")
    }
    
    public func setXRMode(_ mode: String) {
        guard let engine = engine else { return }
        kimoyooju_native_set_xr_mode(engine, mode)
    }
    
    public func submitCommandBuffer(_ json: String) {
        guard let engine = engine else { return }
        kimoyooju_native_submit_commands(engine, json)
    }
    
    public func pause() {
        guard let engine = engine else { return }
        kimoyooju_native_pause(engine)
        isPaused = true
        
        if isAREnabled {
            arSession?.pause()
        }
    }
    
    public func resume() {
        guard let engine = engine else { return }
        kimoyooju_native_resume(engine)
        isPaused = false
        
        if isAREnabled, let config = arSession?.configuration {
            arSession?.run(config)
        }
    }
    
    // MARK: - Touch Handling
    
    public override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        if onTouchEvent?(location, 0) != true {
            super.touchesBegan(touches, with: event)
        }
    }
    
    public override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        if onTouchEvent?(location, 1) != true {
            super.touchesMoved(touches, with: event)
        }
    }
    
    public override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: self)
        
        if onTouchEvent?(location, 2) != true {
            super.touchesEnded(touches, with: event)
        }
    }
}

// MARK: - Native Bridge Functions

@_silgen_name("kimoyooju_native_init")
func kimoyooju_native_init(_ metalLayer: UnsafeMutableRawPointer) -> UnsafeMutableRawPointer?

@_silgen_name("kimoyooju_native_destroy")
func kimoyooju_native_destroy(_ engine: UnsafeMutableRawPointer)

@_silgen_name("kimoyooju_native_surface_changed")
func kimoyooju_native_surface_changed(_ engine: UnsafeMutableRawPointer, _ width: Int32, _ height: Int32)

@_silgen_name("kimoyooju_native_draw_frame")
func kimoyooju_native_draw_frame(_ engine: UnsafeMutableRawPointer)

@_silgen_name("kimoyooju_native_set_xr_mode")
func kimoyooju_native_set_xr_mode(_ engine: UnsafeMutableRawPointer, _ mode: String)

@_silgen_name("kimoyooju_native_submit_commands")
func kimoyooju_native_submit_commands(_ engine: UnsafeMutableRawPointer, _ json: String)

@_silgen_name("kimoyooju_native_pause")
func kimoyooju_native_pause(_ engine: UnsafeMutableRawPointer)

@_silgen_name("kimoyooju_native_resume")
func kimoyooju_native_resume(_ engine: UnsafeMutableRawPointer)

@_silgen_name("kimoyooju_native_set_ar_matrices")
func kimoyooju_native_set_ar_matrices(_ engine: UnsafeMutableRawPointer,
                                  _ viewMatrix: UnsafeRawPointer,
                                  _ projMatrix: UnsafeRawPointer)
