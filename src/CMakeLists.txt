# LIBRARY PROJECT - sortnetutils

# === Includes ===
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# === Create Library ===
add_library(sortnetutils "Network.h" "layers.cpp" "CE.h" "layers.h" "permute.h" "permute.cpp" "exec.h" "exec.cpp" "symmetry.h" "symmetry.cpp" "Network.cpp" "tikz.h" "tikz.cpp" "format.h"  "parse.h" "parse.cpp"  "NetworkVerifier.h" "NetworkVerifier.cpp" "format.cpp")

# === Link Libraries ===

# === Feature Enables ===
set_target_properties(sortnetutils PROPERTIES
    DEBUG_POSTFIX          "d"
    RELWITHDEBINFO_POSTFIX "rd"
)
if (SNU_MSVC_STATIC_RUNTIME)
    set_property(TARGET sortnetutils PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set_property(TARGET sortnetutils PROPERTY CXX_STANDARD 20)
enable_ipo(sortnetutils)
set_arch(sortnetutils)

# === Install ===
# Install binaries and headers
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static library files
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # includes
)

# Export includes
install(DIRECTORY "../include"
        DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY "../src"
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        FILES_MATCHING
        PATTERN "*.h")

# Configure the main config.cmake file
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install the export set and configuration files
install(EXPORT ${PROJECT_NAME}Config
    FILE      ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)