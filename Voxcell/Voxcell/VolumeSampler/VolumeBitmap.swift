import Foundation
import simd

class LineBitmap {

    var line : [UInt]

    public init( size: Int ) {
      
        let dim = (size + MemoryLayout<UInt>.stride - 1 ) / MemoryLayout<UInt>.stride

        line = [UInt]( repeating: 0, count: dim )
    }

    public func isSet( z: Int ) -> Bool {

        let index  = z / MemoryLayout<UInt>.stride
        let offset = z % MemoryLayout<UInt>.stride

        let mask : UInt = UInt(1) << offset

        return ( line[index] & mask ) != 0
    }

    public func isNotSet( z: Int ) -> Bool {

        return !isSet( z: z )
    }

    public func set( z: Int ) -> Void {

        let index  = z / MemoryLayout<UInt>.stride
        let offset = z % MemoryLayout<UInt>.stride

        let mask : UInt = UInt(1) << offset

        line[index] = line[index] | mask
    }

}

public class VolumeBitmap {

    var gridInfo: GridInfo
    var bitmap:   [ [ LineBitmap ] ]

    // MARK: public functions

    /// constructs a 3D grid arrays whose elements are bits for the given dimension.
    /// - parameter gridInfo: information about the dimension of the grid.
    public init( gridInfo: GridInfo ) {

        self.gridInfo = gridInfo

        self.bitmap = []
        for _ in 0 ..< gridInfo.dimension.x {

            var yzPlane : [LineBitmap] = []

            for _ in 0 ..< gridInfo.dimension.y {

                let zLine = LineBitmap( size: Int(gridInfo.dimension.z) )
                yzPlane.append(zLine)
            }
            bitmap.append(yzPlane)
        }
    }

    /// returns true if the bit for the given integer coordinate is set.
    /// - parameter x: x-coordinate
    /// - parameter y: y-coordinate
    /// - parameter z: z-coordinate
    /// - returns true if the bit is set.
    public func isSet( x: Int, y: Int, z: Int ) -> Bool {
    
        return bitmap[x][y].isSet( z: z )
    }

    /// returns true if the bit for the given integer coordinate is not set.
    /// - parameter x: x-coordinate
    /// - parameter y: y-coordinate
    /// - parameter z: z-coordinate
    /// - returns true if the bit is not set.
    public func isNotSet( x: Int, y: Int, z: Int ) -> Bool {
    
        return bitmap[x][y].isNotSet( z: z )
    }

    /// shrinks the size of the grid by one grid pitch along all the three axes.
    ///
    /// Assume the current extent in the x-axis is [-3.0, 5.0] and the pitch is 1.0.
    /// The number of cells along the x-axis  (dimension of x) is 8. After the shrinking,
    /// The extent will be [-2.5, 4.5], and the dimension becomes 7. The pitch does not change.
    /// Conceptually, the points that corresponded to the vertices of the cubes become
    ///  center point of the cubes in the new shrunk grid.
    /// The bit in the new grid is set if the bits in all the 8 surrounding cells are set (except for the boundary cells).
    /// for example the bit at the integer coordinate (3,4,5) in the new grid is set only if the bits in the following 8
    /// integer coordinates in the old grid are set:
    /// (3,4,5), (4,4,5), (3,5,5), (4,5,5), (3,4,6), (4,4,6), (3,5,6), (4,5,6).
    /// This can be useful for collision detectors where it is desirable for the collision spheres to be completely inside the
    /// original 3D object.
    public func shrinkByHalfStep() -> Void {

        if gridInfo.dimension.x > 1 && gridInfo.dimension.y > 1 && gridInfo.dimension.z > 1 {

            let newGridInfo = self.gridInfo.shrinkByOnePitch()

            var newBitmap: [ [ LineBitmap ] ] = []

            for _ in 0 ..< newGridInfo.dimension.x {

                var yzPlane : [LineBitmap] = []

                for _ in 0 ..< newGridInfo.dimension.y {

                    let zLine = LineBitmap( size: newGridInfo.dimension.z )
                    yzPlane.append(zLine)
                }
                newBitmap.append(yzPlane)
            }

            for x in 0 ..< gridInfo.dimension.x - 1 {

                for y in 0 ..< gridInfo.dimension.y - 1 {

                    for z in 0 ..< gridInfo.dimension.z - 1 {

                        var adjacentCount = 0
                        if bitmap[x  ][y  ].isSet( z: z   ) { adjacentCount += 1 }
                        if bitmap[x  ][y  ].isSet( z: z+1 ) { adjacentCount += 1 }
                        if bitmap[x  ][y+1].isSet( z: z   ) { adjacentCount += 1 }
                        if bitmap[x  ][y+1].isSet( z: z+1 ) { adjacentCount += 1 }
                        if bitmap[x+1][y  ].isSet( z: z   ) { adjacentCount += 1 }
                        if bitmap[x+1][y  ].isSet( z: z+1 ) { adjacentCount += 1 }
                        if bitmap[x+1][y+1].isSet( z: z   ) { adjacentCount += 1 }
                        if bitmap[x+1][y+1].isSet( z: z+1 ) { adjacentCount += 1 }

                        if adjacentCount >= 4 {
                            newBitmap[x][y].set( z: z )
                        }
                    }
                }
            }
            
            bitmap   = newBitmap
            gridInfo = newGridInfo
        }
    }
    
    /// finds the integer coordinates of the cells that are at the boundary of the voxelized volume.
    ///
    /// - returns array of integer coordinates in the grid.
    ///
    /// A cell is at the boundary if not all of its 6 orthogonally adjacent cells are set.
    public func findShell() -> [SIMD3<Int32>] {

        var array: [SIMD3<Int32>] = []

        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y {

                for z in 0 ..< gridInfo.dimension.z {

                    if bitmap[x][y].isSet( z: z ) {

                        if    x == 0 || x == gridInfo.dimension.x - 1
                           || y == 0 || y == gridInfo.dimension.y - 1
                           || z == 0 || z == gridInfo.dimension.z - 1 {
                        
                            array.append( SIMD3<Int32>( x:Int32(x), y:Int32(y), z:Int32(z)) )
                        }
                        else if    bitmap[x  ][y  ].isNotSet( z: z+1 )
                                || bitmap[x  ][y  ].isNotSet( z: z-1 )
                                || bitmap[x  ][y+1].isNotSet( z: z   )
                                || bitmap[x  ][y-1].isNotSet( z: z   )
                                || bitmap[x-1][y  ].isNotSet( z: z   )
                                || bitmap[x+1][y  ].isNotSet( z: z   )
                        {
                            array.append( SIMD3<Int32>( x:Int32(x), y:Int32(y), z:Int32(z)) )
                        }
                    }
                }
            }
        }
        return array
    }
    
    /// finds the 3D spatial coordinates of the center point of the cells that are at the boundary of the voxelized volume.
    ///
    /// - returns array of geometric coordinates in the grid.
    ///
    /// This is useful for generating collision spheres of the 3D object for collision detection
    /// A cell is at the boundary if not all of its 6 orthogonally adjacent cells are set.
    public func findShellCenterPoints() -> [SIMD3<Float>] {

        var array: [SIMD3<Float>] = []

        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y {

                for z in 0 ..< gridInfo.dimension.z {

                    if bitmap[x][y].isSet( z: z ) {

                        if    x == 0 || x == gridInfo.dimension.x - 1
                           || y == 0 || y == gridInfo.dimension.y - 1
                           || z == 0 || z == gridInfo.dimension.z - 1 {
                        
                            array.append( gridInfo.centerPointFromGridCoords( x: x, y: y, z: z ) )
                        }
                        else if    bitmap[x  ][y  ].isNotSet( z: z+1 )
                                || bitmap[x  ][y  ].isNotSet( z: z-1 )
                                || bitmap[x  ][y+1].isNotSet( z: z   )
                                || bitmap[x  ][y-1].isNotSet( z: z   )
                                || bitmap[x-1][y  ].isNotSet( z: z   )
                                || bitmap[x+1][y  ].isNotSet( z: z   )
                        {
                            array.append( gridInfo.centerPointFromGridCoords( x: x, y: y, z: z ) )
                        }
                    }
                }
            }
        }
        return array
    }
    
    /// finds the total mass by sampling based on the volume represented by the bitmap.
    /// - parameter densityPerUnitVolume
    /// - returns Mass
    public func findMass( densityPerUnitVolume: Float ) -> Float {

        var count = 0
        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y {

                for z in 0 ..< gridInfo.dimension.z {

                    if bitmap[x][y].isSet( z: z ) {
                        count += 1
                    }
                }
            }
        }

        return Float(count) * densityPerUnitVolume * gridInfo.pitch * gridInfo.pitch * gridInfo.pitch
    }

    /// finds the center of mass by sampling based on the volume represented by the bitmap.
    /// - returns CoM in the 3D spatial coordinates.
    public func findCenterOfMass() -> SIMD3<Float> {

        var sum = SIMD3<Float>(0,0,0)
        var count = 0

        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y {

                for z in 0 ..< gridInfo.dimension.z {

                    if bitmap[x][y].isSet( z: z ) {

                        sum   += gridInfo.centerPointFromGridCoords( x:x, y:y, z:z )
                        count += 1
                    }
                }
            }
        }

        return SIMD3<Float>( x: sum.x / Float(count),  y: sum.y / Float(count),  z: sum.z / Float(count) )
    }

    /// finds the 3x3 inertia tensor by sampling based on the volume represented by the bitmap.
    /// - returns intertia tensor
    public func findInertiaTensor( densityPerUnitVolume density: Float ) -> float3x3 {
    
        let CoM = findCenterOfMass()

        var sum_yy_zz : Float = 0.0
        var sum_zz_xx : Float = 0.0
        var sum_xx_yy : Float = 0.0

        var sum_xy    : Float = 0.0
        var sum_yz    : Float = 0.0
        var sum_zx    : Float = 0.0

        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y {

                for z in 0 ..< gridInfo.dimension.z {

                    if bitmap[x][y].isSet( z: z ) {

                        let delta = gridInfo.centerPointFromGridCoords( x:x, y:y, z:z ) - CoM

                        sum_yy_zz += ( delta.y * delta.y + delta.z * delta.z )
                        sum_zz_xx += ( delta.z * delta.z + delta.x * delta.x )
                        sum_xx_yy += ( delta.x * delta.x + delta.y * delta.y )
                        sum_xy    += ( delta.x * delta.y )
                        sum_yz    += ( delta.y * delta.z )
                        sum_zx    += ( delta.z * delta.x )
                    }
                }
            }
        }

        let delta_mass = density * gridInfo.pitch * gridInfo.pitch * gridInfo.pitch

        let inertia = float3x3(

            [      delta_mass * sum_yy_zz,  -1.0 * delta_mass * sum_xy,  -1.0 * delta_mass * sum_zx  ],

            [  -1.0 * delta_mass * sum_xy,      delta_mass * sum_zz_xx,  -1.0 * delta_mass * sum_yz  ],

            [  -1.0 * delta_mass * sum_zx,  -1.0 * delta_mass * sum_yz,      delta_mass * sum_xx_yy  ]
        )

        return inertia
    }

    // MARK: private functions

    // Following functions are used to generate a volume bitmap from DepthPeeler

    func gridCoordfromDepth( depth: Float, direction: CastDirection ) -> Int {

        if direction == .CD_X_POSITIVE || direction == .CD_X_POSITIVE  {
            return gridInfo.gridCoordfromPointX( x: depth )
        }
        else if direction == .CD_Y_POSITIVE || direction == .CD_Y_POSITIVE  {
            return gridInfo.gridCoordfromPointY( y: depth )
        }
        else {
            return gridInfo.gridCoordfromPointZ( z: depth )
        }
    }

    func updateAlongZFrom( sweepLinesAlongZ: SweepLines ) -> Void {

        for x in 0 ..< gridInfo.dimension.x {

            for y in 0 ..< gridInfo.dimension.y  {

                let xy = IntegerPairKey<Int16>( v1: Int16(x), v2: Int16(y) )

                if let sweepLine = sweepLinesAlongZ[xy] {

                    var enteringZ = -1

                    for result in sweepLine {

                        let gridCoordZ = gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Z_POSITIVE )

                        if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {

                            enteringZ = gridCoordZ
                        }
                        else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                            if enteringZ != -1 {

                                if enteringZ < gridCoordZ {

                                    for z in enteringZ ... gridCoordZ {

                                        bitmap[x][y].set( z: z )
                                    }
                                } else {

                                    for z in gridCoordZ ... enteringZ {

                                        bitmap[x][y].set( z: z )
                                    }
                                }
                                enteringZ = -1
                            }
                        }
                    }
                }
            }
        }
    }

    func updateAlongXFrom( sweepLinesAlongX: SweepLines ) -> Void {

        for y in 0 ..< gridInfo.dimension.y {

            for z in 0 ..< gridInfo.dimension.z  {

                let yz = IntegerPairKey<Int16>( v1: Int16(y), v2: Int16(z) )

                if let sweepLine = sweepLinesAlongX[yz] {

                    var enteringX = -1

                    for result in sweepLine {

                        let gridCoordX = gridCoordfromDepth( depth: result.depthCoord, direction: .CD_X_POSITIVE )
  
                        if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {

                            enteringX = gridCoordX
                        }
                        else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                            if enteringX != -1 {
        
                                if enteringX < gridCoordX {

                                    for x in enteringX ... gridCoordX {
        
                                        bitmap[x][y].set( z: z )
                                    }
                                } else {

                                    for x in gridCoordX ... enteringX {

                                        bitmap[x][y].set( z: z )
                                    }
                                }
                                enteringX = -1
                            }
                        }
                    }
                }
            }
        }
    }

    func updateAlongYFrom( sweepLinesAlongY: SweepLines ) -> Void {

        for z in 0 ..< gridInfo.dimension.z {

            for x in 0 ..< gridInfo.dimension.x  {

                let zx = IntegerPairKey<Int16>( v1: Int16(z), v2: Int16(x) )

                if let sweepLine = sweepLinesAlongY[zx] {

                    var enteringY = -1

                    for result in sweepLine {

                        let gridCoordY = gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Y_POSITIVE )

                        if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {

                            enteringY = gridCoordY
                        }
                        else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                            if enteringY != -1 {

                                if enteringY < gridCoordY {

                                    for y in enteringY ... gridCoordY {

                                        bitmap[x][y].set( z: z )
                                    }
                                } else {

                                    for y in gridCoordY ... enteringY {

                                        bitmap[x][y].set( z: z )
                                    }
                                }
                                enteringY = -1
                            }
                        }
                    }
                }
            }
        }
    }
}
