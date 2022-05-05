import Foundation
import simd
import MetalKit

struct Sphere {

    let center                : SIMD3<Float>
    let radius                : Float
    let numPointsAroundCircle : Int
    let color                 : SIMD3<Float>

    func generateMeshForTopCap( indexBegin: inout Int32 )

        -> ( points: [SIMD3<Float>], normals: [SIMD3<Float>], colors: [SIMD3<Float>], indices: [Int32] )
    {
        var points     : [ SIMD3<Float> ] = []
        var normals    : [ SIMD3<Float> ] = []
        var colors     : [ SIMD3<Float> ] = []
        var indices    : [ Int32        ] = []

        let arcAngle = 2.0 * Float.pi / Float(numPointsAroundCircle)

        let northPole = SIMD3<Float>( center.x, center.y + radius, center.z )
        let centerOfCircle = SIMD3<Float>( center.x, center.y + (radius * cos(arcAngle)), center.z )
        let radiusOfCircle = radius * sin(arcAngle)

        for longitude in 0 ..< numPointsAroundCircle {

            let pointOnCircle1 = SIMD3<Float>(
                x: center.x + radiusOfCircle * sin( Float(longitude) * arcAngle ),
                y: centerOfCircle.y,
                z: center.z + radiusOfCircle * cos( Float(longitude) * arcAngle )
            )

            let pointOnCircle2 = SIMD3<Float>(
                x: center.x + radiusOfCircle * sin( Float(longitude+1) * arcAngle ),
                y: centerOfCircle.y,
                z: center.z + radiusOfCircle * cos( Float(longitude+1) * arcAngle )
            )
            
            points.append( pointOnCircle1 )
            points.append( pointOnCircle2 )
            points.append( northPole )

            normals.append( normalize( pointOnCircle1 - center) )
            normals.append( normalize( pointOnCircle2 - center) )
            normals.append( normalize( northPole - center) )

            colors.append( color )
            colors.append( color )
            colors.append( color )

            indices.append( indexBegin )
            indices.append( indexBegin + 1 )
            indices.append( indexBegin + 2 )

            indexBegin += 3
        }

        return ( points: points, normals:normals, colors: colors, indices: indices )
    }

    func generateMeshForMiddleStrip( lattitude: Int, indexBegin: inout Int32 )

        -> ( points: [SIMD3<Float>], normals: [SIMD3<Float>], colors: [SIMD3<Float>], indices: [Int32] )
    {
        var points     : [ SIMD3<Float> ] = []
        var normals    : [ SIMD3<Float> ] = []
        var colors     : [ SIMD3<Float> ] = []
        var indices    : [ Int32        ] = []
    
        let arcAngle = 2.0 * Float.pi / Float(numPointsAroundCircle)

        let centerOfCircle1 = SIMD3<Float>(
            x: center.x,
            y: center.y + radius * cos(arcAngle * Float( lattitude ) ),
            z: center.z
        )

        let centerOfCircle2 = SIMD3<Float>(
            x: center.x,
            y: center.y + radius * cos(arcAngle * Float( lattitude+1 ) ),
            z: center.z
        )

        let radiusOfCircle1 = radius * sin(arcAngle * Float( lattitude ) )
        let radiusOfCircle2 = radius * sin(arcAngle * Float( lattitude+1 ) )

        for longitude in 0 ..<  numPointsAroundCircle {

            let pointOnCircle1 = SIMD3<Float>(
                x: center.x + radiusOfCircle1 * sin( Float(longitude) * arcAngle ),
                y: centerOfCircle1.y,
                z: center.z + radiusOfCircle1 * cos( Float(longitude) * arcAngle )
            )

            let pointOnCircle2 = SIMD3<Float>(
                x: center.x + radiusOfCircle1 * sin( Float(longitude+1) * arcAngle ),
                y: centerOfCircle1.y,
                z: center.z + radiusOfCircle1 * cos( Float(longitude+1) * arcAngle )
            )

            let pointOnCircle3 = SIMD3<Float>(
                x: center.x + radiusOfCircle2 * sin( Float(longitude) * arcAngle ),
                y: centerOfCircle2.y,
                z: center.z + radiusOfCircle2 * cos( Float(longitude) * arcAngle )
            )

            let pointOnCircle4 = SIMD3<Float>(
                x: center.x + radiusOfCircle2 * sin( Float(longitude+1) * arcAngle ),
                y: centerOfCircle2.y,
                z: center.z + radiusOfCircle2 * cos( Float(longitude+1) * arcAngle )
            )

            points.append( pointOnCircle1 )
            points.append( pointOnCircle2 )
            points.append( pointOnCircle3 )
            points.append( pointOnCircle4 )

            normals.append( normalize( pointOnCircle1 - center) )
            normals.append( normalize( pointOnCircle2 - center) )
            normals.append( normalize( pointOnCircle3 - center) )
            normals.append( normalize( pointOnCircle4 - center) )

            colors.append( color )
            colors.append( color )
            colors.append( color )
            colors.append( color )

            indices.append( indexBegin )
            indices.append( indexBegin + 2 )
            indices.append( indexBegin + 3 )
            indices.append( indexBegin )
            indices.append( indexBegin + 3 )
            indices.append( indexBegin + 1 )

            indexBegin += 4
        }
        return ( points: points, normals:normals, colors: colors, indices: indices )
    }

    func generateMeshForBottomCap( indexBegin: inout Int32 )

        -> ( points: [SIMD3<Float>], normals: [SIMD3<Float>], colors: [SIMD3<Float>], indices: [Int32] )
    {
        var points     : [ SIMD3<Float> ] = []
        var normals    : [ SIMD3<Float> ] = []
        var colors     : [ SIMD3<Float> ] = []
        var indices    : [ Int32        ] = []

        let arcAngle = 2.0 * Float.pi / Float(numPointsAroundCircle)

        let southPole = SIMD3<Float>( center.x, center.y - radius, center.z )
        let centerOfCircle = SIMD3<Float>(
             x: center.x,
             y: center.y + radius * cos( arcAngle * Float(numPointsAroundCircle - 1) ),
             z: center.z
         )
        let radiusOfCircle = radius * sin( arcAngle * Float(numPointsAroundCircle - 1) )

        for longitude in 0 ..< numPointsAroundCircle {

            let pointOnCircle1 = SIMD3<Float>(
                x: center.x + radiusOfCircle * sin( Float(longitude) * arcAngle ),
                y: centerOfCircle.y,
                z: center.z + radiusOfCircle * cos( Float(longitude) * arcAngle )
            )

            let pointOnCircle2 = SIMD3<Float>(
                x: center.x + radiusOfCircle * sin( Float(longitude+1) * arcAngle ),
                y: centerOfCircle.y,
                z: center.z + radiusOfCircle * cos( Float(longitude+1) * arcAngle )
            )
            
            points.append( pointOnCircle2 )
            points.append( pointOnCircle1 )
            points.append( southPole )

            normals.append( normalize( pointOnCircle2 - center) )
            normals.append( normalize( pointOnCircle1 - center) )
            normals.append( normalize( southPole - center) )

            colors.append( color )
            colors.append( color )
            colors.append( color )

            indices.append( indexBegin )
            indices.append( indexBegin + 1 )
            indices.append( indexBegin + 2 )

            indexBegin += 3
        }

        return ( points: points, normals:normals, colors: colors, indices: indices )
    }


    func generateMesh( indexBegin: inout Int32 ) -> (points: [SIMD3<Float>], normals: [SIMD3<Float>], colors: [SIMD3<Float>], indices: [Int32]) {

        var points     : [ SIMD3<Float> ] = []
        var normals    : [ SIMD3<Float> ] = []
        var colors     : [ SIMD3<Float> ] = []
        var indices    : [ Int32        ] = []

        let v1 = generateMeshForTopCap( indexBegin: &indexBegin )

        points.append  ( contentsOf: v1.points  )
        normals.append ( contentsOf: v1.normals )
        colors.append  ( contentsOf: v1.colors  )
        indices.append ( contentsOf: v1.indices )

        for lattitude in 0 ..< numPointsAroundCircle - 1 {

            let v2 = generateMeshForMiddleStrip( lattitude: lattitude, indexBegin: &indexBegin )

            points.append  ( contentsOf: v2.points  )
            normals.append ( contentsOf: v2.normals )
            colors.append  ( contentsOf: v2.colors  )
            indices.append ( contentsOf: v2.indices )
        }

        let v3 = generateMeshForBottomCap( indexBegin: &indexBegin )

        points.append  ( contentsOf: v3.points  )
        normals.append ( contentsOf: v3.normals )
        colors.append  ( contentsOf: v3.colors  )
        indices.append ( contentsOf: v3.indices )

        return ( points: points, normals:normals, colors: colors, indices: indices )
    }
}

struct InstancedSphere {

    let sphere: Sphere
    let centers: [SIMD3<Float>]

    func generatePerIncetanceUniforms() -> [UniformsPerInstanceSharedWithVertexShaders] {

        var arrayInstance : [UniformsPerInstanceSharedWithVertexShaders] = []

        for center in centers {

            let perInstanceUniform = UniformsPerInstanceSharedWithVertexShaders(
                modelMatrix:  matrix_float4x4( translation: center ),
                normalMatrix: matrix_float3x3( [ 1.0, 0.0, 0.0 ], [ 0.0, 1.0, 0.0 ], [ 0.0, 0.0, 1.0 ] )
            )
            arrayInstance.append( perInstanceUniform )
        }

        return arrayInstance
    }

    func generateMDLMesh(

            device:           MTLDevice,
            vertexDescriptor: MDLVertexDescriptor?,
            material:         MDLMaterial?,
            defaultColor:     SIMD3<Float>?

    ) -> MDLMesh? {

      let structureOfArrays = StructureOfArraysForMDLMesh(
               useVertices:           true,
               useNormals:            true,
               useTangents:           false,
               useBitangents:         false,
               useTextureCoordinates: false,
               useColors:             true
        )
        var vertexIndex : Int32 = 0

        let arrays = sphere.generateMesh( indexBegin: &vertexIndex )

        structureOfArrays.vertices!.append    ( contentsOf: arrays.points  )
        structureOfArrays.normals!.append     ( contentsOf: arrays.normals )
        structureOfArrays.colors!.append      ( contentsOf: arrays.colors  )
        structureOfArrays.arrayIndices.append ( contentsOf: arrays.indices )

        var vd: MDLVertexDescriptor
        var c:  SIMD3<Float>
        var m:  MDLMaterial

        if vertexDescriptor == nil {
            vd = RenderUtil.defaultVertexDescriptorPositionNormalColor()
        } else {
            vd = vertexDescriptor!
        }
        if defaultColor == nil {
            c = SIMD3<Float>(0.5, 0.5, 0.5)
        } else {
            c = defaultColor!
        }
        if material == nil {
            m = RenderUtil.defaultMaterial( color: c )
        } else {
            m = material!
        }

        let mdlMesh = MDLMesh.generateFromStructureOfArrays(
            device:              device,
            vertexDescriptor:    vd,
            material:            m,
            structureOfArrays:   structureOfArrays
        )

        return mdlMesh
    }
}
