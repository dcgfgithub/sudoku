# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             appjni

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/jni/app_jni.c
             src/main/cpp/sw/and/and_dbg.c
             src/main/cpp/sw/and/and_tex.c
             src/main/cpp/sw/com/com_anm.c
             src/main/cpp/sw/com/com_app.c
             src/main/cpp/sw/com/com_def.c
             src/main/cpp/sw/com/com_list.c
             src/main/cpp/sw/com/com_msg.c
             src/main/cpp/sw/com/com_rnd.c
             src/main/cpp/sw/com/com_spr.c
             src/main/cpp/sw/ogl/ogl_gfx.c
             src/main/cpp/sw/ogl/ogl_img.c
             src/main/cpp/sw/ogl/ogl_quad.c
             src/main/cpp/sw/ogl/ogl_spr.c
             src/main/cpp/sw/ogl/ogl_tex.c
             src/main/cpp/sw/ogl/ogl_tran.c
             src/main/cpp/sw_app.c
             src/main/cpp/src/tex_main.c
             src/main/cpp/src/solver.c
             src/main/cpp/src/sudoku.c
             src/main/cpp/src/sv.c )

include_directories( src/main/cpp/ )
include_directories( src/main/cpp/src/ )
include_directories( src/main/cpp/jni/ )
include_directories( src/main/cpp/sw/ )
include_directories( src/main/cpp/sw/com )
include_directories( src/main/cpp/sw/and )
include_directories( src/main/cpp/sw/ogl )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )
              
find_library( # Sets the name of the path variable.
              gles1-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              GLESv1_CM )              

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       appjni

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} ${gles1-lib} )