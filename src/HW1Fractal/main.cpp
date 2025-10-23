#include "window.h"

#include <QApplication>
#include <QSurfaceFormat>

namespace
{
constexpr auto g_samples = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;
}

int main(int argc, char * argv[])
{
    // Create app and set attributes.
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);

    // Set default surface format.
    QSurfaceFormat format;
    format.setSamples(g_samples);
    format.setVersion(g_gl_major_version, g_gl_minor_version);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Now create window.
    Window w;
    w.resize(640, 480);
    w.show();

    return a.exec();
}
