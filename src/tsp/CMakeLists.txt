# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

add_subdirectory(common)
add_subdirectory(asymmetric)

if(CXX11_ENABLED)
add_subdirectory(symmetric)
endif()
