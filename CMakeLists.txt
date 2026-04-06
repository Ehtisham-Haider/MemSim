cmake_minimum_required(VERSION 3.21)
project(MemSim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Concurrent
    Test
)

qt_standard_project_setup()

include(CTest)
enable_testing()

qt_add_executable(MemSim
    src/main.cpp
    src/core/MemoryBlock.cpp
    src/core/MemoryPool.cpp
    src/algorithms/AlgorithmEngine.cpp
    src/algorithms/FirstFit.cpp
    src/algorithms/NextFit.cpp
    src/algorithms/BestFit.cpp
    src/algorithms/WorstFit.cpp
    src/controller/SimulationController.cpp
    src/controller/StressTestEngine.cpp
    src/controller/ComparativeAnalyser.cpp
    src/controller/ExportManager.cpp
    src/models/MemoryTableModel.cpp
    src/models/EventLogModel.cpp
    resources/qml.qrc
)

set_target_properties(MemSim PROPERTIES
    QT_QML_MODULE_NO_IMPORT_SCAN TRUE
)

target_include_directories(MemSim PRIVATE src)

target_link_libraries(MemSim PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Concurrent
)

add_subdirectory(tests)

include(GNUInstallDirs)
install(TARGETS MemSim
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
