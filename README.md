# Geant4 Solenoid Beamline Simulation

This project simulates a realistic particle beamline using **Geant4**, designed with a series of magnetic solenoids that guide and focus a charged particle beam. The simulation incorporates:
- Tapering solenoids
- Adjustable solenoid geometry
- Realistic magnetic fringe fields (using `tanh` transitions)
- Customizable gaps between solenoids

It is intended for use in high-energy physics simulations, especially muon or proton beam transport systems.

---

## 📁 Project Structure
```
├── **build**
│   ├── 6D_vector.csv
│   ├── all_23_solenoids.csv
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 3.28.3
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   ├── CMakeSystem.cmake
│   │   │   ├── CompilerIdC
│   │   │   │   ├── a.out
│   │   │   │   ├── CMakeCCompilerId.c
│   │   │   │   └── tmp
│   │   │   └── CompilerIdCXX
│   │   │       ├── a.out
│   │   │       ├── CMakeCXXCompilerId.cpp
│   │   │       └── tmp
│   │   ├── cmake.check_cache
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── Makefile2
│   │   ├── Makefile.cmake
│   │   ├── pkgRedirects
│   │   ├── progress.marks
│   │   ├── TargetDirectories.txt
│   │   └── tungsten_sim.dir
│   │       ├── build.make
│   │       ├── cmake_clean.cmake
│   │       ├── compiler_depend.internal
│   │       ├── compiler_depend.make
│   │       ├── compiler_depend.ts
│   │       ├── DependInfo.cmake
│   │       ├── depend.make
│   │       ├── flags.make
│   │       ├── link.txt
│   │       ├── progress.make
│   │       ├── src
│   │       │   ├── ActionInitialization.cc.o
│   │       │   ├── ActionInitialization.cc.o.d
│   │       │   ├── ChicaneConstruction.cc.o
│   │       │   ├── ChicaneConstruction.cc.o.d
│   │       │   ├── DetectorConstruction.cc.o
│   │       │   ├── DetectorConstruction.cc.o.d
│   │       │   ├── ElectricFieldSetup.cc.o
│   │       │   ├── ElectricFieldSetup.cc.o.d
│   │       │   ├── EventAction.cc.o
│   │       │   ├── EventAction.cc.o.d
│   │       │   ├── MomentumChicane.cc.o
│   │       │   ├── MomentumChicane.cc.o.d
│   │       │   ├── PhysicsList.cc.o
│   │       │   ├── PhysicsList.cc.o.d
│   │       │   ├── PrimaryGeneratorAction.cc.o
│   │       │   ├── PrimaryGeneratorAction.cc.o.d
│   │       │   ├── RFCavityField.cc.o
│   │       │   ├── RFCavityField.cc.o.d
│   │       │   ├── RunAction.cc.o
│   │       │   ├── RunAction.cc.o.d
│   │       │   ├── SolenoidSegment.cc.o
│   │       │   ├── SolenoidSegment.cc.o.d
│   │       │   ├── SolenoidSystem.cc.o
│   │       │   ├── SolenoidSystem.cc.o.d
│   │       │   ├── SteppingAction.cc.o
│   │       │   └── SteppingAction.cc.o.d
│   │       ├── tungsten_sim.cc.o
│   │       └── tungsten_sim.cc.o.d
│   ├── cmake_install.cmake
│   ├── init_vis.mac
│   ├── Makefile
│   ├── optimizer.py
│   ├── particle_data0.csv
│   ├── plot.ipynb
│   ├── run.mac
│   ├── tungsten_sim
│   └── vis.mac
├── CMakeLists.txt
├── **include**
│   ├── ActionInitialization.hh
│   ├── ChicaneConstruction.hh
│   ├── DetectorConstruction.hh
│   ├── EventAction.hh
│   ├── MomentumChicane.hh
│   ├── PhysicsList.hh
│   ├── PrimaryGeneratorAction.hh
│   ├── RFCavityField.hh
│   ├── RunAction.hh
│   ├── SolenoidSegment.hh
│   ├── SolenoidSystem.hh
│   └── SteppingAction.hh
├── init_vis.mac
├── main
├── particle_data0.csv
├── particle_data_run1.csv
├── README.md
├── **run.mac**
├── **src**
│   ├── ActionInitialization.cc
│   ├── ChicaneConstruction.cc
│   ├── DetectorConstruction.cc
│   ├── EventAction.cc
│   ├── MomentumChicane.cc
│   ├── PhysicsList.cc
│   ├── PrimaryGeneratorAction.cc
│   ├── RFCavityField.cc
│   ├── RunAction.cc
│   ├── SolenoidSegment.cc
│   ├── SolenoidSystem.cc
│   └── SteppingAction.cc
├── **tungsten_sim.cc**
└── **vis.mac**
```
---

## ⚙️ Prerequisites

- [Geant4 (v11+)](https://geant4.web.cern.ch/support/download) built with visualization and multithreading
- CMake (>= 3.16)
- C++17-compatible compiler (e.g., g++, clang++)
- Linux or macOS (tested on AlmaLinux and Ubuntu)

---

## 🔧 Building the Project

1. Source Geant4 environment:
   ```bash
    source /path/to/geant4-install/bin/geant4make.sh
   ```
   ```bash
   mkdir build
   ```
   ```bash
   cd build
   ```
   ```bash
   cmake ..
    ```
   ```bash
   make -j$(nproc)
   ```
## run in GUI mode
  ```bash
  ./tungstem_sim 
  ```
## run in command line mode
  ```bash
  ./tungstem_sim run.mac 
  ```
  
## Analysis
I. A Jupyter-Notebook plot.ipynb exists in the `build` directory.
- i. It has all the code build in for field and space-phase analysis
 
## Data for Analysis ( in `build` Directory)
1.6D_vector.csv 
- i. for space-phase Analysis
## 
2. all_23_solenoids.csv 
- ii. for solenoid field analysis
# Target_caputure_solenoid
