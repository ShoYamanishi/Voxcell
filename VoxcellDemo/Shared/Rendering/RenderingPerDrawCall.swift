import Foundation
import MetalKit

class RenderingPerDrawCall {

    static let vertexFunctionNamePositionNormalUVTangentBitangent:
                               String = "vertex_position_normal_uv_tangent_bitangent"

    static let vertexFunctionNamePositionNormalUV:
                               String = "vertex_position_normal_uv"

    static let vertexFunctionNamePositionNormalColor:
                               String = "vertex_position_normal_color"

    static let vertexFunctionNamePositionColor:
                               String = "vertex_position_color"

    let device:                MTLDevice

    var textureBaseColor:      MTLTexture?
    var textureTangentSpaceNormal: MTLTexture?
    var textureRoughness:      MTLTexture?

    let vertexDescriptor:      MDLVertexDescriptor
    let mdlMaterial:           MDLMaterial
    var materialFragmentBytes: MaterialFragmentBytesSharedWithFragmentShaders
    let functionConstants:     MTLFunctionConstantValues
    let doAlphaBlending:       Bool

    
    let vertexFunctionName:    String
    let fragmentFunctionName:  String = "fragment_default"

    var renderPipelineState:   MTLRenderPipelineState?
    let numInstances:          Int
    let mtkSubmesh:            MTKSubmesh

    var colorPixelFormat:      MTLPixelFormat

    public init(

        device:                MTLDevice,
        textureStorage:        TextureStorage,
        vertexDescriptor:      MDLVertexDescriptor,
        mdlMaterial:           MDLMaterial,
        doAlphaBlending:       Bool,
        numInstances:          Int,
        mtkSubmesh:            MTKSubmesh
    ) {
        textureStorage.findAndStoreTexturesIn( material: mdlMaterial )

        self.textureBaseColor = RenderingPerDrawCall.getTextureBaseColor(
            textureStorage: textureStorage,
            mdlMaterial:    mdlMaterial
        )

        self.textureTangentSpaceNormal = RenderingPerDrawCall.getTextureTangentSpaceNormal(
            textureStorage: textureStorage,
            mdlMaterial:    mdlMaterial
        )

        self.textureRoughness = RenderingPerDrawCall.getTextureRoughness(
            textureStorage: textureStorage,
            mdlMaterial:    mdlMaterial
        )

        self.device                = device
        self.colorPixelFormat      = .invalid
        self.vertexDescriptor      = vertexDescriptor
        self.mdlMaterial           = mdlMaterial
        self.materialFragmentBytes = MaterialFragmentBytesSharedWithFragmentShaders( mdlMaterial : mdlMaterial )
        self.functionConstants     = RenderingPerDrawCall.makeFunctionConstants(
            textureStorage:   textureStorage,
            vertexDescriptor: vertexDescriptor,
            mdlMaterial:      mdlMaterial
        )
        self.vertexFunctionName    = RenderingPerDrawCall.selectVertexFunction( vertexDescriptor: vertexDescriptor )
        self.doAlphaBlending       = doAlphaBlending
        self.renderPipelineState   = nil
        self.numInstances          = numInstances
        self.mtkSubmesh            = mtkSubmesh
        self.renderPipelineState   = makePipelineState()
        
    }

    public func createPipelineDescriptors( colorPixelFormat: MTLPixelFormat ) {

        self.colorPixelFormat = colorPixelFormat
        renderPipelineState   = makePipelineState()
    }

    public func encode( encoder :  MTLRenderCommandEncoder ) {

        encoder.setRenderPipelineState( renderPipelineState! )

        if let textureBaseColor = self.textureBaseColor {
            encoder.setFragmentTexture( textureBaseColor, index: TextureIndex.BaseColor.rawValue )
        }

        if let textureTangentSpaceNormal = self.textureTangentSpaceNormal {
            encoder.setFragmentTexture( textureTangentSpaceNormal, index: TextureIndex.TangentSpaceNormal.rawValue )
        }

        if let textureRoughness = self.textureRoughness {
            encoder.setFragmentTexture( textureRoughness, index: TextureIndex.Roughness.rawValue )
        }

        encoder.setFragmentBytes(
            &materialFragmentBytes,
            length: MemoryLayout<MaterialFragmentBytesSharedWithFragmentShaders>.stride,
            index:  BufferIndex.MaterialFragment.rawValue
        )

        encoder.drawIndexedPrimitives(
            type:              .triangle,
            indexCount:        mtkSubmesh.indexCount,
            indexType:         mtkSubmesh.indexType,
            indexBuffer:       mtkSubmesh.indexBuffer.buffer,
            indexBufferOffset: mtkSubmesh.indexBuffer.offset,
            instanceCount:     numInstances,
            baseVertex:        0,
            baseInstance:      0
        )
    }

    static func getTextureBaseColor( textureStorage: TextureStorage, mdlMaterial: MDLMaterial ) -> MTLTexture?
    {
        if let name = mdlMaterial.textureNameForBaseColor() {
             return textureStorage.mapping[ name ]
        }
        return nil
    }

    static func getTextureTangentSpaceNormal( textureStorage: TextureStorage, mdlMaterial: MDLMaterial ) -> MTLTexture?
    {
        if let name = mdlMaterial.textureNameForTangentSpaceNormal() {
             return textureStorage.mapping[ name ]
        }
        return nil
    }

    static func getTextureRoughness( textureStorage: TextureStorage, mdlMaterial: MDLMaterial ) -> MTLTexture?
    {
        if let name = mdlMaterial.textureNameForRoughness() {
             return textureStorage.mapping[ name ]
        }
        return nil
    }

    static func makeFunctionConstants(

        textureStorage:   TextureStorage,
        vertexDescriptor: MDLVertexDescriptor,
        mdlMaterial:      MDLMaterial

    ) -> MTLFunctionConstantValues {

        let functionConstants = MTLFunctionConstantValues()

        var hasNormals = vertexDescriptor.hasNormals()
        functionConstants.setConstantValue(
            &hasNormals,
            type: .bool,
            index: FunctionConstantIndex.VertexBufferHasNormals .rawValue
        )

        var hasTangents = vertexDescriptor.hasTangents()
        functionConstants.setConstantValue(
            &hasTangents,
            type: .bool,
            index: FunctionConstantIndex.VertexBufferHasTangents.rawValue
        )

        var hasBitangents = vertexDescriptor.hasBitangents()
        functionConstants.setConstantValue(
            &hasBitangents,
            type: .bool,
            index: FunctionConstantIndex.VertexBufferHasBitangents.rawValue
        )

        var hasTextureCoordinates = vertexDescriptor.hasTextureCoordinates()
        functionConstants.setConstantValue(
            &hasTextureCoordinates,
            type: .bool,
            index: FunctionConstantIndex.VertexBufferHasTextureCoordinates.rawValue
        )

        var hasColors = vertexDescriptor.hasColors()
        functionConstants.setConstantValue(
            &hasColors,
            type: .bool,
            index: FunctionConstantIndex.VertexBufferHasColors.rawValue
        )

        var textureHasBaseColor = getTextureBaseColor( textureStorage: textureStorage, mdlMaterial: mdlMaterial ) != nil
        functionConstants.setConstantValue(
            &textureHasBaseColor,
            type: .bool,
            index: FunctionConstantIndex.TextureBaseColor.rawValue
        )

        var textureHasTangentSpaceNormal = getTextureTangentSpaceNormal( textureStorage: textureStorage, mdlMaterial: mdlMaterial ) != nil
        functionConstants.setConstantValue(
            &textureHasTangentSpaceNormal,
            type: .bool,
            index: FunctionConstantIndex.TextureTangentSpaceNormal.rawValue
        )

        var textureHasRoughness = getTextureRoughness( textureStorage: textureStorage, mdlMaterial: mdlMaterial ) != nil
        functionConstants.setConstantValue(
            &textureHasRoughness,
            type: .bool,
            index: FunctionConstantIndex.TextureRoughness.rawValue
        )

        return functionConstants
    }
    
    static func selectVertexFunction( vertexDescriptor: MDLVertexDescriptor ) -> String {

        let hasNormals = vertexDescriptor.hasNormals()
        let hasTangents = vertexDescriptor.hasTangents()
        let hasBitangents = vertexDescriptor.hasBitangents()
        let hasUVs = vertexDescriptor.hasTextureCoordinates()
        let hasColors = vertexDescriptor.hasColors()

        if hasNormals && hasUVs && hasTangents && hasBitangents {

            return vertexFunctionNamePositionNormalUVTangentBitangent

        } else if hasNormals && hasUVs {

            return vertexFunctionNamePositionNormalUV

        } else if hasNormals && hasColors {
       
            return vertexFunctionNamePositionNormalColor

        } else if hasNormals && hasColors {

            return vertexFunctionNamePositionColor

        } else {
            fatalError()
        }
    }
    
    func makePipelineState() -> MTLRenderPipelineState
    {
        let library = device.makeDefaultLibrary()

        let vertexFunction:   MTLFunction?
        let fragmentFunction: MTLFunction?

        do {
            vertexFunction   = try library!.makeFunction( name: vertexFunctionName,   constantValues: functionConstants )
            fragmentFunction = try library!.makeFunction( name: fragmentFunctionName, constantValues: functionConstants )

        } catch{
            fatalError( "No Metal function exists" )
        }
        
        let pipelineDescriptor = MTLRenderPipelineDescriptor()
        pipelineDescriptor.vertexFunction   = vertexFunction
        pipelineDescriptor.fragmentFunction = fragmentFunction
    
        pipelineDescriptor.vertexDescriptor = MTKMetalVertexDescriptorFromModelIO( vertexDescriptor )
        pipelineDescriptor.colorAttachments[0].pixelFormat = colorPixelFormat

        if doAlphaBlending {
            pipelineDescriptor.colorAttachments[0].isBlendingEnabled = true
            pipelineDescriptor.colorAttachments[0].rgbBlendOperation = .add
            pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = .sourceAlpha
            pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = .oneMinusSourceAlpha
        }

        pipelineDescriptor.depthAttachmentPixelFormat = .depth32Float

        var pipelineState: MTLRenderPipelineState
        do {
            pipelineState = try device.makeRenderPipelineState( descriptor: pipelineDescriptor )
        } catch let error {

            fatalError( error.localizedDescription )
        }

        return pipelineState
    }
}
