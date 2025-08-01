set(CMAKE_CXX_STANDARD 17)

if (BUILD_TESTING)
    add_subdirectory(googletest)

#    Otherwise there would be lwip from pico sdk
#    add_library(lwip STATIC lwip)
    add_subdirectory(lwip)
endif ()
        
