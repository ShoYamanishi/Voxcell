import Foundation

extension Brep {

    /// Constructs a convex hull from the given list of points
    ///
    /// - Current the construction depends on the c++ implementation found in `manifold_convex_hull.cpp`
    /// - If the given set of points does not form at least 3-simplex, it returns an empty Brep with no vertex, no faces, and no half-edges.
    /// - Algorithm
    ///    1. Peform the principal component analysis.
    ///    2. Find the 2 extremal points along the primary axis, and other 2 extremal points along the secondary axis.
    ///    3. Form an initial 3-simplex from the 4 points.
    ///    4. For the rest of the points, incrementally test each point according to *Clarkson & Shor* algorithm described in *de Berg, van Kreveld, Overmars, & Cheong(Schwarzkopf)*.
    ///
    ///
    /// - parameter vertices: array of points
    /// - returns: A Brep that represents the convex hull.
    ///
    /// - seealso : Ref1: K.L. Clarkson, P. W. Schor. Applications of random sampling in computational geometry II, Discrete Comput. Gem. 4: 387-421, 1989
    /// - seealso : Ref2 : de Berg, Cheong, van Kreveld, & Overmars, Computational Geometry, Springer-Verlag, 2008, 978-3-540-77973-5
    ///

    static public func findConvexHull( vertices : [ SIMD3<Float> ] ) -> Brep {

        let p : UnsafePointer< SIMD3<Float> >? = vertices.withUnsafeBufferPointer( { ptr in return ptr.baseAddress ?? nil})

        let mObjc : ManifoldObjc = ManifoldObjc()

        mObjc.findConvexHull( p, numPoints: Int32(vertices.count) )

        return constructFeaturesFromManifoldObjc( mObjc );
    }
}
