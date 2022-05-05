import Foundation
import MetalKit

class QuadGenerationHelper {

    public let structureOfArrays : StructureOfArraysForMDLMesh

    let generatePoints:             Bool
    let generateNormals:            Bool
    let generateTangents:           Bool
    let generateBitangents:         Bool
    let generateTextureCoordinates: Bool
    let generateColors:             Bool

    let volumeSampler: VolumeSampler

    var vertexIndex : Int32

    /// generates arrays of coordinates for rendering the voxel in a composition of quads. The arrays generated here can be used to generate MDLMesh etc.
    /// - parameter volumeSampler: volumeSampler. It internally uses volumeBitmap and sweepLines.
    ///
    /// - parameter useTextureColors: use the colors sampled during the depth peeling.
    ///
    /// **arrayVertex**: array of points of triangles, 4 points per quad.
    ///
    /// **arrayNormal**: array of normals, 4 normals per quad.
    ///
    /// **arrayColor**: array of colors, 4 colors per quad.
    ///
    /// **arrayIndices**: array of indices into the 3 arrays above. Two triangles per quad.

    public init(
        volumeSampler:              VolumeSampler,
        generatePoints:             Bool,
        generateNormals:            Bool,
        generateTangents:           Bool,
        generateBitangents:         Bool,
        generateTextureCoordinates: Bool,
        generateColors:             Bool,
        useTextureColors:           Bool
    ) {
        self.volumeSampler              = volumeSampler
        self.generatePoints             = generatePoints
        self.generateNormals            = generateNormals
        self.generateTangents           = generateTangents
        self.generateBitangents         = generateBitangents
        self.generateTextureCoordinates = generateTextureCoordinates
        self.generateColors             = generateColors

        self.structureOfArrays = StructureOfArraysForMDLMesh(

            useVertices:           generatePoints,
            useNormals:            generateNormals,
            useTangents:           generateTangents,
            useBitangents:         generateBitangents,
            useTextureCoordinates: generateTextureCoordinates,
            useColors:             generateColors
        )

        self.vertexIndex = 0

        for x in 0 ..< volumeSampler.gridInfo!.dimension.x {
        
            for y in 0 ..< volumeSampler.gridInfo!.dimension.y {

                for z in 0 ..< volumeSampler.gridInfo!.dimension.z {

                    if volumeSampler.volumeBitmap!.isSet( x: x, y: y, z: z ) {

                        if x == 0 || ( x > 0 && volumeSampler.volumeBitmap!.isNotSet( x: x-1, y: y, z: z ) ) {

                            generateLowerFaceYZ( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }
                        if x == volumeSampler.gridInfo!.dimension.x - 1 || ( x < volumeSampler.gridInfo!.dimension.x - 1 && volumeSampler.volumeBitmap!.isNotSet( x: x+1, y: y, z: z ) ) {
                           
                            generateHigherFaceYZ( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }

                        if y == 0 || ( y > 0 && volumeSampler.volumeBitmap!.isNotSet( x: x, y: y-1, z: z ) ) {

                            generateLowerFaceZX( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }
                        if y == volumeSampler.gridInfo!.dimension.y - 1 || ( y < volumeSampler.gridInfo!.dimension.y - 1 && volumeSampler.volumeBitmap!.isNotSet( x: x, y: y+1, z: z ) ) {

                            generateHigherFaceZX( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }

                        if z == 0 || ( z > 0 && volumeSampler.volumeBitmap!.isNotSet( x: x, y: y, z: z-1 ) ) {

                            generateLowerFaceXY( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }
                        if z == volumeSampler.gridInfo!.dimension.z - 1 || ( z < volumeSampler.gridInfo!.dimension.z - 1 && volumeSampler.volumeBitmap!.isNotSet( x: x, y: y, z: z+1 ) ) {

                            generateHigherFaceXY( x:x, y:y, z:z, useTextureColors: useTextureColors )
                        }
                    }
                }
            }
        }
    }

    func generateLowerFaceYZ( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y,   z: z   )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y,   z: z+1 )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y+1, z: z+1 )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y+1, z: z   )

        let  n = SIMD3<Float>( -1.0,  0.0,  0.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func generateHigherFaceYZ( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z   )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z   )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z+1 )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z+1 )

        let  n = SIMD3<Float>(  1.0,  0.0,  0.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func generateLowerFaceZX( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x  , y: y,   z: z   )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z   )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z+1 )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x  , y: y,   z: z+1 )

        let  n = SIMD3<Float>(  0.0, -1.0,  0.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func generateHigherFaceZX( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x  , y: y+1, z: z   )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y+1, z: z+1 )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z+1 )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z   )

        let  n = SIMD3<Float>(  0.0,  1.0,  0.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func generateLowerFaceXY( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x  , y: y,   z: z   )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y+1, z: z   )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z   )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z   )

        let  n = SIMD3<Float>(  0.0,  0.0, -1.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesZ( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func generateHigherFaceXY( x: Int, y: Int, z: Int, useTextureColors: Bool ) -> Void {

        let v1 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x  , y: y,   z: z+1 )
        let v2 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y,   z: z+1 )
        let v3 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x+1, y: y+1, z: z+1 )
        let v4 = volumeSampler.gridInfo!.gridPointFromGridCoords( x: x,   y: y+1, z: z+1 )

        let  n = SIMD3<Float>(  0.0,  0.0,  1.0 )
        let  c = useTextureColors ? volumeSampler.pickColorFromSweepLinesZ( x: x, y: y, z: z, compensate: true, relax: false )
                                  : (false, volumeSampler.defaultColor)

        addVerticesAndIndicesToMLDMeshArrays( vertex1: v1, vertex2: v2, vertex3: v3, vertex4: v4, normal: n, color: c.1 )
    }

    func addVerticesAndIndicesToMLDMeshArrays(

        vertex1: SIMD3<Float>,
        vertex2: SIMD3<Float>,
        vertex3: SIMD3<Float>,
        vertex4: SIMD3<Float>,
        normal:  SIMD3<Float>,
        color:   SIMD3<Float>
    ) -> Void {

        if generatePoints {

            structureOfArrays.vertices!.append( vertex1 )
            structureOfArrays.vertices!.append( vertex2 )
            structureOfArrays.vertices!.append( vertex3 )
            structureOfArrays.vertices!.append( vertex4 )
        }

        if generateNormals {

            structureOfArrays.normals!.append( normal )
            structureOfArrays.normals!.append( normal )
            structureOfArrays.normals!.append( normal )
            structureOfArrays.normals!.append( normal )
        }

        if generateColors {

            structureOfArrays.colors!.append( color )
            structureOfArrays.colors!.append( color )
            structureOfArrays.colors!.append( color )
            structureOfArrays.colors!.append( color )
        }

        if generateTangents {

            let tangent = tangentFromNormal( normal: normal )
            structureOfArrays.tangents!.append( tangent )
            structureOfArrays.tangents!.append( tangent )
            structureOfArrays.tangents!.append( tangent )
            structureOfArrays.tangents!.append( tangent )
        }

        if generateBitangents {

            let bitangent = bitangentFromNormal( normal: normal )
            structureOfArrays.bitangents!.append( bitangent )
            structureOfArrays.bitangents!.append( bitangent )
            structureOfArrays.bitangents!.append( bitangent )
            structureOfArrays.bitangents!.append( bitangent )
        }

        if generateTextureCoordinates {

            structureOfArrays.textureCoordinates!.append( SIMD2<Float>(0.0, 0.0) )
            structureOfArrays.textureCoordinates!.append( SIMD2<Float>(0.0, 0.0) )
            structureOfArrays.textureCoordinates!.append( SIMD2<Float>(0.0, 0.0) )
            structureOfArrays.textureCoordinates!.append( SIMD2<Float>(0.0, 0.0) )
        }

        structureOfArrays.arrayIndices.append( vertexIndex     )
        structureOfArrays.arrayIndices.append( vertexIndex + 1 )
        structureOfArrays.arrayIndices.append( vertexIndex + 2 )

        structureOfArrays.arrayIndices.append( vertexIndex     )
        structureOfArrays.arrayIndices.append( vertexIndex + 2 )
        structureOfArrays.arrayIndices.append( vertexIndex + 3 )

        vertexIndex += 4
    }
    
    func tangentFromNormal( normal : SIMD3<Float> ) -> SIMD3<Float> {

        switch normal {

          case SIMD3<Float>(1.0, 0.0, 0.0):
              return SIMD3<Float>(0,1,0)

          case SIMD3<Float>(-1.0, 0.0, 0.0):
              return SIMD3<Float>(0,-1,0)

          case SIMD3<Float>(0.0, 1.0, 0.0):
              return SIMD3<Float>(0,0,1)

          case SIMD3<Float>(0.0, -1.0, 0.0):
              return SIMD3<Float>(0,0,-1)

          case SIMD3<Float>(0.0, 0.0, 1.0):
              return SIMD3<Float>(1,0,0)

          case SIMD3<Float>(0.0, 0.0, -1.0):
              return SIMD3<Float>(-1,0,0)

          default:
              return SIMD3<Float>(0,0,0)

        }
    }

    func bitangentFromNormal( normal : SIMD3<Float> ) -> SIMD3<Float> {

        switch normal {

          case SIMD3<Float>(1.0, 0.0, 0.0):
              return SIMD3<Float>(0,0,1)

          case SIMD3<Float>(-1.0, 0.0, 0.0):
              return SIMD3<Float>(0,0,-1)

          case SIMD3<Float>(0.0, 1.0, 0.0):
              return SIMD3<Float>(1,0,0)

          case SIMD3<Float>(0.0, -1.0, 0.0):
              return SIMD3<Float>(-1,0,0)

          case SIMD3<Float>(0.0, 0.0, 1.0):
              return SIMD3<Float>(0,1,0)

          case SIMD3<Float>(0.0, 0.0, -1.0):
              return SIMD3<Float>(0,-1,0)

          default:
              return SIMD3<Float>(0,0,0)

        }
    }
}
