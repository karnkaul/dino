
# Interface
set(COMPILE_DEFS
	_UNICODE
	$<$<NOT:$<CONFIG:Debug>>:NDEBUG RELEASE>
	$<$<CONFIG:Debug>:DEBUG>
	$<$<BOOL:${MSVC_RUNTIME}>:WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS>
)
set(CLANG_COMMON -Wconversion -Wunreachable-code -Wdeprecated-declarations -Wtype-limits -Wunused -Wno-unknown-pragmas)
if(LINUX_GCC OR LINUX_CLANG OR WIN64_GCC OR WIN64_CLANG)
	set(COMPILE_OPTS
		-Wextra
		-Werror=return-type
		$<$<NOT:$<CONFIG:Debug>>:-Werror>
		$<$<NOT:$<BOOL:${WIN64_CLANG}>>:-fexceptions>
		$<$<OR:$<BOOL:${LINUX_GCC}>,$<BOOL:${LINUX_CLANG}>>:-Wall>
		$<$<OR:$<BOOL:${LINUX_GCC}>,$<BOOL:${WIN64_GCC}>>:-utf-8 -Wno-unknown-pragmas>
		$<$<OR:$<BOOL:${LINUX_CLANG}>,$<BOOL:${WIN64_CLANG}>>:${CLANG_COMMON}>
	)
elseif(WIN64_MSBUILD)
	set(COMPILE_OPTS
		$<$<NOT:$<CONFIG:Debug>>:/WX>
		/MP
	)
endif()
if(WIN64_CLANG AND MSVC)
	list(APPEND COMPILE_OPTS /W4 -utf-8)
endif()
if(PLATFORM STREQUAL "Linux")
	list(APPEND COMPILE_OPTS -fPIC)
	set(LINK_OPTS
		-no-pie         # Build as application
		-Wl,-z,origin   # Allow $ORIGIN in RUNPATH
	)
elseif(PLATFORM STREQUAL "Win64" AND NOT WIN64_GCC)
	if(MSVC)
		set(LINK_OPTS
			/ENTRY:mainCRTStartup                              # Link to main() and not WinMain()
			/SUBSYSTEM:$<IF:$<CONFIG:Debug>,CONSOLE,WINDOWS>   # Spawn a console in Debug
		)
	else()
		set(LINK_OPTS -Wl,/SUBSYSTEM:$<IF:$<CONFIG:Debug>,CONSOLE,WINDOWS>,/ENTRY:mainCRTStartup)
	endif()
endif()
set(COMPILE_FTRS cxx_std_17)
set(LINK_LIBS $<$<STREQUAL:${PLATFORM},Linux>:pthread stdc++fs dl>)
add_library(interface-lib INTERFACE)
target_compile_features(interface-lib INTERFACE ${COMPILE_FTRS})
target_compile_definitions(interface-lib INTERFACE ${COMPILE_DEFS})
target_compile_options(interface-lib INTERFACE ${COMPILE_OPTS})
target_link_options(interface-lib INTERFACE ${LINK_OPTS})
target_link_libraries(interface-lib INTERFACE ${LINK_LIBS})
