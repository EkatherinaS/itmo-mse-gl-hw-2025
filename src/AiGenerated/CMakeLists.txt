set(SRCS
    Camera.cpp
    Camera.h
    Entity.cpp
    Entity.h
    main.cpp
    ModelEntity.cpp
    ModelEntity.h
    OpenGLContext.cpp
    OpenGLContext.h
    SceneGraph.cpp
    SceneGraph.h
    SceneRenderer.cpp
    SceneRenderer.h
    SkyboxEntity.cpp
    SkyboxEntity.h
    Window.cpp
    Window.h

    Shaders/model.fs
    Shaders/model.vs
    Shaders/skybox.fs
    Shaders/skybox.vs

    Models/sponza.glb
)

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_big_resources(RES resources.qrc)

add_executable(demo-app-ai ${SRCS} ${RES})

target_link_libraries(demo-app-ai
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGLWidgets
        FGL::Base
        thirdparty::tinygltf
)
