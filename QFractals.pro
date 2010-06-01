# -------------------------------------------------
# Project created by QtCreator 2009-12-11T21:48:34
# -------------------------------------------------
TARGET = QFractals
TEMPLATE = app
SOURCES += main.cpp \
    graphics/colorpalette.cpp \
    math/transformation.cpp \
    math/spline.cpp \
    graphics/imagewrapper.cpp \
    graphics/renderedimage.cpp \
    graphics/plottedimage.cpp \
    ui/selectablewidget.cpp \
    settings.cpp \
    ui/mainwindow.cpp \
    graphics/image.cpp \
    fractals/specification.cpp \
    fractals/viewport.cpp \
    fractals/rendering.cpp \
    fractals/mandelbrot.cpp \
    ui/imagecontrolwidget.cpp \
    ui/resizedialog.cpp \
    fractals/pendulum.cpp \
    fractals/plotting.cpp \
    fractals/ifs.cpp \
    math/interpreter.cpp
HEADERS += graphics/colorpalette.h \
    math/transformation.h \
    math/spline.h \
    graphics/imagewrapper.h \
    graphics/renderedimage.h \
    graphics/plottedimage.h \
    graphics/image.h \
    ui/selectablewidget.h \
    settings.h \
    ui/mainwindow.h \
    fractals/specification.h \
    fractals/viewport.h \
    fractals/rendering.h \
    fractals/mandelbrot.h \
    fractals/plotting.h \
    fractals/ifs.h \
    ui/imagecontrolwidget.h \
    ui/resizedialog.h \
    fractals/pendulum.h \
    math/interpreter.h
FORMS +=
CONFIG += debug
#LIBS += -L/opt/local/lib/ \
#    -lgmpxx \
#    -lgmp
INCLUDEPATH += /opt/local/include/
