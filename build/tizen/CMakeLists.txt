cmake_minimum_required(VERSION 2.6)
project("libdli" CXX)

set(libdli_prj_name "dli")

if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions("-DDEBUG_ENABLED")
endif()

foreach(flag ${PKGS_CFLAGS})
	set(extra_flags "${extra_flags} ${flag}")
endforeach(flag)

set(prj_cxx_std c++17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(extra_flags "${extra_flags} -fPIE -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(extra_flags "${extra_flags} -fPIE -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(extra_flags "${extra_flags} /std:${prj_cxx_std} /vmg /D_USE_MATH_DEFINES /D_CRT_SECURE_NO_WARNINGS /MP /GS /Oi /GL /EHsc")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")

set(prefix ${CMAKE_INSTALL_PREFIX})

set(libdli_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../libdli/")

set(libdli_src_dir "${libdli_dir}src/")
file(GLOB libdli_src_files "${libdli_src_dir}*.cpp")

set(libdli_include_dir "${libdli_dir}include/")
set(prefix_include_dir "${prefix}/include/")
include_directories(${libdli_include_dir}
	${prefix_include_dir}
)

add_library(${libdli_prj_name} STATIC ${libdli_src_files})
install(TARGETS ${libdli_prj_name} DESTINATION ${prefix}/lib)

file(GLOB libdli_include_files "${libdli_include_dir}**")
install(DIRECTORY ${libdli_include_files}
	DESTINATION ${prefix_include_dir}
	FILES_MATCHING PATTERN "*.h"
)
