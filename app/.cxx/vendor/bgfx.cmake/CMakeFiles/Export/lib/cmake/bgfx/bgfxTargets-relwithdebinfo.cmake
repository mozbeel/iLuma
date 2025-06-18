#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bgfx::bx" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bx )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bx "${_IMPORT_PREFIX}/lib/libbx.a" )

# Import target "bgfx::bin2c" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bin2c APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bin2c PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/bin2c"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bin2c )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bin2c "${_IMPORT_PREFIX}/bin/bin2c" )

# Import target "bgfx::bimg" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bimg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bimg PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbimg.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bimg )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bimg "${_IMPORT_PREFIX}/lib/libbimg.a" )

# Import target "bgfx::bimg_decode" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bimg_decode APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bimg_decode PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbimg_decode.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bimg_decode )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bimg_decode "${_IMPORT_PREFIX}/lib/libbimg_decode.a" )

# Import target "bgfx::bimg_encode" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bimg_encode APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bimg_encode PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbimg_encode.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bimg_encode )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bimg_encode "${_IMPORT_PREFIX}/lib/libbimg_encode.a" )

# Import target "bgfx::texturec" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::texturec APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::texturec PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/texturec"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::texturec )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::texturec "${_IMPORT_PREFIX}/bin/texturec" )

# Import target "bgfx::bgfx" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::bgfx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::bgfx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbgfx.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::bgfx )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::bgfx "${_IMPORT_PREFIX}/lib/libbgfx.a" )

# Import target "bgfx::texturev" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::texturev APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::texturev PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/texturev.so"
  IMPORTED_SONAME_RELWITHDEBINFO "texturev.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::texturev )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::texturev "${_IMPORT_PREFIX}/bin/texturev.so" )

# Import target "bgfx::geometryc" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::geometryc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::geometryc PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/geometryc"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::geometryc )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::geometryc "${_IMPORT_PREFIX}/bin/geometryc" )

# Import target "bgfx::geometryv" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::geometryv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::geometryv PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/geometryv.so"
  IMPORTED_SONAME_RELWITHDEBINFO "geometryv.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::geometryv )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::geometryv "${_IMPORT_PREFIX}/bin/geometryv.so" )

# Import target "bgfx::shaderc" for configuration "RelWithDebInfo"
set_property(TARGET bgfx::shaderc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bgfx::shaderc PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/shaderc"
  )

list(APPEND _IMPORT_CHECK_TARGETS bgfx::shaderc )
list(APPEND _IMPORT_CHECK_FILES_FOR_bgfx::shaderc "${_IMPORT_PREFIX}/bin/shaderc" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
