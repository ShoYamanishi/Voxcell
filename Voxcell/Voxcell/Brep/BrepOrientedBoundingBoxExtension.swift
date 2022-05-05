import Foundation

extension Brep {

    static public func findOrientedBoundingBox( vertices : [ SIMD3<Float> ] ) -> Brep {

        let p : UnsafePointer< SIMD3<Float> >? = vertices.withUnsafeBufferPointer( { ptr in return ptr.baseAddress ?? nil})

        let mObjc : ManifoldObjc = ManifoldObjc()

        mObjc.findOrientedBoundingBox( p, numPoints: Int32(vertices.count) )

        return constructFeaturesFromManifoldObjc( mObjc );
    }

}

