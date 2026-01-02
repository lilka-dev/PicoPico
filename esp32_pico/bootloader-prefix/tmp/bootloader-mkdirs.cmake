# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/yevhen/esp/esp-idf/components/bootloader/subproject"
  "/home/yevhen/PicoPico/esp32_pico/bootloader"
  "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix"
  "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/tmp"
  "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/src/bootloader-stamp"
  "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/src"
  "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/yevhen/PicoPico/esp32_pico/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
