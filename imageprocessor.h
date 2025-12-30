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
#include <QRubberBand>
#include <QRect>
#include "imagetransform.h"
#include "mouseevent.h"
#include "zoomwindow.h"

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
    void openZoomWindow(const QImage &zoomedImage);

private slots:
    void showOpenFile();
    void bigFile();
    void smallFile();
    void showGeometryTransform();
    void enableRegionZoom();

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
    QAction     *regionZoomAction;

    QLabel  *statusLabel;
    QLabel  *MousePosLabel;
    
    QRubberBand *rubberBand;
    QPoint origin;
    bool selectingRegion;

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};
#endif // IMAGEPROCESSOR_H
