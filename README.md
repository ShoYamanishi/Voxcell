# Voxcell
A XCFramework for iOS and macOS to generate voxel, convex hull, etc. from MDLMesh

<a href="figs/moo_org.PNG" ><img src="figs/moo_org.PNG" width="100"></a>
<a href="figs/moo_voxel_01.PNG" ><img src="figs/moo_voxel_01.PNG" width="100"></a>
<a href="figs/moo_voxel_02.PNG" ><img src="figs/moo_voxel_02.PNG" width="100"></a>
<a href="figs/moo_voxel_03.PNG" ><img src="figs/moo_voxel_03.PNG" width="100"></a>
<a href="figs/moo_voxel_04.PNG" ><img src="figs/moo_voxel_04.PNG" width="100"></a>
<a href="figs/moo_voxel_05.PNG" ><img src="figs/moo_voxel_05.PNG" width="100"></a>
<a href="figs/moo_voxel_06.PNG" ><img src="figs/moo_voxel_06.PNG" width="100"></a>

<a href="figs/moo_ch.PNG" ><img src="figs/moo_ch.PNG"             width="100"></a>
<a href="figs/moo_obb.PNG" ><img src="figs/moo_obb.PNG"           width="100"></a>
<a href="figs/moo_inertia.PNG" ><img src="figs/moo_inertia.PNG"   width="100"></a>
<a href="figs/moo_balls_01.PNG" ><img src="figs/moo_balls_01.PNG" width="100"></a>
<a href="figs/moo_balls_02.PNG" ><img src="figs/moo_balls_02.PNG" width="100"></a>
<a href="figs/moo_balls_03.PNG" ><img src="figs/moo_balls_03.PNG" width="100"></a>
<a href="figs/moo_balls_04.PNG" ><img src="figs/moo_balls_04.PNG" width="100"></a>

<a href="figs/quack_org.PNG" ><img src="figs/quack_org.PNG" width="100"></a>
<a href="figs/quack_voxel_01.PNG" ><img src="figs/quack_voxel_01.PNG" width="100"></a>
<a href="figs/quack_voxel_02.PNG" ><img src="figs/quack_voxel_02.PNG" width="100"></a>
<a href="figs/quack_voxel_03.PNG" ><img src="figs/quack_voxel_03.PNG" width="100"></a>
<a href="figs/quack_voxel_04.PNG" ><img src="figs/quack_voxel_04.PNG" width="100"></a>
<a href="figs/quack_voxel_05.PNG" ><img src="figs/quack_voxel_05.PNG" width="100"></a>
<a href="figs/quack_voxel_06.PNG" ><img src="figs/quack_voxel_06.PNG" width="100"></a>

<a href="figs/quack_ch.PNG" ><img src="figs/quack_ch.PNG"             width="100"></a>
<a href="figs/quack_obb.PNG" ><img src="figs/quack_obb.PNG"           width="100"></a>
<a href="figs/quack_inertia.PNG" ><img src="figs/quack_inertia.PNG"   width="100"></a>
<a href="figs/quack_balls_01.PNG" ><img src="figs/quack_balls_01.PNG" width="100"></a>
<a href="figs/quack_balls_02.PNG" ><img src="figs/quack_balls_02.PNG" width="100"></a>
<a href="figs/quack_balls_03.PNG" ><img src="figs/quack_balls_03.PNG" width="100"></a>
<a href="figs/quack_balls_04.PNG" ><img src="figs/quack_balls_04.PNG" width="100"></a>

[Demo Video](https://youtu.be/VEKGdyVggcU)

## Input

* MDLMesh that contains the 3D object
* Optional MTLTexture to sample colors

## Output

* MDLMesh that represents voxelized object (optionally with samploed colors)
* Collision Balls for GPU-based collision detection
* 3D Volume bitmap, where true bits represent the boundary or the inside of the sampled object.
* Mass, center of mass, and the inertia tensor based on the sampled volume.
* Convex hull in the real Brep (graph embedding with vertices, edges, and faces)
* Optimum oriented bounding box in terms of volume.

## Supported devices
It should work for the recent Macs, iPhones, and iPads.
It's been tested with the following devices and the tool chain.

* Mac mini(M1, 2020) macOS Monterey 12.3.1
* MacBook Pro (13-inch, 2017) macOS Monterey 12.3.1
* iPhone 13 mini iOS 15.4.1
* Xcode 13.3.1 (13E500a)


## Dependencies

* Accelerate.framework
* Metal.framework
* MetalPerformanceShaders.framework

## Install
```
$ cd /path/to/your/repo/Voxcell/Voxcell

$ ls #Check if you are in the right directory
Voxcell Voxcell.xcodeproj VoxcellTests module.modulemap

$ xcodebuild archive -scheme Voxcell -configuration Release -destination 'generic/platform=iOS' -archivePath './build/Voxcell.framework-iphoneos.xcarchive' SKIP_INSTALL=NO BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

$ xcodebuild archive -scheme Voxcell -configuration Release -destination 'generic/platform=iOS Simulator' -archivePath './build/Voxcell.framework-iphonesimulator.xcarchive' SKIP_INSTALL=NO BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

$ xcodebuild archive -scheme Voxcell -configuration Release -sdk macosx -archivePath './build/Voxcell.framework-macos.xcarchive' SKIP_INSTALL=NO BUILD_LIBRARIES_FOR_DISTRIBUTION=YES

$ xcodebuild -create-xcframework -framework './build/Voxcell.framework-iphoneos.xcarchive/Products/Library/Frameworks/Voxcell.framework' -framework './build/Voxcell.framework-iphonesimulator.xcarchive/Products/Library/Frameworks/Voxcell.framework' -framework './build/Voxcell.framework-macos.xcarchive/Products/Library/Frameworks/Voxcell.framework' -output './build/Voxcell.xcframework'
```

(Run `sudo xcode-select -s /Applications/Xcode.app/Contents/Developer` first if it's not selected yet.)

## Usage

API Documentation is found [here](Voxcell/Voxcell/Voxcell.docc/Voxcell.md).


## Running the accompanying demo App.
Open `/path/to/your/repo/Voxcell/VoxcellDemo/VoxcellDemo.xcodeproj`, build and run for macOS and iOS.

<a href="figs/ios_app_01.PNG"><img src="figs/ios_app_01.PNG" width="400"></a>
<a href="figs/ios_app_02.PNG"><img src="figs/ios_app_02.PNG" width="400"></a>

<a href="figs/macos_app_01.PNG"><img src="figs/macos_app_01.PNG" width="400"></a>


**NOTE:**

The 3D duck model is removed from this repo as its license prohibits distribution of in any readable forms.

The 3D cow model is provided by [Keenan Crane](https://www.cs.cmu.edu/~kmcrane/Projects/ModelRepository/) under CC0. 1.0.


## Technical Stuff

### Depth Peeling in Metal
The volume sampling is done by a technique called 'depth peeling' In OpenGL it is implemented using two Z-buffers alternatingly, which enable a function 'closest object but that is further than X' per pixel. This can not be implemented in Metal. However Metal has a nice ray-intersector, which can be used for depth peeling.
Please see 
[VolumeSampler/DepthPeeler.swift](Voxcell/Voxcell/VolumeSampler/DepthPeeler.swift) and
[VolumeSampler/DepthPeelerShaders.metal](Voxcell/Voxcell/VolumeSampler/DepthPeelerShaders.metal).

### Finding Convex Hull.
The implementation is taken from my existing C++ codebase for computational geometry and graph algorithms.
The algorithm is based on 
 *Computational Geometry Algorithms and Applications*,
 M. de Berg, M. van Kreveld, M. Overmars, and O.Schwartzkopf
 2nd Ed, Springer 2000, ISBN 3-540-65620-0.
Please see [CppCode/manifold_convex_hull.cpp](Voxcell/Voxcell/CppCode/manifold_convex_hull.cpp).


### Notes on how to build an Apple framework that contains both Swift and C++ code.

Ref: [Boris Gromov's Gist](https://gist.github.com/bgromov/f4327343ad67a5f7216262ccbe99c376)

1. Arrange an adapter class in Obj-c for each C++ class.

( Ex.
```
@implementation ManifoldObjc
...
-(void) findConvexHullOrOrientedBoundingBox:(const simd_float3 *const) points
                                  numPoints:(const int) numPoints
                     forOrientedBoundingBox:(const bool) orientedBoundingBox
{
    ...
    Manifold convex_hull;
    ...
    convex_hull.findConvexHull(vec, pred);
    ...
}
```
in [SwiftObjcCppInterface/manifold_objc.{h,mm}](Voxcell/Voxcell/SwiftObjcCppInterface/manifold_objc.mm) )

2. DO NOT create a bridging header file

3. Arrange an adapter class in Swift for Obj-c class

( Ex.
```
extension Brep {
    ...
    let mObjc : ManifoldObjc = ManifoldObjc()
    ...
}
``` 
in [BrepConvexHullExtension.swift](Voxcell/Voxcell/Brep/BrepConvexHullExtension.swift) )

4. Set all the Obj-c header files to Target Membership 'Public'

(Ex.
```
objects = {
    EF6E0C9E28233B5D00E5D6BC /* manifold_objc.h in Headers */ = {isa = PBXBuildFile; fileRef = EF6E0C9A28233B5D00E5D6BC /* manifold_objc.h */; settings = {ATTRIBUTES = (Public, ); }; };
}
```
in  [./Voxcell.xcodeproj/project.pbxproj](Voxcell/Voxcell.xcodeproj/project.pbxproj)

5. Place the Obj-c header filenames under the module definiton file. (Ex. [module.modulemap](Voxcell/module.modulemap) )

6. TARGETS -> Packaging -> Module Map File = module.modulemap (Ex. `buildSettings = {MODULEMAP_FILE = module.modulemap;}` in in  [./Voxcell.xcodeproj/project.pbxproj](Voxcell/Voxcell.xcodeproj/project.pbxproj) )






## License

GPL v3.

Open for commercial use.
Especially for indy developers and small businesses MIT-type license can be readily considered.

## Contact

For technical and commercial inquiries, please contact: Shoichiro Yamanishi

yamanishi72@gmail.com