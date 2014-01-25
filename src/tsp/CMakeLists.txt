# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

add_subdirectory(common)

if(CXX11_ENABLED)
add_subdirectory(asymmetric)
add_subdirectory(symmetric)
endif()
