import Foundation
import MetalKit
import SwiftUI
import Voxcell

/// describes the world which is rendered to the screen and to which the user interacts
///   * It contains the camera, lights, and 3D objects to be presented on the screen.
///   * It responds to the drawing request from Metal (MTKView) through RenderCoordinator and emits draw calls to the spcecified metal encoder.
///   * It receives the user inputs such as tap, pinch, drag, as well as the inputs from the UI controls such as toggle switches etc, and update the world accordingly.
class WorldManager : ObservableObject {

    var      device:                MTLDevice!
    var      colorPixelFormat:      MTLPixelFormat
    var      textureStorage:        TextureStorage
    var      scene:                 RenderingPerScene?
    var      camera:                Camera
    var      autoRotate:            Bool
    var      lights:                [ LightSharedWithFragmentShaders ]
    var      modelContainerSpot:    ModelContainer
    var      modelContainerDuck:    ModelContainer
    var      activeFigure:          Figure
    var      models:                [ RenderingPerModel ]
    var      modelUniforms:         [ [ UniformsPerInstanceSharedWithVertexShaders ] ]
    var      sampleResolution:      Float

    public init( device : MTLDevice ) {

        self.device           = device
        self.colorPixelFormat = .invalid
        self.textureStorage   = TextureStorage( device: device )
        self.scene            = RenderingPerScene( device: device, maxNumLights: 3 )
        
        self.camera           = Camera()
        self.autoRotate       = true
        self.lights           = []
        self.models           = []
        self.modelUniforms    = []
        self.sampleResolution = 0.256

        self.modelContainerSpot = ModelContainer(
            name:           "spot_smoothed.obj",
            resolution:     self.sampleResolution,
            device:         device,
            textureStorage: textureStorage
        )

        self.modelContainerDuck = ModelContainer(
            name:           "duck_smoothed.obj",
            resolution:     self.sampleResolution,
            device:         device,
            textureStorage: textureStorage
        )
        self.activeFigure = .QuackQuack

        self.setupLights()
    }


    func setupLights() {
    
        var sun1 = LightSharedWithFragmentShaders()
        sun1.position = SIMD4<Float>( 100.0, 100.0, 100.0, 1.0 )

        var sun2 = LightSharedWithFragmentShaders()
        sun2.position = SIMD4<Float>( -100.0, 100.0, -100.0, 1.0 )

        var sun3 = LightSharedWithFragmentShaders()
        sun3.position = SIMD4<Float>(    0.0, -100.0,   0.0, 1.0 )

        var ambient = LightSharedWithFragmentShaders()
        ambient.color     = SIMD4<Float>( 1.0, 1.0, 1.0, 1.0 )
        ambient.intensity = 0.2
        ambient.lightType = LightType.Ambientlight.rawValue

        self.lights = [ sun1, sun2, sun3, ambient ]
    }

    func changeFigureTo( figure: Figure ) { activeFigure = figure }

    func changeRepresentationTo( representation: Representation ) {
        modelContainerSpot.changeRepresentationTo( representation: representation )
        modelContainerDuck.changeRepresentationTo( representation: representation )
    }

    func changeResolution( resolution: Float ) {
        if ( resolution != self.sampleResolution ){
            modelContainerSpot.resample( resolution: resolution )
            modelContainerSpot.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
            modelContainerDuck.resample( resolution: resolution )
            modelContainerDuck.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
            self.sampleResolution = resolution
        }
    }

    func changeVoxelTypeTo( voxelType: VoxelType ) {
        modelContainerSpot.changeVoxelTypeTo( voxelType: voxelType )
        modelContainerDuck.changeVoxelTypeTo( voxelType: voxelType )
    }

    func changeHullTypeTo( hullType: HullType ) {
        modelContainerSpot.changeHullTypeTo( hullType: hullType )
        modelContainerDuck.changeHullTypeTo( hullType: hullType )
    }

    func setAutoRotate( value: Bool ) {
        self.autoRotate = value
    }

    func createPipelineDescriptors( colorPixelFormat: MTLPixelFormat ) {

        self.colorPixelFormat = colorPixelFormat
        
        modelContainerSpot.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelContainerDuck.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
    }

    func updateWorld() {

        if autoRotate {
            camera.rotation.y += 0.02
        }

        scene?.updateLightsAndCamera( camera: camera, lights: &lights )

        modelContainerSpot.updateWorld()
        modelContainerDuck.updateWorld()
    }

    func encode( encoder : MTLRenderCommandEncoder ) {
    
        scene?.encode( encoder : encoder )

        if activeFigure == .MooMoo {

            modelContainerSpot.encode( encoder: encoder )

        } else {
            modelContainerDuck.encode( encoder: encoder )
        }
    }

    func updateScreenSizes( size: CGSize ){
        camera.aspect = Float( size.width ) / Float( size.height )
    }
    
    func handleTap   ( location: SIMD2<Float> ) {
    }

    func handlePan   ( delta: SIMD2<Float> ) {
        camera.rotate( delta: delta )
    }

    func handlePinch ( scale: Float ) {

        camera.zoom( scale: scale )
    }
}

class ModelContainer {

    let      device:            MTLDevice
    let      textureStorage:    TextureStorage

    var      representationType: Representation
    var      hullType:           HullType
    var      voxelType:          VoxelType

    let      modelOriginal:     RenderingPerModel
    let      uniformOriginal:   [ UniformsPerInstanceSharedWithVertexShaders ]

    let      mdlMeshForResampling:  MDLMesh
    let      textureForResampling:  MTLTexture?

    let      modelCH:           RenderingPerModel
    let      uniformCH:         [ UniformsPerInstanceSharedWithVertexShaders ]

    let      modelOBB:          RenderingPerModel
    let      uniformOBB:        [ UniformsPerInstanceSharedWithVertexShaders ]

    var      modelVoxel:        RenderingPerModel?
    var      uniformVoxel:      [ UniformsPerInstanceSharedWithVertexShaders ] = []

    var      modelSpheres:      RenderingPerModel?
    var      uniformSpheres:    [ UniformsPerInstanceSharedWithVertexShaders ] = []

    var      modelAxes:         RenderingPerModel?
    var      uniformAxes:       [ UniformsPerInstanceSharedWithVertexShaders ] = []

    init( name: String, resolution: Float, device: MTLDevice!, textureStorage: TextureStorage ) {

        self.device = device
        self.textureStorage = textureStorage
        self.voxelType = .Voxel
        self.hullType  = .ConvexHull
        self.representationType = .Original

        guard let assetUrl = Bundle.main.url( forResource: name, withExtension: nil )
        else {
            fatalError( "Model: \(name) not found" )
        }

        let allocator = MTKMeshBufferAllocator( device: device )
        let vertexDescriptor = RenderUtil.defaultVertexDescriptorPositionNormalTextureCoordinate()
        let asset = MDLAsset(
                        url:              assetUrl,
                        vertexDescriptor: vertexDescriptor,
                        bufferAllocator:  allocator
                    )

        let mdlMeshes = asset.childObjects( of: MDLMesh.self ) as! [MDLMesh]
        let mdlSubmesh : MDLSubmesh = mdlMeshes[0].submeshes![0] as! MDLSubmesh
        let material = mdlSubmesh.material!
        textureStorage.findAndStoreTexturesIn( material : material )

        mdlMeshForResampling = mdlMeshes[0]
        textureForResampling = textureStorage.mapping[ material.textureNameForBaseColor()! ]

        modelOriginal = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                mdlMeshes[0],
            doAlphaBlending:        false,
            numPerInstanceUniforms: 1
        )

        uniformOriginal = [ UniformsPerInstanceSharedWithVertexShaders() ]

        let coordinates = ExtractedCoordinatesFromMTKMesh( mdlMesh: mdlMeshes[0], device: device )

        let ch = Brep.findConvexHull( vertices : coordinates.vertices )

        let obb = Brep.findOrientedBoundingBox( vertices : coordinates.vertices )

        let mdlMeshCH = ch.generateMDLMesh(
            device:           device,
            type:             MDLGeometryType.triangles,
            vertexDescriptor: nil,
            material:         nil,
            color:            SIMD3<Float>( 0.8, 0.5, 0.5 )
        )

        modelCH = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                mdlMeshCH!,
            doAlphaBlending:        true,
            numPerInstanceUniforms: 1
        )

        uniformCH = [ UniformsPerInstanceSharedWithVertexShaders() ]

        let mdlMeshOBB = obb.generateMDLMesh(
            device:           device,
            type:             MDLGeometryType.triangles,
            vertexDescriptor: nil,
            material:         nil,
            color:            SIMD3<Float>( 0.3, 0.5, 0.8 )
        )

        modelOBB = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                mdlMeshOBB!,
            doAlphaBlending:        true,
            numPerInstanceUniforms: 1
        )

        uniformOBB = [ UniformsPerInstanceSharedWithVertexShaders() ]

        resample( resolution: resolution )
    }

    func createPipelineDescriptors( colorPixelFormat: MTLPixelFormat ) {

        modelOriginal.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelCH.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelOBB.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelVoxel!.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelSpheres!.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        modelAxes!.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
    }

    func updateWorld() {
        
        modelOriginal.updatePerInstanceUniforms( uniforms : uniformOriginal )
        modelCH.updatePerInstanceUniforms( uniforms : uniformCH )
        modelOBB.updatePerInstanceUniforms( uniforms : uniformOBB )
        modelVoxel!.updatePerInstanceUniforms( uniforms : uniformVoxel )
        modelSpheres!.updatePerInstanceUniforms( uniforms : uniformSpheres )
        modelAxes!.updatePerInstanceUniforms( uniforms : uniformAxes )
    }

    func changeRepresentationTo( representation: Representation ) { self.representationType = representation }

    func changeVoxelTypeTo( voxelType: VoxelType ) { self.voxelType = voxelType }

    func changeHullTypeTo( hullType: HullType ) { self.hullType = hullType }

    func encode( encoder : MTLRenderCommandEncoder ) {
        switch representationType {
            case .Original:
                modelOriginal.encode( encoder: encoder )
            case .VolumeSampled:
                switch voxelType {
                    case .Voxel:
                        modelVoxel!.encode( encoder: encoder )
                    case .Balls:
                        modelSpheres!.encode( encoder: encoder )
                }
            case .Hull:
                switch hullType {
                    case .ConvexHull:
                        modelCH.encode( encoder: encoder )
                    case .OBB:
                        modelOriginal.encode( encoder: encoder )
                        modelOBB.encode( encoder: encoder )
                }
            case .ComInertia:
                modelAxes!.encode( encoder: encoder )
        }
    }

    func resample( resolution: Float ) {

        let volumeSampler = VolumeSampler(
            device:            device,
            useExternalShader: false,
            mdlMesh:           mdlMeshForResampling,
            texture:           textureForResampling,
            defaultColor:      SIMD3<Float>( 0.8, 0.8, 0.8 ),
            pitch:             resolution,
            epsilon:           0.0001
        )
        
        let mdlMeshVoxel = volumeSampler.generateMDLMesh(
            vertexDescriptor: RenderUtil.defaultVertexDescriptorPositionNormalColor(),
            material:         RenderUtil.defaultMaterial( color: SIMD3<Float>( 0.8, 0.8, 0.8 ) ),
            useTextureColors: true
        )

        modelVoxel = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                mdlMeshVoxel!,
            doAlphaBlending:        false,
            numPerInstanceUniforms: 1
        )

        uniformVoxel = [ UniformsPerInstanceSharedWithVertexShaders() ]
       
        let sphere = Sphere(
           center                : SIMD3<Float>(0,0,0),
           radius                : volumeSampler.gridInfo!.pitch / 2.0,
           numPointsAroundCircle : 32,
           color                 : SIMD3<Float>( 1.0, 1.0, 1.0 )
        )
        volumeSampler.shrinkByHalfStep()
        let sphereCenters = volumeSampler.findShellCenterPoints()
        let spheres = InstancedSphere(
            sphere:  sphere,
            centers: sphereCenters
        )

        let sphereMesh = spheres.generateMDLMesh(
            device:           device,
            vertexDescriptor: RenderUtil.defaultVertexDescriptorPositionNormalColor(),
            material:         RenderUtil.defaultMaterial( color: sphere.color ),
            defaultColor:     SIMD3<Float>( 0.5,0.5,0.5 )
        )

        modelSpheres = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                sphereMesh!,
            doAlphaBlending:        false,
            numPerInstanceUniforms: sphereCenters.count
        )

        uniformSpheres = spheres.generatePerIncetanceUniforms()
        
        let arrows = generateAxes( volumeSampler: volumeSampler, scale: 1.0 )

        let arrowsMesh = arrows.generateMDLMesh(
            device:           device,
            vertexDescriptor: RenderUtil.defaultVertexDescriptorPositionNormalColor(),
            material:         RenderUtil.defaultMaterial( color: sphere.color ),
            defaultColor:     SIMD3<Float>( 0.5,0.5,0.5 )
        )

        modelAxes = RenderingPerModel(
            device:                 device,
            textureStorage:         textureStorage,
            mdlMesh:                arrowsMesh!,
            doAlphaBlending:        false,
            numPerInstanceUniforms: 1
        )

        uniformAxes = [ UniformsPerInstanceSharedWithVertexShaders() ]
    }

    func generateAxes( volumeSampler: VolumeSampler, scale : Float ) -> LinesAndArrows
    {
        let CoM = volumeSampler.findCenterOfMass()
        let inertia = volumeSampler.findInertiaTensor( densityPerUnitVolume: 0.004 )
        let eigen = GeomPrimitives.EigenVectorsIfSymmetric( mat: inertia )
        
        let arrow1 = LineOrArrow(
            arrowHeight : 0.0,
            arrowRadius : 0.0,
            lineRadius  : 0.01,
            numSides    : 16,
            color       : SIMD3<Float>(  0.1,   0.0,   0.0 ),
            p1          : SIMD3<Float>(-50.0,   0.0,   0.0 ),
            p2          : SIMD3<Float>( 50.0,   0.0,   0.0 )
        )

        let arrow2 = LineOrArrow(
            arrowHeight : 0.0,
            arrowRadius : 0.0,
            lineRadius  : 0.01,
            numSides    : 16,
            color       : SIMD3<Float>(  0.0,   0.1,   0.0 ),
            p1          : SIMD3<Float>(  0.0, -50.0,   0.0 ),
            p2          : SIMD3<Float>(  0.0,  50.0,   0.0 )
        )

        let arrow3 = LineOrArrow(
            arrowHeight : 0.0,
            arrowRadius : 0.0,
            lineRadius  : 0.01,
            numSides    : 16,
            color       : SIMD3<Float>(  0.0,   0.0,   0.1 ),
            p1          : SIMD3<Float>(  0.0,   0.0, -50.0 ),
            p2          : SIMD3<Float>(  0.0,   0.0,  50.0 )
        )
        
        let arrow4 = LineOrArrow(
            arrowHeight : 0.2 * scale,
            arrowRadius : 0.1 * scale,
            lineRadius  : 0.03 * scale,
            numSides    : 16,
            color       : SIMD3<Float>(  1.0,   0.5,   0.5 ),
            p1          : CoM,
            p2          : CoM + ( eigen.eigenVectors[0] * eigen.eigenValues[0] * 1000.0 * scale )
        )

        let arrow5 = LineOrArrow(
            arrowHeight : 0.2 * scale,
            arrowRadius : 0.1 * scale,
            lineRadius  : 0.03 * scale,
            numSides    : 16,
            color       : SIMD3<Float>(  0.5,   1.0,   0.5 ),
            p1          : CoM,
            p2          : CoM + ( eigen.eigenVectors[1] * eigen.eigenValues[1] * 1000.0 * scale )
        )

        let arrow6 = LineOrArrow(
            arrowHeight : 0.2 * scale,
            arrowRadius : 0.1 * scale,
            lineRadius  : 0.03 * scale,
            numSides    : 16,
            color       : SIMD3<Float>(  0.5,   0.5,   1.0 ),
            p1          : CoM,
            p2          : CoM + ( eigen.eigenVectors[2] * eigen.eigenValues[2] * 1000.0 * scale )
        )

        let arrows = LinesAndArrows()
        arrows.elements.append(arrow1)
        arrows.elements.append(arrow2)
        arrows.elements.append(arrow3)
        arrows.elements.append(arrow4)
        arrows.elements.append(arrow5)
        arrows.elements.append(arrow6)

        return arrows
    }
}

