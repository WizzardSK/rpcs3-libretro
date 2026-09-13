if(USE_SYSTEM_RTMIDI)
  message(STATUS "RPCS3: using shared RtMidi")
  pkg_check_modules(RtMidi REQUIRED IMPORTED_TARGET rtmidi>=6.0.0)
  add_library(rtmidi INTERFACE)
  target_link_libraries(rtmidi INTERFACE PkgConfig::RtMidi)
  target_include_directories(rtmidi INTERFACE PkgConfig::RtMidi)
else()
  option(RTMIDI_API_JACK "Compile with JACK support." OFF)
  # RtMidi's Android backend reaches for JNI_GetCreatedJavaVMs, which the JVM
  # in an Android app provides and a plain shared library has no way to link
  # against - the libretro core fails at the last step, linking, with that one
  # symbol undefined. There is no MIDI device to reach for here anyway.
  option(RTMIDI_API_AMIDI "Compile with Android support." OFF)
  option(RTMIDI_BUILD_TESTING "Build test programs" OFF)
  set(RTMIDI_TARGETNAME_UNINSTALL "uninstall-rpcs3-rtmidi")
  add_subdirectory(rtmidi EXCLUDE_FROM_ALL)
endif()
