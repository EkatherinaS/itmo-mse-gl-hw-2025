set(SRCS
    main.cpp
    Window.cpp
    Window.h

    Shaders/diffuse.fs
    Shaders/diffuse.vs
    Textures/voronoi.png

    resources.qrc
)

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)

add_executable(demo-app ${SRCS})

target_link_libraries(demo-app
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        FGL::Base
        thirdparty::tinygltf
)
