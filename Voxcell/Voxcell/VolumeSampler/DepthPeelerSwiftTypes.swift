import Foundation

// This is the Swift counterpart of DepthPeelerShadersTypes.h.

// struct Ray is required by MPSRayIntersector
// if rayDataType = MPSRayDataType.originMaskDirectionMaxDistance
// It is not defined anywhere by Metal.
struct Ray {

// MemoryLayout<Ray>.stride: 48
// MemoryLayout<Ray>.size: 48
// MemoryLayout<Ray>.alignment: 16

    var origin_x:    Float
    var origin_y:    Float
    var origin_z:    Float
    var mask:        UInt32
    var direction_x: Float
    var direction_y: Float
    var direction_z: Float
    var maxDistance: Float
    var color:       SIMD3<Float>

} // 48 bytes


// struct Intersection is required by MPSRayIntersector
// if intersectionDataType =  MPSIntersectionDataType.distancePrimitiveIndexCoordinates
// It is not defined anywhere by Metal.
struct Intersection {

    var distance:       Float
    var primitiveIndex: Int32
    var coordinates_u:  Float
    var coordinates_v:  Float
}


enum CastDirection: UInt32 {

    case CD_INVALID    = 0
    case CD_X_POSITIVE = 1
    case CD_X_NEGATIVE = 2
    case CD_Y_POSITIVE = 3
    case CD_Y_NEGATIVE = 4
    case CD_Z_POSITIVE = 5
    case CD_Z_NEGATIVE = 6
}

struct UniformRayIntersector {

    var extentMin:     SIMD3<Float>
    var extentMax:     SIMD3<Float>
    var gridDimension: SIMD3<UInt32>
    var castDirection: UInt32
    var epsilon:       Float
    var defaultColor:  SIMD3<Float>

    public init() {

       extentMin      = SIMD3<Float>(0,0,0)
       extentMax      = SIMD3<Float>(0,0,0)
       gridDimension  = SIMD3<UInt32>(0,0,0)
       castDirection  = CastDirection.CD_INVALID.rawValue
       epsilon        = 0.0
       defaultColor   = SIMD3<Float>(0.5,0.5,0.5)
    }
}

public enum DepthPeelingResultType: UInt32 {

    case DPRT_NONE            = 0
    case DPRT_ENTERING_VOLUME = 1
    case DPRT_LEAVING_VOLUME  = 2
}

public struct DepthPeelingResult {

    public var resultType:    UInt32
    public var triangleIndex: Int32
    public var depthCoord:    Float
    public var color:         SIMD3<Float>
}
