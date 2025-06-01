# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-src"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-build"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/tmp"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/src/arduinojson-populate-stamp"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/src"
  "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/src/arduinojson-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/src/arduinojson-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Raspberrypi_Pico/GP2040-CE-0.7.11_8BitDo4/GP2040-CE/build/_deps/arduinojson-subbuild/arduinojson-populate-prefix/src/arduinojson-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
