set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(SOURCE_DIR "${ROOT_DIR}/src")
set(SOURCE_FILES
	"${SOURCE_DIR}/Hooks.cpp"
	"${SOURCE_DIR}/Hooks.h"
	"${SOURCE_DIR}/HUDHandler.cpp"
	"${SOURCE_DIR}/HUDHandler.h"
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/ModAPI.cpp"
	"${SOURCE_DIR}/ModAPI.h"
	"${SOURCE_DIR}/Offsets.h"
	"${SOURCE_DIR}/Papyrus.cpp"
	"${SOURCE_DIR}/Papyrus.h"
	"${SOURCE_DIR}/PCH.h"
	"${SOURCE_DIR}/Settings.cpp"
	"${SOURCE_DIR}/Settings.h"
	"${SOURCE_DIR}/TrueHUDAPI.h"
	"${SOURCE_DIR}/Utils.cpp"
	"${SOURCE_DIR}/Utils.h"
	"${SOURCE_DIR}/Scaleform/Scaleform.cpp"
	"${SOURCE_DIR}/Scaleform/Scaleform.h"
	"${SOURCE_DIR}/Scaleform/TrueHUDMenu.cpp"
	"${SOURCE_DIR}/Scaleform/TrueHUDMenu.h"
	"${SOURCE_DIR}/Widgets/ActorInfoBar.cpp"
	"${SOURCE_DIR}/Widgets/ActorInfoBar.h"
	"${SOURCE_DIR}/Widgets/BossInfoBar.cpp"
	"${SOURCE_DIR}/Widgets/BossInfoBar.h"
	"${SOURCE_DIR}/Widgets/FloatingText.cpp"
	"${SOURCE_DIR}/Widgets/FloatingText.h"
	"${SOURCE_DIR}/Widgets/InfoBarBase.cpp"
	"${SOURCE_DIR}/Widgets/InfoBarBase.h"
	"${SOURCE_DIR}/Widgets/PlayerWidget.cpp"
	"${SOURCE_DIR}/Widgets/PlayerWidget.h"
	"${SOURCE_DIR}/Widgets/RecentLoot.cpp"
	"${SOURCE_DIR}/Widgets/RecentLoot.h"
	"${SOURCE_DIR}/Widgets/ShoutIndicator.cpp"
	"${SOURCE_DIR}/Widgets/ShoutIndicator.h"
)

source_group(TREE "${ROOT_DIR}" FILES ${SOURCE_FILES})

set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"${ROOT_DIR}/cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)

source_group("src" FILES "${VERSION_HEADER}")

configure_file(
	"${ROOT_DIR}/cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCE_FILES}
	"${VERSION_HEADER}"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	"${ROOT_DIR}/.clang-format"
	"${ROOT_DIR}/.editorconfig"
)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_20
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/src"
		"${SOURCE_DIR}"
)

set(SKSE_SUPPORT_XBYAK ON)

if(BUILD_SKYRIM_AE)
	add_compile_definitions(IS_SKYRIM_AE)
	add_subdirectory("$ENV{CommonLibSSEPath_AE}" CommonLibSSE EXCLUDE_FROM_ALL)
else()
	add_subdirectory("$ENV{CommonLibSSEPath_SE}" CommonLibSSE EXCLUDE_FROM_ALL)
endif(BUILD_SKYRIM_AE)

find_package(xbyak REQUIRED CONFIG)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		CommonLibSSE::CommonLibSSE
		xbyak::xbyak
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		"${SOURCE_DIR}/PCH.h"
)

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT "main"
)

install(
	FILES
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "/"
	COMPONENT "pdbs"
)

if("${COPY_OUTPUT}")
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		VERBATIM
	)
endif()
