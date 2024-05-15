#-------------------------------------------------
#
# Project created by QtCreator 2011-09-04T01:01:32
#
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yumrepomanager

TEMPLATE = app

SOURCES += main.cpp \
    dataflow.cpp \
    repodata.cpp \
    yumrepomanager.cpp \
    globaloptions.cpp \
    repodatamodel.cpp \
    mytableview.cpp \
    createrepowizard.cpp \
    actionspage.cpp \
    progresspage.cpp \
    about.cpp \
    errorpage.cpp \
    proxywidget.cpp \
    metadataexpirewidget.cpp \
    licensedialog.cpp \
    preferences.cpp \
    oldpackages.cpp \
    yumplugin.cpp \
    plugindata.cpp \
    yumpluginmodel.cpp \
    repodatadelegate.cpp \
    mywidget.cpp \
    bandwidthwidget.cpp \
    repodataproxymodel.cpp \
    helperthread.cpp \
    createrepoarguments.cpp \
    texteditor.cpp \
    addrepository.cpp

HEADERS += \
    dataflow.h \
    repodata.h \
    yumrepomanager.h \
    globaloptions.h \
    repodatamodel.h \
    mytableview.h \
    createrepowizard.h \
    actionspage.h \
    progresspage.h \
    about.h \
    errorpage.h \
    proxywidget.h \
    metadataexpirewidget.h \
    licensedialog.h \
    preferences.h \
    oldpackages.h \
    yumplugin.h \
    plugindata.h \
    yumpluginmodel.h \
    repodatadelegate.h \
    mywidget.h \
    repodataproxymodel.h \
    helperthread.h \
    createrepoarguments.h \
    texteditor.h \
    addrepository.h

FORMS += \
    yumrepomanager.ui \
    globaloptions.ui \
    createrepowizard.ui \
    about.ui \
    licensedialog.ui \
    preferences.ui \
    oldpackages.ui \
    yumplugin.ui \
    createrepoarguments.ui \
    texteditor.ui \
    addrepository.ui

RESOURCES += \
    Images.qrc \
    Translations.qrc

TRANSLATIONS = Translations/spanish_es.ts
