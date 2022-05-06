//
//  MDLMaterialExtensionTexture.swift
//  Voxell
//
//  Created by Shoichiro Yamanishi on 01.05.22.
//

import Foundation
import MetalKit

extension MDLMaterial {

    public func baseColor( ifNotFound : SIMD3<Float> ) -> SIMD3<Float> {
        if let val = float3For( name : .baseColor ) {
           return val
        }
        else {
            return ifNotFound
        }
    }

    public func specular( ifNotFound : SIMD3<Float> ) -> SIMD3<Float> {
        if let val = float3For( name : .specular ) {
           return val
        }
        else {
            return ifNotFound
        }
    }

    public func specularExponent( ifNotFound : Float ) -> Float {
        if let val = floatFor( name : .specularExponent ) {
            return val
        }
        else {
            return ifNotFound
        }
    }

    public func roughness( ifNotFound : SIMD3<Float> ) -> SIMD3<Float> {
        if let val = float3For( name : .roughness ) {
            return val
        }
        else {
            return ifNotFound
        }
    }

    public func opacity( ifNotFound : Float ) -> Float {
        if let val = floatFor( name : .opacity ) {
            return val
        }
        else {
            return ifNotFound
        }
    }

    public func textureNameForBaseColor() -> String? {
        return textureNameFor( name : MDLMaterialSemantic.baseColor )
    }

    public func textureNameForTangentSpaceNormal() -> String? {
        return textureNameFor( name : MDLMaterialSemantic.tangentSpaceNormal )
    }

    public func textureNameForRoughness() -> String? {
        return textureNameFor( name : MDLMaterialSemantic.roughness )
    }

    func textureNameFor( name : MDLMaterialSemantic ) -> String? {
        for i in 0 ..< self.count {
            let item = self[i]

            if    item!.semantic == name
               && item!.type     == MDLMaterialPropertyType.string
            {
                return item!.stringValue!
            }
        }
        return nil
    }
    
    func floatFor( name : MDLMaterialSemantic ) -> Float? {
        for i in 0 ..< self.count {
            let item = self[i]

            if    item!.semantic == name
               && item!.type     == .float
            {
                return item!.floatValue
            }
        }
        return nil
    }

    func float3For( name : MDLMaterialSemantic ) -> SIMD3<Float>? {
        for i in 0 ..< self.count {
            let item = self[i]

            if    item!.semantic == name
               && item!.type     == .float3
            {
                return item!.float3Value
            }
        }
        return nil
    }

}
