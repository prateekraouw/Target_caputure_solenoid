cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(TungstenProtonSimulation)

# Find Geant4 package (NOT g4bl!)
find_package(Geant4 REQUIRED ui_all vis_all)

# Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Explicitly list all source files
set(SOURCES
    src/DetectorConstruction.cc
    src/PhysicsList.cc
    src/ActionInitialization.cc
    src/PrimaryGeneratorAction.cc
    src/RunAction.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/RFCavityField.cc
    src/SolenoidSystem.cc
    src/MomentumChicane.cc
)

# Add the executable with explicit source files
add_executable(tungsten_sim tungsten_sim.cc ${SOURCES})

# Link against Geant4 libraries
target_link_libraries(tungsten_sim ${Geant4_LIBRARIES})

# Install the executable
install(TARGETS tungsten_sim DESTINATION bin)

# Copy necessary scripts to build directory
set(TUNGSTEN_SCRIPTS
    init_vis.mac
    vis.mac
    run.mac
)

foreach(_script ${TUNGSTEN_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
  )
endforeach()
