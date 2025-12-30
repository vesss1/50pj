#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QStatusBar>
#include "imagetransform.h"
#include "mouseevent.h"

class ImageProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ImageProcessor(QWidget *parent = nullptr);
    ~ImageProcessor();

    void createActions();
    void createMenus();
    void createToolBars();
    void loadFile(QString filename);

private slots:
    void showOpenFile();
    void bigFile();
    void smallFile();
    void showGeometryTransform();

private:
    ImageTransform *gWin;
    MouseEvent     *mouse;
    QWidget     *central;
    QMenu       *fileMenu;
    QToolBar    *fileTool;
    QImage      img;
    QString     filename;
    QLabel      *imgWin;
    QAction     *openFileAction;
    QAction     *exitAction;
    QAction     *geometryAction;

    QAction     *big;
    QAction     *small;

    QLabel  *statusLabel;
    QLabel  *MousePosLabel;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};
#endif // IMAGEPROCESSOR_H
