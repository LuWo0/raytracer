cmake_minimum_required(VERSION 3.10)

project(main LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS-specific OpenMP configuration
if(APPLE)
  # Try to find Homebrew's libomp
  execute_process(
    COMMAND brew --prefix libomp
    OUTPUT_VARIABLE LIBOMP_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  if(LIBOMP_PREFIX)
    message(STATUS "Found Homebrew libomp at: ${LIBOMP_PREFIX}")
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I${LIBOMP_PREFIX}/include")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${LIBOMP_PREFIX}/lib/libomp.dylib")

    # Set variables for find_package
    set(OpenMP_C_FLAGS "${OpenMP_C_FLAGS}" CACHE STRING "" FORCE)
    set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS}" CACHE STRING "" FORCE)
    set(OpenMP_omp_LIBRARY "${OpenMP_omp_LIBRARY}" CACHE STRING "" FORCE)
  endif()
endif()

# Find OpenMP
find_package(OpenMP)

# Add executable
add_executable(main
  src/main.cpp
)

# Include directories
target_include_directories(main PRIVATE include)

# Compiler-specific optimizations
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# If OpenMP is found, link it
if(OpenMP_CXX_FOUND)
  message(STATUS "OpenMP found - parallel rendering enabled")
  target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)

  # On macOS, we need to explicitly add the library path
  if(APPLE AND LIBOMP_PREFIX)
    target_link_directories(main PRIVATE ${LIBOMP_PREFIX}/lib)
  endif()
else()
  message(WARNING "OpenMP NOT found - rendering will be single-threaded (slower)")
  message(WARNING "Install with: brew install libomp")
endif()

# Optimization flags for Release build
if(CMAKE_BUILD_TYPE MATCHES Release)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(main PRIVATE
      -O3                    # Maximum optimization
      -march=native          # Use CPU-specific instructions
      -ffast-math           # Fast floating-point math
      -funroll-loops        # Unroll loops
    )
  elseif(MSVC)
    target_compile_options(main PRIVATE
      /O2                    # Maximum optimization
      /arch:AVX2            # Use AVX2 instructions if available
      /fp:fast              # Fast floating-point math
    )
  endif()
endif()

# Debug build flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(main PRIVATE
      -g                     # Debug symbols
      -Wall                  # All warnings
      -Wextra               # Extra warnings
    )
  elseif(MSVC)
    target_compile_options(main PRIVATE
      /W4                    # Warning level 4
      /Zi                    # Debug information
    )
  endif()
endif()

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
