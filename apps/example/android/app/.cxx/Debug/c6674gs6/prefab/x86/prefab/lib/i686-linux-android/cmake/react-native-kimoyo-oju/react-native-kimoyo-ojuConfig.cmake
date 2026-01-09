if(NOT TARGET react-native-kimoyo-oju::KimoyoOjuNitro)
add_library(react-native-kimoyo-oju::KimoyoOjuNitro SHARED IMPORTED)
set_target_properties(react-native-kimoyo-oju::KimoyoOjuNitro PROPERTIES
    IMPORTED_LOCATION "/Users/mikevocalz/viro-nitro/packages/react-native-kimoyo-oju/android/build/intermediates/cxx/Debug/1d40672t/obj/x86/libKimoyoOjuNitro.so"
    INTERFACE_INCLUDE_DIRECTORIES "/Users/mikevocalz/viro-nitro/packages/react-native-kimoyo-oju/cpp"
    INTERFACE_LINK_LIBRARIES ""
)
endif()

