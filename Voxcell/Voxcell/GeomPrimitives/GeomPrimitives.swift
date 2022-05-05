import Foundation

public class GeomPrimitives {

    static public func EigenVectorsIfSymmetric( mat: float3x3 ) -> ( eigenVectors: float3x3, eigenValues: SIMD3<Float> ) {

        let primitive = PrimitivesObjc()

        primitive!.eigenVectorsIfSymmetric( mat )

        let eigenVectors = float3x3(

            [ primitive!.eigenVectorMatrixRow(0, col: 0),
              primitive!.eigenVectorMatrixRow(1, col: 0),
              primitive!.eigenVectorMatrixRow(2, col: 0) ],

            [ primitive!.eigenVectorMatrixRow(0, col: 1),
              primitive!.eigenVectorMatrixRow(1, col: 1),
              primitive!.eigenVectorMatrixRow(2, col: 1) ],

            [ primitive!.eigenVectorMatrixRow(0, col: 2),
              primitive!.eigenVectorMatrixRow(1, col: 2),
              primitive!.eigenVectorMatrixRow(2, col: 2) ]
        )

        let eigenValues = SIMD3<Float>(
            x: primitive!.eigenValue(0),
            y: primitive!.eigenValue(1),
            z: primitive!.eigenValue(2)
        )

        return ( eigenVectors: eigenVectors, eigenValues: eigenValues )
    }
}
