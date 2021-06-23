
# Interface
if(LINUX_GCC OR LINUX_CLANG OR WIN64_GCC OR WIN64_CLANG)
  set(GCC_CLANG TRUE)
else()
  set(GCC_CLANG FALSE)
endif()

set(COMPILE_OPTS
  $<$<BOOL:${GCC_CLANG}>:-Wall -Wextra -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>>
  $<$<BOOL:${WIN64_MSBUILD}>:/MP $<$<NOT:$<CONFIG:Debug>>:/WX>>
)

add_library(dino-interface INTERFACE)
add_library(dino::interface ALIAS dino-interface)
target_compile_features(dino-interface INTERFACE cxx_std_17)
target_compile_definitions(dino-interface INTERFACE $<$<BOOL:${MSVC_RUNTIME}>:WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS>)
target_compile_options(dino-interface INTERFACE ${COMPILE_OPTS})
