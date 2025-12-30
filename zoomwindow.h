#ifndef ZOOMWINDOW_H
#define ZOOMWINDOW_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QColorDialog>
#include <QSpinBox>

class ZoomWindow : public QWidget
{
    Q_OBJECT

public:
    ZoomWindow(const QImage &image, QWidget *parent = nullptr);
    ~ZoomWindow();

private slots:
    void saveImage();
    void enableDrawing();
    void choosePenColor();
    void changePenWidth(int width);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QImage originalImage;
    QImage drawingImage;
    QLabel *imageLabel;
    QToolBar *toolBar;
    QAction *saveAction;
    QAction *drawAction;
    QAction *colorAction;
    QSpinBox *penWidthSpinBox;
    
    bool drawing;
    bool isDrawingMode;
    QPoint lastPoint;
    QColor penColor;
    int penWidth;
};

#endif // ZOOMWINDOW_H
