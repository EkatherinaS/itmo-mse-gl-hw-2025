set(BASE_SRCS
        GLWidget.cpp
        GLWidget.hpp
        )

add_library(Base ${BASE_SRCS})

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)

target_link_libraries(Base
        PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        )

add_library(FGL::Base ALIAS Base)
