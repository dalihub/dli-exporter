cmake_minimum_required(VERSION 2.6)
project("org.tizen.dli-exporter" CXX)

set(dli_exporter_prj_name "dli-exporter")

foreach(flag ${PKGS_CFLAGS})
	set(extra_flags "${extra_flags} ${flag}")
endforeach(flag)

set(prj_cxx_std c++14)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(extra_flags "${extra_flags} -fPIE -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(extra_flags "${extra_flags} -fPIE -std=${prj_cxx_std}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(extra_flags "${extra_flags} /std:${prj_cxx_std} /vmg /D_USE_MATH_DEFINES /D_CRT_SECURE_NO_WARNINGS /MP /GS /Oi /GL /EHsc")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${extra_flags}")

set(prefix ${CMAKE_INSTALL_PREFIX})

set(dli_exporter_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../")

if(UNIX)
	set(assimp_dir "${CMAKE_CURRENT_SOURCE_DIR}/../third-party/assimp-5.0.0/")
endif()

#
# CORE
#
set(dli_exporter_core_src_dir "${dli_exporter_dir}core/src/")
file(GLOB dli_exporter_core_src_files "${dli_exporter_core_src_dir}*.cpp")

set(dli_exporter_core_include_dir "${dli_exporter_dir}core/include/")
include_directories(${dli_exporter_core_include_dir}
)

if(UNIX)
	include_directories("${assimp_dir}include/"
		"${assimp_dir}linux64/include/"
	)
endif()

set(dli_exporter_core_prj_name "${dli_exporter_prj_name}-core")
add_library(${dli_exporter_core_prj_name} STATIC ${dli_exporter_core_src_files})

#
# CLI
#
set(dli_exporter_cli_src_dir "${dli_exporter_dir}cli/src/")
file(GLOB dli_exporter_cli_src_files "${dli_exporter_cli_src_dir}*.cpp")

set(dli_exporter_cli_prj_name ${dli_exporter_prj_name})
add_executable(${dli_exporter_cli_prj_name} ${dli_exporter_cli_src_files})

find_library(assimp assimp
	PATHS "${assimp_dir}/linux64/code"
)

target_link_libraries(${dli_exporter_cli_prj_name}
	${dli_exporter_core_prj_name}
	${assimp}
)
