set(MEMSIM_TEST_SOURCES
    ../src/core/MemoryBlock.cpp
    ../src/core/MemoryPool.cpp
    ../src/algorithms/AlgorithmEngine.cpp
    ../src/algorithms/FirstFit.cpp
    ../src/algorithms/NextFit.cpp
    ../src/algorithms/BestFit.cpp
    ../src/algorithms/WorstFit.cpp
)

function(add_memsim_test target file)
    qt_add_executable(${target} ${file} ${MEMSIM_TEST_SOURCES})
    target_include_directories(${target} PRIVATE ../src)
    target_link_libraries(${target} PRIVATE Qt6::Core Qt6::Test)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_memsim_test(test_firstfit test_firstfit.cpp)
add_memsim_test(test_nextfit test_nextfit.cpp)
add_memsim_test(test_bestfit test_bestfit.cpp)
add_memsim_test(test_worstfit test_worstfit.cpp)
add_memsim_test(test_splitting test_splitting.cpp)
add_memsim_test(test_coalescing test_coalescing.cpp)
add_memsim_test(test_fragmentation test_fragmentation.cpp)
