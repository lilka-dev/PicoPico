# Install script for directory: /home/yevhen/PicoPico/SDL

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/yevhen/PicoPico/build_luac/SDL/libSDL2-2.0.so.0.22.0"
    "/home/yevhen/PicoPico/build_luac/SDL/libSDL2-2.0.so.0"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2-2.0.so.0.22.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libSDL2-2.0.so.0"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/yevhen/PicoPico/build_luac/SDL/libSDL2-2.0.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/yevhen/PicoPico/build_luac/SDL/libSDL2main.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake"
         "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2Targets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake"
         "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2/SDL2mainTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/CMakeFiles/Export/f084604df1a27ef5b4fef7c7544737d1/SDL2mainTargets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SDL2" TYPE FILE FILES
    "/home/yevhen/PicoPico/SDL/SDL2Config.cmake"
    "/home/yevhen/PicoPico/build_luac/SDL2ConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "/home/yevhen/PicoPico/SDL/include/SDL.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_assert.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_atomic.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_audio.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_bits.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_blendmode.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_clipboard.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_android.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_emscripten.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_iphoneos.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_macosx.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_minimal.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_os2.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_pandora.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_windows.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_config_winrt.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_copying.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_cpuinfo.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_egl.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_endian.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_error.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_events.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_filesystem.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_gamecontroller.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_gesture.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_haptic.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_hidapi.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_hints.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_joystick.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_keyboard.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_keycode.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_loadso.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_locale.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_log.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_main.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_messagebox.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_metal.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_misc.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_mouse.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_mutex.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_name.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengl.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengl_glext.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles2.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles2_gl2.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles2_gl2ext.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles2_gl2platform.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_opengles2_khrplatform.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_pixels.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_platform.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_power.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_quit.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_rect.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_render.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_rwops.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_scancode.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_sensor.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_shape.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_stdinc.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_surface.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_system.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_syswm.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_assert.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_common.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_compare.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_crc32.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_font.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_fuzzer.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_harness.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_images.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_log.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_md5.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_memory.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_test_random.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_thread.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_timer.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_touch.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_types.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_version.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_video.h"
    "/home/yevhen/PicoPico/SDL/include/SDL_vulkan.h"
    "/home/yevhen/PicoPico/SDL/include/begin_code.h"
    "/home/yevhen/PicoPico/SDL/include/close_code.h"
    "/home/yevhen/PicoPico/build_luac/SDL/include/SDL_config.h"
    "/home/yevhen/PicoPico/build_luac/SDL/include/SDL_revision.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  
            execute_process(COMMAND /usr/bin/cmake -E create_symlink
              "libSDL2-2.0.so" "libSDL2.so"
              WORKING_DIRECTORY "/home/yevhen/PicoPico/build_luac/SDL")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/libSDL2.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/yevhen/PicoPico/build_luac/SDL/sdl2.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/home/yevhen/PicoPico/build_luac/SDL/sdl2-config")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/aclocal" TYPE FILE FILES "/home/yevhen/PicoPico/SDL/sdl2.m4")
endif()

