cmake_minimum_required(VERSION 3.16)
project(SupermanVsGhost)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    set(CMAKE_CXX_FLAGS "/W4")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# Find SFML
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

# Define source files
set(SOURCES
    main.cpp
    Game.cpp
    Superman.cpp
    Ghost.cpp
    Projectile.cpp
    ParticleSystem.cpp
    ResourceManager.cpp
)

# Define header files
set(HEADERS
    Game.h
    Superman.h
    Ghost.h
    Projectile.h
    ParticleSystem.h
    ResourceManager.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-audio)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Copy SFML DLLs to output directory for Windows
    if(SFML_FOUND)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(SFML_SUFFIX "-d-2")
        else()
            set(SFML_SUFFIX "-2")
        endif()
        
        # Define SFML DLL names
        set(SFML_DLLS
            sfml-system${SFML_SUFFIX}.dll
            sfml-window${SFML_SUFFIX}.dll
            sfml-graphics${SFML_SUFFIX}.dll
            sfml-audio${SFML_SUFFIX}.dll
        )
        
        # Copy DLLs
        foreach(DLL ${SFML_DLLS})
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:sfml-system>/${DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying ${DLL}"
            )
        endforeach()
        
        # Also copy OpenAL DLL for audio
        find_file(OPENAL_DLL openal32.dll PATHS ${SFML_ROOT}/bin DOC "OpenAL DLL")
        if(OPENAL_DLL)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${OPENAL_DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying OpenAL DLL"
            )
        endif()
    endif()
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Superman vs Ghost"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
else()
    # Linux-specific settings
    # Install desktop file for Linux
    configure_file(
        ${CMAKE_SOURCE_DIR}/superman-vs-ghost.desktop.in
        ${CMAKE_BINARY_DIR}/superman-vs-ghost.desktop
        @ONLY
    )
endif()

# Create assets directory structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/textures)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/sounds)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets/fonts)

# Copy assets if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "Copying assets"
    )
endif()

# Installation rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)

# Install assets
install(DIRECTORY ${CMAKE_BINARY_DIR}/assets
    DESTINATION bin
    COMPONENT Runtime
    OPTIONAL
)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "Superman vs Ghost")
set(CPACK_PACKAGE_VENDOR "Game Developer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Superman vs Ghost - Action Game")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "SupermanVsGhost")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Superman vs Ghost")
    set(CPACK_NSIS_PACKAGE_NAME "Superman vs Ghost")
    set(CPACK_NSIS_CONTACT "developer@example.com")
    set(CPACK_NSIS_HELP_LINK "https://github.com/example/superman-vs-ghost")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Game Developer <developer@example.com>")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsfml-dev (>= 2.5)")
endif()

include(CPack)

# Build configuration summary
message(STATUS "")
message(STATUS "Superman vs Ghost - Build Configuration Summary")
message(STATUS "==============================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "SFML Version: ${SFML_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# Custom targets for development
add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Superman vs Ghost"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/assets
    COMMENT "Cleaning all build files and assets"
)