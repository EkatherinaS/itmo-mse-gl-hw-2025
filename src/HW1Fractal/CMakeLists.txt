set(SRCS
    main.cpp
    Window.cpp
    Window.h

    Shaders/fractal.fs
    Shaders/fractal.vs


    resources.qrc
)

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(HW1Fractal ${SRCS}
    slidergroup.h slidergroup.cpp
)

target_link_libraries(HW1Fractal
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        FGL::Base
        thirdparty::tinygltf
)
target_link_libraries(HW1Fractal PRIVATE Qt6::Widgets)
