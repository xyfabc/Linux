#include "LoadFile.h"
#include "mainwindow.h"
#include "QFileDialog"

LoadFileTh::LoadFileTh()
{
}

void LoadFileTh::run()
{
    MainWindow::m_self->LoadFile();
    emit runOver();
}
