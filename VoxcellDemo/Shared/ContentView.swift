import SwiftUI

enum Figure: Int, CaseIterable, Identifiable {

    case MooMoo
    case QuackQuack

    var id: Int { self.rawValue }

    var name: String {
        switch self {
            case .MooMoo:     return "MooMoo"
            case .QuackQuack: return "QuackQuack"
        }
    }
}

enum SampleResolution: Int, CaseIterable, Identifiable {

    case Res1
    case Res2
    case Res3
    case Res4
    case Res5
    case Res6

    var id: Int { self.rawValue }

    var name: String {
        switch self {
            case .Res1:        return "0.008"
            case .Res2:        return "0.016"
            case .Res3:        return "0.032"
            case .Res4:        return "0.064"
            case .Res5:        return "0.128"
            case .Res6:        return "0.256"
        }
    }
}

enum VoxelType: Int, CaseIterable, Identifiable {

    case Voxel
    case Balls

    var id: Int { self.rawValue }

    var name: String {
        switch self {
            case .Voxel: return "Minecraft-ish"
            case .Balls: return "Collision Balls"
        }
    }
}

enum HullType: Int, CaseIterable, Identifiable {

    case ConvexHull
    case OBB

    var id: Int { self.rawValue }

    var name: String {
        switch self {
            case .ConvexHull: return "Convex Hull"
            case .OBB: return "Oriented Bounding Box"
        }
    }
}

enum Representation: Int, CaseIterable, Identifiable {

    case Original
    case VolumeSampled
    case Hull
    case ComInertia

    var id: Int { self.rawValue }

    var name: String {
        switch self {
            case .Original:      return "Original"
            case .VolumeSampled: return "Volume Sampled"
            case .Hull:          return "Hull"
            case .ComInertia:    return "CoM & Inertia"
        }
    }
}


struct ContentView: View {

    @EnvironmentObject var worldManager: WorldManager

    @State var figure:             Figure = .QuackQuack
    @State var representation:     Representation = .Original
    @State var sampleResolution:   SampleResolution = .Res6
    @State var voxelType:          VoxelType = .Voxel
    @State var hullType:           HullType = .ConvexHull
    @State var autoRotate:         Bool = true
    
    @State var configText:         String = ""

    func updateConfigText() {
        switch representation {
          case .Original:
            configText = "Original 3D Model"
          case .VolumeSampled:
            switch voxelType {
              case .Voxel:
                switch sampleResolution {
                  case .Res1:
                    configText = "Voxel: Grid Pitch 0.008"
                  case .Res2:
                    configText = "Voxel: Grid Pitch 0.016"
                  case .Res3:
                    configText = "Voxel: Grid Pitch 0.032"
                  case .Res4:
                    configText = "Voxel: Grid Pitch 0.064"
                  case .Res5:
                    configText = "Voxel: Grid Pitch 0.128"
                  case .Res6:
                    configText = "Voxel: Grid Pitch 0.256"
                }
              case .Balls:
                switch sampleResolution {
                  case .Res1:
                    configText = "Collision Balls: Grid Pitch 0.008"
                  case .Res2:
                    configText = "Collision Balls: Grid Pitch 0.016"
                  case .Res3:
                    configText = "Collision Balls: Grid Pitch 0.032"
                  case .Res4:
                    configText = "Collision Balls: Grid Pitch 0.064"
                  case .Res5:
                    configText = "Collision Balls: Grid Pitch 0.128"
                  case .Res6:
                    configText = "Collision Balls: Grid Pitch 0.256"
                }

            }
          case .ComInertia:
            configText = "Center of Mass & Insertia (Eigen Vectors)"
          case .Hull:
            switch hullType {
              case .ConvexHull:
                configText = "Convex Hull"
              case .OBB:
                configText = "Optimum Oriented Bounding Box"
            }
        }
    }


#if os(iOS)
    @Environment(\.verticalSizeClass) var verticalSizeClass


    var body: some View {

        if verticalSizeClass == .compact {
        
            HStack {
                MetalView().padding()

                VStack {

                    Text(configText).font(.system(size: 26))
                
                    Picker( "", selection: $figure ) {
                        ForEach( Figure.allCases ) { val in
                            Text( val.name).tag(val)
                        }
                    }
                    .font(.largeTitle)
                    .pickerStyle( SegmentedPickerStyle() )
                    .onChange(of: figure) { tag in
                        worldManager.changeFigureTo( figure: figure )
                        updateConfigText()
                    }

                    Picker( "", selection: $representation ) {
                        ForEach( Representation.allCases ) { val in
                            Text( val.name).tag(val)
                        }
                    }
                    .font(.largeTitle)
                    .pickerStyle( SegmentedPickerStyle() )
                    .onChange(of: representation) { tag in
                        worldManager.changeRepresentationTo( representation: representation )
                        updateConfigText()
                    }

                    Picker( "", selection: $voxelType ) {
                        ForEach( VoxelType.allCases ) { val in
                            Text( val.name).tag(val)
                        }
                    }
                    .font(.largeTitle)
                    .pickerStyle( SegmentedPickerStyle() )
                    .onChange(of: voxelType) { tag in
                        worldManager.changeVoxelTypeTo( voxelType: voxelType )
                        updateConfigText()
                    }

                    Picker( "", selection: $sampleResolution ) {
                        ForEach( SampleResolution.allCases ) { val in
                            Text( val.name).tag(val)
                        }
                    }
                    .font(.largeTitle)
                    .pickerStyle( SegmentedPickerStyle() )
                    .onChange(of: sampleResolution) { tag in

                        switch tag {
                            case .Res1:
                                worldManager.changeResolution( resolution: 0.008 )
                            case .Res2:
                                worldManager.changeResolution( resolution: 0.016 )
                            case .Res3:
                                worldManager.changeResolution( resolution: 0.032 )
                            case .Res4:
                                worldManager.changeResolution( resolution: 0.064 )
                            case .Res5:
                                worldManager.changeResolution( resolution: 0.128 )
                            case .Res6:
                                worldManager.changeResolution( resolution: 0.256 )
                        }
                        updateConfigText()
                    }

                    Picker( "", selection: $hullType ) {
                        ForEach( HullType.allCases ) { val in
                            Text( val.name).tag(val)
                        }
                    }
                    .font(.largeTitle)
                    .pickerStyle( SegmentedPickerStyle() )
                    .onChange(of: hullType) { tag in
                        worldManager.changeHullTypeTo( hullType: hullType )
                        updateConfigText()
                    }

                    Toggle("Auto Rotation", isOn: $autoRotate)
                    .onChange(of: autoRotate) { value in
                        worldManager.setAutoRotate( value: value )
                        updateConfigText()
                    }
                }
            }
        } else {
            VStack {

                MetalView().padding()

                Text(configText).font(.system(size: 26))

                Picker( "", selection: $figure ) {
                    ForEach( Figure.allCases ) { val in
                        Text( val.name).tag(val)
                    }
                }
                .font(.largeTitle)
                .pickerStyle( SegmentedPickerStyle() )
                .onChange(of: figure) { tag in
                    worldManager.changeFigureTo( figure: figure )
                    updateConfigText()
                }

                Picker( "", selection: $representation ) {
                    ForEach( Representation.allCases ) { val in
                        Text( val.name).tag(val)
                    }
                }
                .font(.largeTitle)
                .pickerStyle( SegmentedPickerStyle() )
                .onChange(of: representation) { tag in
                    worldManager.changeRepresentationTo( representation: representation )
                    updateConfigText()
                }

                Picker( "", selection: $voxelType ) {
                    ForEach( VoxelType.allCases ) { val in
                        Text( val.name).tag(val)
                    }
                }
                .font(.largeTitle)
                .pickerStyle( SegmentedPickerStyle() )
                .onChange(of: voxelType) { tag in
                    worldManager.changeVoxelTypeTo( voxelType: voxelType )
                    updateConfigText()
                }

                Picker( "", selection: $sampleResolution ) {
                    ForEach( SampleResolution.allCases ) { val in
                        Text( val.name).tag(val)
                    }
                }
                .font(.largeTitle)
                .pickerStyle( SegmentedPickerStyle() )
                .onChange(of: sampleResolution) { tag in

                    switch tag {
                        case .Res1:
                            worldManager.changeResolution( resolution: 0.008 )
                        case .Res2:
                            worldManager.changeResolution( resolution: 0.016 )
                        case .Res3:
                            worldManager.changeResolution( resolution: 0.032 )
                        case .Res4:
                            worldManager.changeResolution( resolution: 0.064 )
                        case .Res5:
                            worldManager.changeResolution( resolution: 0.128 )
                        case .Res6:
                            worldManager.changeResolution( resolution: 0.256 )
                    }
                    updateConfigText()
                }

                Picker( "", selection: $hullType ) {
                    ForEach( HullType.allCases ) { val in
                        Text( val.name).tag(val)
                    }
                }
                .font(.largeTitle)
                .pickerStyle( SegmentedPickerStyle() )
                .onChange(of: hullType) { tag in
                    worldManager.changeHullTypeTo( hullType: hullType )
                    updateConfigText()
                }

                Toggle("Auto Rotation", isOn: $autoRotate)
                .onChange(of: autoRotate) { value in
                    worldManager.setAutoRotate( value: value )
                    updateConfigText()
                }
            }
        }
    }
#else
    var body: some View {

        VStack {

            MetalView().padding()

            Text(configText).font(.system(size: 26))

            Picker( "", selection: $figure ) {
                ForEach( Figure.allCases ) { val in
                    Text( val.name).tag(val)
                }
            }
            .font(.largeTitle)
            .pickerStyle( SegmentedPickerStyle() )
            .onChange(of: figure) { tag in
                worldManager.changeFigureTo( figure: figure )
                updateConfigText()
            }

            Picker( "", selection: $representation ) {
                ForEach( Representation.allCases ) { val in
                    Text( val.name).tag(val)
                }
            }
            .font(.largeTitle)
            .pickerStyle( SegmentedPickerStyle() )
            .onChange(of: representation) { tag in
                worldManager.changeRepresentationTo( representation: representation )
                updateConfigText()
            }

            Picker( "", selection: $voxelType ) {
                ForEach( VoxelType.allCases ) { val in
                    Text( val.name).tag(val)
                }
            }
            .font(.largeTitle)
            .pickerStyle( SegmentedPickerStyle() )
            .onChange(of: voxelType) { tag in
                worldManager.changeVoxelTypeTo( voxelType: voxelType )
                updateConfigText()
            }

            Picker( "", selection: $sampleResolution ) {
                ForEach( SampleResolution.allCases ) { val in
                    Text( val.name).tag(val)
                }
            }
            .font(.largeTitle)
            .pickerStyle( SegmentedPickerStyle() )
            .onChange(of: sampleResolution) { tag in

                switch tag {
                    case .Res1:
                        worldManager.changeResolution( resolution: 0.008 )
                    case .Res2:
                        worldManager.changeResolution( resolution: 0.016 )
                    case .Res3:
                        worldManager.changeResolution( resolution: 0.032 )
                    case .Res4:
                        worldManager.changeResolution( resolution: 0.064 )
                    case .Res5:
                        worldManager.changeResolution( resolution: 0.128 )
                    case .Res6:
                        worldManager.changeResolution( resolution: 0.256 )
                }
                updateConfigText()
            }

            Picker( "", selection: $hullType ) {
                ForEach( HullType.allCases ) { val in
                    Text( val.name).tag(val)
                }
            }
            .font(.largeTitle)
            .pickerStyle( SegmentedPickerStyle() )
            .onChange(of: hullType) { tag in
                worldManager.changeHullTypeTo( hullType: hullType )
                updateConfigText()
            }

            Toggle("Auto Rotation", isOn: $autoRotate)
            .onChange(of: autoRotate) { value in
                worldManager.setAutoRotate( value: value )
                updateConfigText()
            }
        }
    }
#endif
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
