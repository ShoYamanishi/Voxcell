import Foundation
import MetalKit

/// Systemwide storage and mapping from the string name to MTLTexture
class TextureStorage {

    public var mapping : [ String : MTLTexture ] = [:]
    public let device  : MTLDevice


    /// initializes the mapping
    /// - parameter device : Metal device
    public init ( device : MTLDevice ) {
        self.device = device
    }

    /// load and store textures found in the given MDLMaterial
    ///
    ///  - parameter material : MDLMaterial
    ///  
    ///  if looks for items that match the following criteria:
    ///        - `MDLMaterialSemantic == .baseColor` or `.tangentSpaceNormal` or `.roughness`
    ///        - `MDLMaterialPropertyType.string`
    ///    It searches in toe App's bundle and assets with the URL given in  `MDLMaterialProperty.stringValue`.
    /// It stores the generated MTLTexture with the URL as the key.
    public func findAndStoreTexturesIn( material : MDLMaterial ) {
        if let filename = material.textureNameForBaseColor() {
            let texture = TextureStorage.loadTexture( imageName: filename, device: device )
            mapping[ filename ] = texture
        }

        if let filename = material.textureNameForTangentSpaceNormal() {
            let texture = TextureStorage.loadTexture( imageName: filename, device: device )
            mapping[ filename ] = texture
        }

        if let filename = material.textureNameForRoughness() {
            let texture = TextureStorage.loadTexture( imageName: filename, device: device )
            mapping[ filename ] = texture
        }
    }
    
    static func loadTexture( imageName: String, device: MTLDevice ) -> MTLTexture? {

        let textureLoader = MTKTextureLoader( device: device )
    
        let textureLoaderOptions: [ MTKTextureLoader.Option: Any ]
            = [ .origin:          MTKTextureLoader.Origin.bottomLeft,
                .SRGB:            false,
                .generateMipmaps: NSNumber(booleanLiteral: true) ]

        let fileExtension
            = URL( fileURLWithPath: imageName ).pathExtension.isEmpty ? "png" : nil
        do {
            guard let url = Bundle.main.url( forResource: imageName, withExtension: fileExtension )
            else {
                let texture = try? textureLoader.newTexture(
                    name:        imageName,
                    scaleFactor: 1.0,
                    bundle:      Bundle.main,
                    options:     nil
                )

                if texture != nil {

                    print( "loaded: \(imageName) from asset catalog" )

                } else {

                    print("Texture not found: \(imageName)")
                }
                return texture
            }
    
            let texture = try textureLoader.newTexture( URL: url, options: textureLoaderOptions )
       
            print( "loaded texture: \(url.lastPathComponent)" )
      
            return texture
        } catch {
            print("Texture not found: \(imageName)")
            return nil
        }
    }
}
