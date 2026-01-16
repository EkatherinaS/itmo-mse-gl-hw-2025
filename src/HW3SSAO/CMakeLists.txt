set(SRCS
    main.cpp
    Window.cpp
    Window.h

    model.h model.cpp
    camera.h camera.cpp

    Models/portal_gun_with_download.glb

    Shaders/ssao.fs
    Shaders/ssao.vs

    Shaders/quad.fs
    Shaders/quad.vs

    Shaders/geometry.fs
    Shaders/geometry.vs

    Shaders/output.fs
    Shaders/output.vs
)

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(HW3SSAO ${SRCS} ${RES}
    resources.qrc
    light.h light.cpp
    slidergroup.h slidergroup.cpp

)

target_link_libraries(HW3SSAO
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        FGL::Base
        thirdparty::tinygltf
)
target_link_libraries(HW3SSAO PRIVATE Qt6::Widgets)
