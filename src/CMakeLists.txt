# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	 *.c
	 *.cpp)
	
# Add header files
file(GLOB_RECURSE HEADER_FILES 
	 *.h
	 *.hpp)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# Define the include DIRs
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC "${ASSIMP_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLAD_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${GLM_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IMGUI_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${IRRKLANG_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${FREETYPE_INCLUDE_DIR}")

target_link_libraries(${PROJECT_NAME} "${OPENGL_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${ASSIMP_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLFW_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${GLAD_LIBRARY}"      "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${IMGUI_LIBRARY}"     "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${STB_IMAGE_LIBRARY}" "${CMAKE_DL_LIBS}")
target_link_libraries(${PROJECT_NAME} "${IRRKLANG_LIBRARY}")
target_link_libraries(${PROJECT_NAME} "${FREETYPE_LIBRARY}")



target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBRARY_SUFFIX="")

add_custom_command(TARGET  ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
						   ${CMAKE_SOURCE_DIR}/res
						   ${CMAKE_CURRENT_BINARY_DIR}/res)

if(WIN32)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		add_custom_command(TARGET  ${SAMPLE_NAME} PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy_directory
							${CMAKE_SOURCE_DIR}/thirdparty/irrKlang-1.6.0/bin/win32-visualStudio
							${CMAKE_CURRENT_BINARY_DIR})
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		add_custom_command(TARGET  ${SAMPLE_NAME} PRE_BUILD
							COMMAND ${CMAKE_COMMAND} -E copy_directory
							${CMAKE_SOURCE_DIR}/thirdparty/irrKlang-1.6.0/bin/win32-gcc
							${CMAKE_CURRENT_BINARY_DIR})
	endif()
endif(WIN32)

# Create virtual folders to make it look nicer in VS
source_group("sources" FILES ${SOURCE_FILES_EXE})						   
source_group("headers" FILES ${HEADER_FILES_EXE})