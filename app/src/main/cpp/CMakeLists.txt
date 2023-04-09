# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("theptavpn")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library( event2
        STATIC
        IMPORTED)

set_target_properties( event2
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libevent/libs/${ANDROID_ABI}/libevent2.a)

include_directories("./libevent/include")
include_directories(".")


#set(ANDROID_STL "c++_shared")


add_library( # Sets the name of the library.
        theptavpn

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        Sock5Client.cpp
        ThreadSafeQueue.cpp
        Tcp.cpp
        utils.cpp
        Udp.cpp
        Icmp.cpp
        Tcb.cpp
        IP.cpp
        ProxyServer.cpp
        entry.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        theptavpn
        event2
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})