// KimoyoOjuRealityEngine.swift
// RealityKit backend for iOS, iPadOS, and visionOS

import Foundation
import RealityKit
import ARKit
import Combine

// MARK: - Entity Handle

public struct EntityHandle: Hashable, Codable {
    public let id: UInt64
    public let generation: UInt32
    
    public init(id: UInt64, generation: UInt32 = 0) {
        self.id = id
        self.generation = generation
    }
}

// MARK: - Material Configuration

public struct PBRMaterialConfig {
    public var baseColor: SIMD4<Float> = [1, 1, 1, 1]
    public var metallic: Float = 0.0
    public var roughness: Float = 0.5
    public var emissive: SIMD3<Float>? = nil
    
    public init() {}
}

// MARK: - RealityKit Engine

public class KimoyoOjuRealityEngine {
    
    // MARK: - Properties
    
    private var arView: ARView?
    private var rootAnchor: AnchorEntity
    private var entityRegistry: [EntityHandle: Entity] = [:]
    private var nextId: UInt64 = 1
    private var currentGeneration: UInt32 = 0
    private var cancellables = Set<AnyCancellable>()
    
    // Callbacks
    public var onTrackingStateChanged: ((ARCamera.TrackingState) -> Void)?
    public var onPlanesUpdated: (([ARPlaneAnchor]) -> Void)?
    public var onSessionError: ((Error) -> Void)?
    
    // MARK: - Initialization
    
    public init() {
        rootAnchor = AnchorEntity(world: .zero)
    }
    
    deinit {
        shutdown()
    }
    
    // MARK: - View Management
    
    public func attachView(_ view: ARView) {
        self.arView = view
        view.scene.addAnchor(rootAnchor)
        
        // Configure default environment
        view.environment.lighting.intensityExponent = 1.0
        
        // Setup frame updates
        view.scene.subscribe(to: SceneEvents.Update.self) { [weak self] event in
            self?.onFrameUpdate(deltaTime: event.deltaTime)
        }.store(in: &cancellables)
    }
    
    public func detachView() {
        cancellables.removeAll()
        arView?.scene.anchors.removeAll()
        arView = nil
    }
    
    public func shutdown() {
        detachView()
        entityRegistry.removeAll()
        nextId = 1
    }
    
    // MARK: - Entity Creation
    
    public func createEntity(type: String, name: String) -> EntityHandle {
        let handle = allocateHandle()
        
        let entity: Entity
        switch type {
        case "box":
            let mesh = MeshResource.generateBox(size: 1.0)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "sphere":
            let mesh = MeshResource.generateSphere(radius: 0.5)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "plane":
            let mesh = MeshResource.generatePlane(width: 1, depth: 1)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "cylinder":
            let mesh = MeshResource.generateCylinder(height: 1, radius: 0.5)
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "text":
            let mesh = MeshResource.generateText(
                name,
                extrusionDepth: 0.01,
                font: .systemFont(ofSize: 0.1)
            )
            let material = SimpleMaterial(color: .white, isMetallic: false)
            entity = ModelEntity(mesh: mesh, materials: [material])
            
        case "group":
            entity = Entity()
            
        case "light":
            entity = Entity()
            let light = PointLight()
            light.light.intensity = 1000
            light.light.color = .white
            entity.components.set(light)
            
        case "directionalLight":
            entity = Entity()
            let light = DirectionalLight()
            light.light.intensity = 1000
            light.light.color = .white
            entity.components.set(light)
            
        case "spotLight":
            entity = Entity()
            let light = SpotLight()
            light.light.intensity = 1000
            light.light.color = .white
            entity.components.set(light)
            
        default:
            entity = Entity()
        }
        
        entity.name = name
        rootAnchor.addChild(entity)
        entityRegistry[handle] = entity
        
        return handle
    }
    
    public func destroyEntity(handle: EntityHandle) -> Bool {
        guard let entity = entityRegistry[handle] else { return false }
        entity.removeFromParent()
        entityRegistry.removeValue(forKey: handle)
        return true
    }
    
    // MARK: - Parenting
    
    public func setParent(child: EntityHandle, parent: EntityHandle?) {
        guard let childEntity = entityRegistry[child] else { return }
        
        if let parentHandle = parent, let parentEntity = entityRegistry[parentHandle] {
            childEntity.setParent(parentEntity)
        } else {
            childEntity.setParent(rootAnchor)
        }
    }
    
    // MARK: - Transforms
    
    public func setTransform(
        handle: EntityHandle,
        position: SIMD3<Float>,
        rotation: simd_quatf,
        scale: SIMD3<Float>
    ) {
        guard let entity = entityRegistry[handle] else { return }
        entity.position = position
        entity.orientation = rotation
        entity.scale = scale
    }
    
    public func setPosition(handle: EntityHandle, position: SIMD3<Float>) {
        guard let entity = entityRegistry[handle] else { return }
        entity.position = position
    }
    
    public func setRotation(handle: EntityHandle, rotation: simd_quatf) {
        guard let entity = entityRegistry[handle] else { return }
        entity.orientation = rotation
    }
    
    public func setScale(handle: EntityHandle, scale: SIMD3<Float>) {
        guard let entity = entityRegistry[handle] else { return }
        entity.scale = scale
    }
    
    // MARK: - Materials (PBR)
    
    public func setMaterial(handle: EntityHandle, config: PBRMaterialConfig) {
        guard let entity = entityRegistry[handle] as? ModelEntity else { return }
        
        var material = PhysicallyBasedMaterial()
        material.baseColor = .init(tint: UIColor(
            red: CGFloat(config.baseColor.x),
            green: CGFloat(config.baseColor.y),
            blue: CGFloat(config.baseColor.z),
            alpha: CGFloat(config.baseColor.w)
        ))
        material.metallic = .init(floatLiteral: config.metallic)
        material.roughness = .init(floatLiteral: config.roughness)
        
        if let emissive = config.emissive {
            material.emissiveColor = .init(color: UIColor(
                red: CGFloat(emissive.x),
                green: CGFloat(emissive.y),
                blue: CGFloat(emissive.z),
                alpha: 1.0
            ))
        }
        
        entity.model?.materials = [material]
    }
    
    public func setSimpleMaterial(handle: EntityHandle, color: SIMD4<Float>, isMetallic: Bool) {
        guard let entity = entityRegistry[handle] as? ModelEntity else { return }
        
        let uiColor = UIColor(
            red: CGFloat(color.x),
            green: CGFloat(color.y),
            blue: CGFloat(color.z),
            alpha: CGFloat(color.w)
        )
        let material = SimpleMaterial(color: uiColor, isMetallic: isMetallic)
        entity.model?.materials = [material]
    }
    
    // MARK: - Visibility
    
    public func setVisibility(handle: EntityHandle, visible: Bool) {
        guard let entity = entityRegistry[handle] else { return }
        entity.isEnabled = visible
    }
    
    // MARK: - Lights
    
    public func setPointLight(
        handle: EntityHandle,
        color: SIMD3<Float>,
        intensity: Float,
        attenuationRadius: Float
    ) {
        guard let entity = entityRegistry[handle] else { return }
        
        var light = PointLight()
        light.light.color = UIColor(
            red: CGFloat(color.x),
            green: CGFloat(color.y),
            blue: CGFloat(color.z),
            alpha: 1.0
        )
        light.light.intensity = intensity
        light.light.attenuationRadius = attenuationRadius
        entity.components.set(light)
    }
    
    public func setDirectionalLight(
        handle: EntityHandle,
        color: SIMD3<Float>,
        intensity: Float
    ) {
        guard let entity = entityRegistry[handle] else { return }
        
        var light = DirectionalLight()
        light.light.color = UIColor(
            red: CGFloat(color.x),
            green: CGFloat(color.y),
            blue: CGFloat(color.z),
            alpha: 1.0
        )
        light.light.intensity = intensity
        entity.components.set(light)
    }
    
    // MARK: - Asset Loading
    
    public func loadModel(url: String) async throws -> EntityHandle {
        guard let modelURL = URL(string: url) else {
            throw KimoyoOjuError.invalidURL
        }
        
        let entity = try await Entity(contentsOf: modelURL)
        let handle = allocateHandle()
        
        rootAnchor.addChild(entity)
        entityRegistry[handle] = entity
        
        return handle
    }
    
    public func loadModelSync(named: String, in bundle: Bundle? = nil) -> EntityHandle? {
        guard let entity = try? Entity.load(named: named, in: bundle) else {
            return nil
        }
        
        let handle = allocateHandle()
        rootAnchor.addChild(entity)
        entityRegistry[handle] = entity
        
        return handle
    }
    
    // MARK: - AR Configuration
    
    public func enableWorldTracking(
        planeDetection: Bool = true,
        sceneReconstruction: Bool = false,
        peopleOcclusion: Bool = false
    ) {
        guard let arView = arView else { return }
        
        let config = ARWorldTrackingConfiguration()
        
        if planeDetection {
            config.planeDetection = [.horizontal, .vertical]
        }
        
        if sceneReconstruction, ARWorldTrackingConfiguration.supportsSceneReconstruction(.mesh) {
            config.sceneReconstruction = .mesh
        }
        
        if peopleOcclusion, ARWorldTrackingConfiguration.supportsFrameSemantics(.personSegmentationWithDepth) {
            config.frameSemantics.insert(.personSegmentationWithDepth)
        }
        
        // Environment texturing for realistic reflections
        config.environmentTexturing = .automatic
        
        arView.session.run(config)
        
        // Setup delegate for tracking updates
        arView.session.delegate = SessionDelegate(engine: self)
    }
    
    public func pauseSession() {
        arView?.session.pause()
    }
    
    public func resumeSession() {
        guard let arView = arView, let config = arView.session.configuration else { return }
        arView.session.run(config)
    }
    
    // MARK: - Hit Testing
    
    public func hitTest(screenPoint: CGPoint) -> [HitTestResult] {
        guard let arView = arView else { return [] }
        
        let results = arView.hitTest(screenPoint, types: [.existingPlaneUsingGeometry, .estimatedHorizontalPlane])
        
        return results.map { result in
            HitTestResult(
                position: SIMD3<Float>(
                    result.worldTransform.columns.3.x,
                    result.worldTransform.columns.3.y,
                    result.worldTransform.columns.3.z
                ),
                distance: result.distance,
                type: result.type == .existingPlaneUsingGeometry ? .plane : .featurePoint
            )
        }
    }
    
    public func raycast(origin: SIMD3<Float>, direction: SIMD3<Float>) -> [HitTestResult] {
        guard let arView = arView else { return [] }
        
        let query = ARRaycastQuery(
            origin: origin,
            direction: direction,
            allowing: .estimatedPlane,
            alignment: .any
        )
        
        let results = arView.session.raycast(query)
        
        return results.map { result in
            HitTestResult(
                position: SIMD3<Float>(
                    result.worldTransform.columns.3.x,
                    result.worldTransform.columns.3.y,
                    result.worldTransform.columns.3.z
                ),
                distance: 0, // ARRaycastResult doesn't provide distance
                type: .plane
            )
        }
    }
    
    // MARK: - Anchors
    
    public func createAnchor(position: SIMD3<Float>) -> EntityHandle {
        let anchor = AnchorEntity(world: position)
        let handle = allocateHandle()
        
        arView?.scene.addAnchor(anchor)
        entityRegistry[handle] = anchor
        
        return handle
    }
    
    // MARK: - Private Helpers
    
    private func allocateHandle() -> EntityHandle {
        let id = nextId
        nextId += 1
        return EntityHandle(id: id, generation: currentGeneration)
    }
    
    private func onFrameUpdate(deltaTime: TimeInterval) {
        // Frame update logic - animations, physics, etc.
    }
    
    // MARK: - Session Delegate
    
    private class SessionDelegate: NSObject, ARSessionDelegate {
        weak var engine: KimoyoOjuRealityEngine?
        
        init(engine: KimoyoOjuRealityEngine) {
            self.engine = engine
        }
        
        func session(_ session: ARSession, cameraDidChangeTrackingState camera: ARCamera) {
            engine?.onTrackingStateChanged?(camera.trackingState)
        }
        
        func session(_ session: ARSession, didAdd anchors: [ARAnchor]) {
            let planes = anchors.compactMap { $0 as? ARPlaneAnchor }
            if !planes.isEmpty {
                engine?.onPlanesUpdated?(planes)
            }
        }
        
        func session(_ session: ARSession, didUpdate anchors: [ARAnchor]) {
            let planes = anchors.compactMap { $0 as? ARPlaneAnchor }
            if !planes.isEmpty {
                engine?.onPlanesUpdated?(planes)
            }
        }
        
        func session(_ session: ARSession, didFailWithError error: Error) {
            engine?.onSessionError?(error)
        }
    }
}

// MARK: - Supporting Types

public struct HitTestResult {
    public let position: SIMD3<Float>
    public let distance: Float
    public let type: HitType
    
    public enum HitType {
        case plane
        case featurePoint
        case mesh
    }
}

public enum KimoyoOjuError: Error {
    case invalidURL
    case entityNotFound
    case loadFailed(String)
    case sessionNotAvailable
}

// MARK: - visionOS Extensions

#if os(visionOS)
extension KimoyoOjuRealityEngine {
    
    public func configureForVisionOS() {
        // visionOS-specific configuration
        // Enable spatial tracking, hand tracking, etc.
    }
    
    public func enableHandTracking() async throws {
        // Hand tracking for visionOS
        let session = ARKitSession()
        let handTracking = HandTrackingProvider()
        try await session.run([handTracking])
    }
}
#endif
