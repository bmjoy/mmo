# We require 3.16 for the VS_DPI_AWARE option below, otherwise the application will be scaled by windows
# which will result in blurry madness on high dpi monitors
cmake_minimum_required(VERSION 3.16)

# Imgui sources
file(GLOB imgui_src 
	"${PROJECT_SOURCE_DIR}/deps/imgui/imgui.cpp"
	"${PROJECT_SOURCE_DIR}/deps/imgui/imgui_draw.cpp"
	"${PROJECT_SOURCE_DIR}/deps/imgui/imgui_widgets.cpp"
	"${PROJECT_SOURCE_DIR}/deps/imgui/examples/imgui_impl_win32.cpp"
	"${PROJECT_SOURCE_DIR}/deps/imgui/examples/imgui_impl_dx11.cpp")

message(STATUS "Add src: ${imgui_src}")

include_directories(${FBXSDK_INCLUDE_DIR})

# Add executable
add_gui_exe(model_editor ${imgui_src})

target_link_libraries(model_editor 
	base 
	log 
	simple_file_format_hdrs 
	binary_io_hdrs 
	network_hdrs 
	auth_protocol 
	graphics 
	math 
	virtual_dir 
	hpak_v1_0 
	tex_v1_0 
	assets
	mesh
	mesh_v1_0)
	
target_link_libraries(model_editor ${FBXSDK_LIBRARIES})
target_link_libraries(model_editor ${OPENSSL_LIBRARIES})
set_property(TARGET model_editor PROPERTY FOLDER "editor")

# Editor is dpi aware
set_property(TARGET model_editor PROPERTY VS_DPI_AWARE "PerMonitor")
