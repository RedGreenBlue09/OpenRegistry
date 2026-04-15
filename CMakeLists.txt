cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0069 NEW) 
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

macro(set_build_dir_default TARGET_NAME_)
	set_target_properties(${TARGET_NAME_}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "Binary"
		LIBRARY_OUTPUT_DIRECTORY "Library"
		RUNTIME_OUTPUT_DIRECTORY "Binary"
	)
endmacro()

project(OpenRegistry LANGUAGES C)

set(OpenRegistry_HEADER_FILES
	"${OpenRegistry_SOURCE_DIR}/Include/Registry.h"
	"${OpenRegistry_SOURCE_DIR}/Include/RegistryString.h"
	"${OpenRegistry_SOURCE_DIR}/Source/Common.h"
	"${OpenRegistry_SOURCE_DIR}/Source/Endian.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmdata.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmlib.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/Dependency/debug.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/Dependency/typedefs.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/Dependency/Wine/unicode.h"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hivedata.h"
)
set(OpenRegistry_SOURCE_FILES
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmcheck.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmheal.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmindex.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cminit.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmkeydel.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmname.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmse.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/cmvalue.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/Dependency/ntoskrnl.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hivebin.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hivecell.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hiveinit.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hivesum.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/hivewrt.c"
	"${OpenRegistry_SOURCE_DIR}/Source/Hive.c"
	"${OpenRegistry_SOURCE_DIR}/Source/Key.c"
	"${OpenRegistry_SOURCE_DIR}/Source/String.c"
	"${OpenRegistry_SOURCE_DIR}/Source/Value.c"
	"${OpenRegistry_SOURCE_DIR}/Source/ValueData.c"
)

add_library(OpenRegistry STATIC
	${OpenRegistry_HEADER_FILES}
	${OpenRegistry_SOURCE_FILES}
)
target_include_directories(OpenRegistry
	PRIVATE "${OpenRegistry_SOURCE_DIR}/Source/ConfigManager/Dependency"
	PRIVATE "${OpenRegistry_SOURCE_DIR}/Source/ConfigManager"
	PUBLIC  "${OpenRegistry_SOURCE_DIR}/Include"
)
target_compile_definitions(OpenRegistry
	PRIVATE CMLIB_HOST=1
	PRIVATE PAGE_SIZE=0x1000
)
set_target_properties(OpenRegistry PROPERTIES
	INTERPROCEDURAL_OPTIMIZATION TRUE
	C_STANDARD 99
)

set_build_dir_default(OpenRegistry)
