# Install script for directory: C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/vendor/bgfx.cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/iLuma")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/moz04/AppData/Local/Android/Sdk/ndk/25.1.8937393/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licences/bgfx" TYPE FILE FILES "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/vendor/bgfx.cmake/bgfx/LICENSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx" TYPE FILE FILES "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/vendor/bgfx.cmake/cmake/bgfxToolUtils.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx" TYPE FILE FILES
    "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/generated/bgfxConfig.cmake"
    "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/generated/bgfxConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx/bgfxTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx/bgfxTargets.cmake"
         "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/CMakeFiles/Export/lib/cmake/bgfx/bgfxTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx/bgfxTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx/bgfxTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx" TYPE FILE FILES "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/CMakeFiles/Export/lib/cmake/bgfx/bgfxTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/bgfx" TYPE FILE FILES "C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/CMakeFiles/Export/lib/cmake/bgfx/bgfxTargets-relwithdebinfo.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/cmake/bx/cmake_install.cmake")
  include("C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/cmake/bimg/cmake_install.cmake")
  include("C:/Users/moz04/Desktop/GameDevelopment/iLuma/app/.cxx/vendor/bgfx.cmake/cmake/bgfx/cmake_install.cmake")

endif()

