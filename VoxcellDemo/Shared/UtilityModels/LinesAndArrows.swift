
import Foundation
import simd
import MetalKit

struct LineOrArrow {

    let arrowHeight : Float
    let arrowRadius : Float
    let lineRadius  : Float
    let numSides    : Int
    let color       : SIMD3<Float>
    let p1          : SIMD3<Float>
    let p2          : SIMD3<Float>

    /// represent an arrow or a line segment between two points p1 and p2. The line segment is represented by a cylinder and the arrow head by a cone.
    ///
    /// - parameter arrowHeight: height of the arrow head. Set this to 0 if no arrow head is needed.
    /// - parameter arrowRadius: width of the arrow head
    /// - parameter numSides: number of sides of the cylinder and the cone.
    /// - parameter color: color
    /// - parameter p1: starting point
    /// - parameter p2: finishing point (arrow head side)
    public init(

        arrowHeight : Float,
        arrowRadius : Float,
        lineRadius  : Float,
        numSides    : Int,
        color       : SIMD3<Float>,
        p1          : SIMD3<Float>,
        p2          : SIMD3<Float>
    ) {
        self.arrowHeight = arrowHeight
        self.arrowRadius = arrowRadius
        self.lineRadius  = lineRadius
        self.numSides    = numSides
        self.color       = color
        self.p1          = p1
        self.p2          = p2
    }

    public func generateMesh(indexBegin: inout Int32 )
         -> ( points: [SIMD3<Float>], normals: [SIMD3<Float>], colors: [SIMD3<Float>], indices: [Int32] )
    {
        let d = normalize( p2 - p1 )
        let dAnti = d * -1.0

        var far_from_d : SIMD3<Float>
        let dot_x = abs( dot(d, SIMD3<Float>( 1.0, 0.0, 0.0 ) ) )
        let dot_y = abs( dot(d, SIMD3<Float>( 0.0, 1.0, 0.0 ) ) )
        let dot_z = abs( dot(d, SIMD3<Float>( 0.0, 0.0, 1.0 ) ) )

        if dot_x <= dot_y && dot_x <= dot_z {
            far_from_d = SIMD3<Float>( 1.0, 0.0, 0.0 )
        }
        else if dot_y <= dot_z && dot_y <= dot_x {
            far_from_d = SIMD3<Float>( 0.0, 1.0, 0.0 )
        }
        else {
            far_from_d = SIMD3<Float>( 0.0, 0.0, 1.0 )
        }
        let t = normalize( cross( d, far_from_d ) )
        let b = normalize( cross( d, t ) )


        var nPrev = t

        var points  : [ SIMD3<Float> ] = []
        var normals : [ SIMD3<Float> ] = []
        var colors  : [ SIMD3<Float> ] = []
        var indices : [ Int32        ] = []

        for i in 1 ... numSides {

            let rad = Float(i)/Float(numSides) * 2.0 * Float.pi
            let nCur = (t * cos(rad)) + (b * sin(rad))
            
            let pPrev1 = nPrev * lineRadius + p1
            let pCur1  = nCur  * lineRadius + p1
            let pPrev2 = nPrev * lineRadius + p2
            let pCur2  = nCur  * lineRadius + p2

            // Sides

            points.append(pPrev1)
            points.append(pCur1)
            points.append(pCur2)
            points.append(pPrev2)

            normals.append(nPrev)
            normals.append(nCur)
            normals.append(nCur)
            normals.append(nPrev)

            colors.append(color)
            colors.append(color)
            colors.append(color)
            colors.append(color)

            indices.append(indexBegin)
            indices.append(indexBegin + 1)
            indices.append(indexBegin + 2)
            indices.append(indexBegin)
            indices.append(indexBegin + 2)
            indices.append(indexBegin + 3)
            indexBegin += 4

            // Bottom

            points.append(pCur1)
            points.append(pPrev1)
            points.append(p1)

            normals.append( dAnti )
            normals.append( dAnti )
            normals.append( dAnti )

            colors.append(color)
            colors.append(color)
            colors.append(color)

            indices.append(indexBegin)
            indices.append(indexBegin + 1)
            indices.append(indexBegin + 2)

            indexBegin += 3

            // Top

            if arrowHeight > 0.0 {

                let aPrev = nPrev * arrowRadius + p2
                let aCur  = nCur  * arrowRadius + p2
                let aTip  = d     * arrowHeight + p2

                let nArrowPrev = normalize ( cross ( cross( aTip - aPrev, nPrev ), aTip - aPrev ) )
                let nArrowCur  = normalize ( cross ( cross( aTip - aCur,  nCur  ), aTip - aCur  ) )
                let nArrowTip  = normalize ( nArrowPrev + nArrowCur )

                points.append(aPrev)
                points.append(aCur)
                points.append(p2)

                normals.append( dAnti )
                normals.append( dAnti )
                normals.append( dAnti )

                colors.append(color)
                colors.append(color)
                colors.append(color)

                indices.append(indexBegin)
                indices.append(indexBegin + 1)
                indices.append(indexBegin + 2)

                indexBegin += 3

                points.append(aPrev)
                points.append(aCur)
                points.append(aTip)

                normals.append( nArrowPrev )
                normals.append( nArrowCur  )
                normals.append( nArrowTip  )

                colors.append(color)
                colors.append(color)
                colors.append(color)

                indices.append(indexBegin)
                indices.append(indexBegin + 1)
                indices.append(indexBegin + 2)

                indexBegin += 3
            }
            else {

                points.append(pPrev2)
                points.append(pCur2)
                points.append(p2)

                normals.append( d )
                normals.append( d )
                normals.append( d )

                colors.append(color)
                colors.append(color)
                colors.append(color)

                indices.append(indexBegin)
                indices.append(indexBegin + 1)
                indices.append(indexBegin + 2)

                indexBegin += 3
            }

            nPrev = nCur
        }

        return ( points: points, normals:normals, colors: colors, indices: indices )
    }
}

/// container of lines and arrows to generate MDLMesh collectively
class LinesAndArrows {

    public var elements: [LineOrArrow] = []

    /// generates MDLMesh for the arrows and lines
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

        for element in elements {

            let arrays = element.generateMesh( indexBegin: &vertexIndex )

            structureOfArrays.vertices!.append    ( contentsOf: arrays.points  )
            structureOfArrays.normals!.append     ( contentsOf: arrays.normals )
            structureOfArrays.colors!.append      ( contentsOf: arrays.colors  )
            structureOfArrays.arrayIndices.append ( contentsOf: arrays.indices )
        }

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
            structureOfArrays:      structureOfArrays
        )

        return mdlMesh
    }
}

