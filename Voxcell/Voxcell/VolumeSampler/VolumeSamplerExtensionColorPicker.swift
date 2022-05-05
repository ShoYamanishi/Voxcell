import Foundation

/// correction of routines for QuadGenerationHelper to find an appropriate color based on the sampled colors in the SweepLines.
extension VolumeSampler {

    func pickColorFromSweepLinesX( x: Int, y: Int, z: Int, compensate: Bool, relax: Bool ) -> ( Bool, SIMD3<Float> ) {

        let uv = IntegerPairKey<Int16>( v1: Int16(y), v2: Int16(z) )

        var enteringX : Int = -1

        if sweepLinesYZ?[uv] != nil {

            if relax {
                var closestDist = volumeBitmap!.gridInfo.dimension.x
                var closestColor = SIMD3<Float>(0,0,0)

                for result in sweepLinesYZ![uv]! {

                    let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_X_POSITIVE )
                
                    let dist = abs( gridCoord - x )
                    if dist < closestDist {
                        closestDist = dist
                        closestColor = result.color
                    }
                }
                return (true, closestColor)
            }

            for result in sweepLinesYZ![uv]! {

                let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_X_POSITIVE )

                if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {
                    if gridCoord == x {
                        return (true, result.color)
                    }
                    enteringX = gridCoord
                }
                else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                    if enteringX != -1 {

                        if enteringX < gridCoord {
                            for e in enteringX ... gridCoord {
                                if e == x {
                                    return (true, result.color)
                                }
                            }
                        } else {
                            for e in gridCoord ... enteringX {
                                if e == x {
                                    return (true, result.color)
                                }
                            }
                        }
                        enteringX = -1
                    }
                }
            }
        }

        if compensate {
        
            let rtnY = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnY.0 {
                return rtnY
            }

            let rtnZ = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnZ.0 {
                return rtnZ
            }

            let rtnX = pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnX.0 {
                return rtnX
            }

            let rtnYrelaxed = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnYrelaxed.0 {
                return rtnYrelaxed
            }

            let rtnZrelaxed = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnZrelaxed.0 {
                return rtnZrelaxed
            }
        }
        
        return (false, SIMD3<Float>(0.5,0.5,0.5))

    }

    func pickColorFromSweepLinesY( x: Int, y: Int, z: Int, compensate: Bool, relax: Bool ) -> (Bool, SIMD3<Float>) {

        let uv = IntegerPairKey<Int16>( v1: Int16(z), v2: Int16(x) )

        var enteringY : Int = -1

        if sweepLinesZX?[uv] != nil {

            if relax {

                var closestDist = volumeBitmap!.gridInfo.dimension.y
                var closestColor = SIMD3<Float>(0,0,0)

                for result in sweepLinesZX![uv]! {

                    let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Y_POSITIVE )

                    let dist = abs( gridCoord - x )
                    if dist < closestDist {
                        closestDist = dist
                        closestColor = result.color
                    }
                }
                return (true, closestColor)
            }

            for result in sweepLinesZX![uv]! {

                let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Y_POSITIVE )

                if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {
                    if gridCoord == y {
                        return (true, result.color)
                    }
                    enteringY = gridCoord
                }
                else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                    if enteringY != -1 {

                        if enteringY < gridCoord {
                            for e in enteringY ... gridCoord {
                                if e == y {
                                    return (true, result.color)
                                }
                            }
                        } else {
                            for e in gridCoord ... enteringY {
                                if e == y {
                                    return (true, result.color)
                                }
                            }
                        }
                        enteringY = -1
                    }
                }
            }
        }
        
        if compensate {

            let rtnZ = pickColorFromSweepLinesZ( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnZ.0 {
                return rtnZ
            }

            let rtnX = pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnX.0 {
                return rtnX
            }
            
            let rtnY = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnY.0 {
                return rtnY
            }

            let rtnZrelaxed = pickColorFromSweepLinesZ( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnZrelaxed.0 {
                return rtnZrelaxed
            }

            let rtnXrelaxed = pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnXrelaxed.0 {
                return rtnXrelaxed
            }
        }
        return (false, SIMD3<Float>(0.5,0.5,0.5))
    }

    func pickColorFromSweepLinesZ( x: Int, y: Int, z: Int, compensate: Bool, relax: Bool ) -> (Bool, SIMD3<Float>) {

        let uv = IntegerPairKey<Int16>( v1: Int16(x), v2: Int16(y) )

        var enteringZ : Int = -1

        if sweepLinesXY?[uv] != nil {

            if relax {

                var closestDist = volumeBitmap!.gridInfo.dimension.z
                var closestColor = SIMD3<Float>(0,0,0)

                for result in sweepLinesXY![uv]! {

                    let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Z_POSITIVE )

                    let dist = abs( gridCoord - x )
                    if dist < closestDist {
                        closestDist = dist
                        closestColor = result.color
                    }
                }
                return (true, closestColor)
            }

            for result in sweepLinesXY![uv]! {

                let gridCoord = volumeBitmap!.gridCoordfromDepth( depth: result.depthCoord, direction: .CD_Z_POSITIVE )

                if result.resultType == DepthPeelingResultType.DPRT_ENTERING_VOLUME.rawValue {

                    if gridCoord == z {
                        return (true, result.color)
                    }
                    enteringZ = gridCoord
                }
                else if result.resultType == DepthPeelingResultType.DPRT_LEAVING_VOLUME.rawValue {

                    if enteringZ != -1 {

                        if enteringZ < gridCoord {
                            for e in enteringZ ... gridCoord {
                                if e == z {
                                    return (true, result.color)
                                }
                            }
                        } else {
                            for e in gridCoord ... enteringZ {
                                if e == z {
                                    return (true, result.color)
                                }
                            }
                        }
                        enteringZ = -1
                    }
                }
            }
        }

        if compensate {

            let rtnX = pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnX.0 {
                return rtnX
            }

            let rtnY = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: false )
            if rtnY.0 {
                return rtnY
            }
            
            let rtnZ = pickColorFromSweepLinesZ( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnZ.0 {
                return rtnZ
            }

            let rtnXrelaxed = pickColorFromSweepLinesX( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnXrelaxed.0 {
                return rtnXrelaxed
            }

            let rtnYrelaxed = pickColorFromSweepLinesY( x: x, y: y, z: z, compensate: false, relax: true )
            if rtnYrelaxed.0 {
                return rtnYrelaxed
            }
        }
        return (false, SIMD3<Float>(0.5,0.5,0.5))
    }

}
