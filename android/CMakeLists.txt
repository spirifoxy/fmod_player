cmake_minimum_required( VERSION 3.4.1 )

set( LIB_FMOD ${FMOD_API_ROOT}/core/lib/${ANDROID_ABI}/libfmod${FMOD_LIB_SUFFIX}.so )

add_library( fmod SHARED IMPORTED )

set_target_properties(
        fmod PROPERTIES
        IMPORTED_LOCATION ${LIB_FMOD}
        INTERFACE_INCLUDE_DIRECTORIES ${FMOD_API_ROOT}/core/inc
)

add_library(
         fmod_player

         # Sets the library as a shared library.
         SHARED

         # Provides a relative path to your source file(s).
         ../ios/Classes/common.c
         ../ios/Classes/fmod_player.c
)

target_include_directories(
        fmod_player

        PRIVATE

        ../lib/core/inc
)

target_link_libraries( fmod_player fmod )
