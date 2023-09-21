QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    controller/astar.cpp \
    View/graphical/background.cpp \
    controller/command.cpp \
    controller/controller.cpp \
    View/graphical/enemytile.cpp \
    model/gameWorld.cpp \
    View/graphical/graphicView.cpp \
    View/graphical/healthsupply.cpp \
    View/graphical/penemytile.cpp \
    main.cpp \
    View/graphical/pathtile.cpp \
    View/graphical/protagonisttile.cpp \
    View/text/textview.cpp \
    View/mainview.cpp \
    model/xenemy.cpp \
    View/graphical/xenemypixel.cpp \
    controller/pathfinder.cpp \
    View/graphical/ActorTile.cpp \
    View/graphical/healthindicator.cpp

HEADERS += \
    controller/astar.h \
    View/graphical/background.h \
    View/graphical/ActorTile.h \
    controller/command.h \
    controller/controller.h \
    View/graphical/enemytile.h \
    model/gameWorld.h \
    View/graphical/graphicView.h \
    View/graphical/healthindicator.h \
    View/graphical/healthsupply.h \
    controller/node.h \
    controller/pathfinder.h \
    View/graphical/pathtile.h \
    View/graphical/penemytile.h \
    View/graphical/protagonisttile.h \
    View/scene.h \
    View/text/textview.h \
    View/mainview.h \
    model/xenemy.h \
    View/graphical/xenemypixel.h

FORMS += \
    View/mainview.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#INCLUDEPATH += lib
#DEPENDPATH += lib

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lworld
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lworld
#else:unix: LIBS += -L$$PWD/lib/ -lworld

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    images.qrc

unix:!macx: LIBS += -L$$PWD/lib/ -lworld

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libworld.a

macx: LIBS += -L$$PWD/lib/ -lworld

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

macx: LIBS += -L$$PWD/lib/ -lworld

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
