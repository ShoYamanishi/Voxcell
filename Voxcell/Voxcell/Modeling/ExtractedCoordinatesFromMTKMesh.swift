import Foundation
import MetalKit

public class ExtractedCoordinatesFromMTKMesh {

    var vertices:     [SIMD3<Float>] = []
    var masks:        [UInt32]       = []
    var normals:      [SIMD3<Float>] = []
    var textureUVs:   [SIMD2<Float>] = []


    /// contrcuts 4 linear (non-interleaved) arrays from MDLMesh for VolumeSampler.
    /// This aggregate is needed to wrap the arrays in a class, as the arrays are
    /// a value type in Swift.
    public convenience init( mdlMesh: MDLMesh, device: MTLDevice ) {
        do {
            let mtkMesh = try MTKMesh( mesh: mdlMesh, device: device )
            self.init( mtkMesh: mtkMesh )
        } catch {
            fatalError()
        }
    }

    /// contrcuts 4 linear (non-interleaved) arrays from MTKMesh for VolumeSampler.
    /// This aggregate is needed to wrap the arrays in a class, as the arrays are
    /// a value type in Swift.
    public init( mtkMesh: MTKMesh ) {

        let verticesCompact   = mtkMesh.getPositions()
        let normalsCompact    = mtkMesh.getNormals()
        let textureUVsCompact = mtkMesh.getTextureCoordinates()
        let triangleIndices   = mtkMesh.getTriangleIndices()

        for triangle in triangleIndices {

            vertices  .append( verticesCompact  [ Int(triangle[0]) ] )
            normals   .append( normalsCompact   [ Int(triangle[0]) ] )
            textureUVs.append( textureUVsCompact[ Int(triangle[0]) ] )
            masks     .append( 1 )

            vertices  .append( verticesCompact  [ Int(triangle[1]) ] )
            normals   .append( normalsCompact   [ Int(triangle[1]) ] )
            textureUVs.append( textureUVsCompact[ Int(triangle[1]) ] )
            masks     .append( 1 )

            vertices  .append( verticesCompact  [ Int(triangle[2]) ] )
            normals   .append( normalsCompact   [ Int(triangle[2]) ] )
            textureUVs.append( textureUVsCompact[ Int(triangle[2]) ] )
            masks     .append( 1 )
        }
    }
}
