import Foundation

public struct GridInfo {

    public let extentMin: SIMD3<Float>
    public let extentMax: SIMD3<Float>
    public let dimension: SIMD3<Int>
    public let pitch: Float

    /// initialize the grid information for 3D space
    ///
    /// - parameter extentMin : lowest point of the grid
    /// - parameter extentMax : highest point of the grid
    /// - parameter pitch : grid pitch. It is the edge length of each cell (cube) in the grid
    ///
    /// after the initialization, `extentMin` and `extentMax` may be adjusted such that
    ///  `(0,0,0)` coresponds to `extentMin` and` (dimension.x, dimension.y, dimension.z)` corresponds to `extentMax`.
    public init( extentMin: SIMD3<Float>, extentMax: SIMD3<Float>, pitch: Float ) {

        var min = extentMin
        var max = extentMax

        let width_x = extentMax.x - extentMin.x
        let numCells_x = ceil( width_x / pitch )
        let margin_x = numCells_x * pitch - width_x;
        max.x += (margin_x / 2.0)
        min.x -= (margin_x / 2.0)

        let width_y = extentMax.y - extentMin.y
        let numCells_y = ceil( width_y / pitch )
        let margin_y = numCells_y * pitch - width_y;
        max.y += (margin_y / 2.0)
        min.y -= (margin_y / 2.0)

        let width_z = extentMax.z - extentMin.z
        let numCells_z = ceil( width_z / pitch )
        let margin_z = numCells_z * pitch - width_z;
        max.z += (margin_z / 2.0)
        min.z -= (margin_z / 2.0)

        self.extentMin = min
        self.extentMax = max
        self.dimension = SIMD3<Int>(x: Int(numCells_x), y: Int(numCells_y), z: Int(numCells_z) )
        self.pitch = pitch
    }

    /// returns the point in the 3D space that corresponds to the given integer coordinates in the grid.
    ///
    /// - parameter x: interger x-coordinate in the grid between 0, dimension.x - 1
    /// - parameter y: interger x-coordinate in the grid between 0, dimension.y - 1
    /// - parameter z: interger x-coordinate in the grid between 0, dimension.z - 1
    /// - returns the point in the 3D space.
    ///  `(0,0,0)` coresponds to `extentMin` and` (dimension.x - 1, dimension.y - 1, dimension.z - 1)` corresponds to `extentMax`.
    public func gridPointFromGridCoords( x: Int, y: Int, z: Int) -> SIMD3<Float> {

        return SIMD3<Float>( x: Float(x) * pitch + extentMin.x,
                             y: Float(y) * pitch + extentMin.y,
                             z: Float(z) * pitch + extentMin.z  )
    }

    /// returns the center point of the grid cell in the 3D space that corresponds to the given integer coordinates in the grid.
    ///
    /// - parameter x: interger x-coordinate in the grid between 0, dimension.x - 1
    /// - parameter y: interger x-coordinate in the grid between 0, dimension.y - 1
    /// - parameter z: interger x-coordinate in the grid between 0, dimension.z - 1
    /// - returns the point in the 3D space.
    ///  `(0,0,0)` coresponds to `extentMin + (pitch * 0.5, pitch * 0.5, pitch * 0.5 )`.
    public func centerPointFromGridCoords( x: Int, y: Int, z: Int) -> SIMD3<Float> {

        return SIMD3<Float>( x: ( Float(x) + 0.5 ) * pitch + extentMin.x,
                             y: ( Float(y) + 0.5 ) * pitch + extentMin.y,
                             z: ( Float(z) + 0.5 ) * pitch + extentMin.z  )
    }


    /// returns the integer grid coordinates from the point in the 3D space
    ///
    /// - parameter point: the point in the 3D space
    /// - returns the grid coordinates rounded down.
    public func gridCoordfromPoint( point: SIMD3<Float> ) -> SIMD3<Int> {

        let x = max( min( Int( (point.x - extentMin.x) / pitch ), Int(dimension.x) ), 0 )
        let y = max( min( Int( (point.y - extentMin.y) / pitch ), Int(dimension.y) ), 0 )
        let z = max( min( Int( (point.z - extentMin.z) / pitch ), Int(dimension.z) ), 0 )

        return SIMD3<Int>( x:x, y:y, z:z )
    }

    public func gridCoordfromPointX( x: Float ) -> Int {

        return max( min( Int( ( x - extentMin.x) / pitch ), Int(dimension.x) ), 0 )
    }

    public func gridCoordfromPointY( y: Float ) -> Int {

        return max( min( Int( ( y - extentMin.y) / pitch ), Int(dimension.y) ), 0 )
    }

    public func gridCoordfromPointZ( z: Float ) -> Int {

        return max( min( Int( ( z - extentMin.z) / pitch ), Int(dimension.z) ), 0 )
    }

    public func dimensionUInt32() -> SIMD3<UInt32> {
        return SIMD3<UInt32>( x: UInt32(dimension.x), y: UInt32(dimension.y), z: UInt32(dimension.z) )
    }

    /// returns a new grid information based on the current grid info by shrinking the grid by one cell in all the 3 axes.
    /// - returns new grid information whose size in 3D space is smaller by 1 pitch in all the 3 axes.

    public func shrinkByOnePitch() -> GridInfo {

        // NOTE: The reason for 0.55 instead of 0.5 is to add some perturbation (0.05)
        // to avoid degeneracy due to numerical error.
        let newExtentMin = SIMD3<Float>(
            x: extentMin.x + pitch * 0.55,
            y: extentMin.y + pitch * 0.55,
            z: extentMin.z + pitch * 0.55
        )

        let newExtentMax = SIMD3<Float>(
            x: extentMax.x - pitch * 0.55,
            y: extentMax.y - pitch * 0.55,
            z: extentMax.z - pitch * 0.55
        )

        return GridInfo( extentMin: newExtentMin, extentMax: newExtentMax, pitch: pitch )
    }

    /// makes a grid info based on the given points and the grid pitch such that the grid contains all the points.
    /// - parameter arrayOfPoints
    /// - parameter pitch
    /// - returns new grid information
    public static func generateGridInfoFrom( arrayOfPoints : [SIMD3<Float>], pitch: Float ) -> GridInfo {

        var extentMax :SIMD3<Float> = arrayOfPoints[0]
        var extentMin :SIMD3<Float> = arrayOfPoints[0]

        for v in arrayOfPoints {
            extentMax.x = max( v.x, extentMax.x )
            extentMax.y = max( v.y, extentMax.y )
            extentMax.z = max( v.z, extentMax.z )

            extentMin.x = min( v.x, extentMin.x )
            extentMin.y = min( v.y, extentMin.y )
            extentMin.z = min( v.z, extentMin.z )
        }

        extentMin = SIMD3<Float>( extentMin.x, extentMin.y, extentMin.z )
        extentMax = SIMD3<Float>( extentMax.x, extentMax.y, extentMax.z )

        return GridInfo( extentMin: extentMin, extentMax: extentMax, pitch: pitch )
    }
}
