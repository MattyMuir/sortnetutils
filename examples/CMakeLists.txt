# EXECUTABLE PROJECT - example

# === Create Executable ===
add_executable(example "example.cpp")

# === Libraries ===
target_link_libraries(example PRIVATE sortnetutils)
target_include_directories(example PRIVATE "../include/")

# === Feature Enables ===
if (SNU_MSVC_STATIC_RUNTIME)
	set_property(TARGET example PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set_property(TARGET example PROPERTY CXX_STANDARD 23)
enable_ipo(example)
set_arch(example)