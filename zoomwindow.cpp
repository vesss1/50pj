#include "zoomwindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QMessageBox>

ZoomWindow::ZoomWindow(const QImage &image, QWidget *parent)
    : QWidget(parent), originalImage(image), drawingImage(image),
      drawing(false), isDrawingMode(false), penColor(Qt::red), penWidth(3)
{
    setWindowTitle(tr("放大視窗"));
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create toolbar
    toolBar = new QToolBar(this);
    
    // Create actions
    saveAction = new QAction(tr("另存新檔"), this);
    saveAction->setStatusTip(tr("儲存影像"));
    connect(saveAction, &QAction::triggered, this, &ZoomWindow::saveImage);
    
    drawAction = new QAction(tr("畫筆模式"), this);
    drawAction->setCheckable(true);
    drawAction->setStatusTip(tr("啟用畫筆功能"));
    connect(drawAction, &QAction::triggered, this, &ZoomWindow::enableDrawing);
    
    colorAction = new QAction(tr("選擇顏色"), this);
    colorAction->setStatusTip(tr("選擇畫筆顏色"));
    connect(colorAction, &QAction::triggered, this, &ZoomWindow::choosePenColor);
    
    // Add pen width spinbox
    penWidthSpinBox = new QSpinBox(this);
    penWidthSpinBox->setRange(1, 50);
    penWidthSpinBox->setValue(penWidth);
    penWidthSpinBox->setPrefix(tr("筆寬: "));
    connect(penWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ZoomWindow::changePenWidth);
    
    // Add actions to toolbar
    toolBar->addAction(saveAction);
    toolBar->addSeparator();
    toolBar->addAction(drawAction);
    toolBar->addAction(colorAction);
    toolBar->addWidget(penWidthSpinBox);
    
    mainLayout->addWidget(toolBar);
    
    // Create scroll area for image
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(false);
    
    // Create label to display image
    imageLabel = new QLabel(this);
    imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
    imageLabel->adjustSize();
    
    scrollArea->setWidget(imageLabel);
    mainLayout->addWidget(scrollArea);
    
    setLayout(mainLayout);
    resize(800, 600);
    
    setMouseTracking(true);
    imageLabel->setMouseTracking(true);
}

ZoomWindow::~ZoomWindow()
{
}

void ZoomWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("另存影像"), "",
        tr("PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp)"));
    
    if (!fileName.isEmpty()) {
        if (drawingImage.save(fileName)) {
            QMessageBox::information(this, tr("成功"), tr("影像已儲存"));
        } else {
            QMessageBox::warning(this, tr("錯誤"), tr("無法儲存影像"));
        }
    }
}

void ZoomWindow::enableDrawing()
{
    isDrawingMode = drawAction->isChecked();
    if (isDrawingMode) {
        setCursor(Qt::CrossCursor);
        imageLabel->setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        imageLabel->setCursor(Qt::ArrowCursor);
    }
}

void ZoomWindow::choosePenColor()
{
    QColor color = QColorDialog::getColor(penColor, this, tr("選擇畫筆顏色"));
    if (color.isValid()) {
        penColor = color;
    }
}

void ZoomWindow::changePenWidth(int width)
{
    penWidth = width;
}

void ZoomWindow::mousePressEvent(QMouseEvent *event)
{
    if (isDrawingMode && event->button() == Qt::LeftButton) {
        // Convert widget coordinates to image label coordinates
        QPoint labelPos = imageLabel->mapFrom(this, event->pos());
        if (imageLabel->rect().contains(labelPos)) {
            drawing = true;
            lastPoint = labelPos;
        }
    }
}

void ZoomWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawingMode && drawing && (event->buttons() & Qt::LeftButton)) {
        QPoint labelPos = imageLabel->mapFrom(this, event->pos());
        if (imageLabel->rect().contains(labelPos)) {
            QPainter painter(&drawingImage);
            painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawLine(lastPoint, labelPos);
            lastPoint = labelPos;
            
            imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
        }
    }
}

void ZoomWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDrawingMode && event->button() == Qt::LeftButton && drawing) {
        drawing = false;
    }
}

void ZoomWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}
