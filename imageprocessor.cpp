#include "imageprocessor.h"
#include "imagetransform.h"
#include "zoomwindow.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollArea>

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent), rubberBand(nullptr), selectingRegion(false)
{
    setWindowTitle(tr("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300,200);
    gWin = new ImageTransform();
    initPixmap->fill(QColor(255,255,255));
    imgWin->resize(300,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    createActions();
    createMenus();
    createToolBars();

    statusLabel = new QLabel;
    statusLabel->setText(tr("指標位置:"));
    statusLabel->setFixedWidth(100);
    MousePosLabel = new QLabel;
    MousePosLabel->setText(tr(" "));
    MousePosLabel->setFixedWidth(100);
    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(MousePosLabel);
    setMouseTracking(true);
    imgWin->setMouseTracking(true);
    central->setMouseTracking(true);

}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::createActions()
{
    openFileAction = new QAction(tr("開啟檔案&O"),this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(tr("開啟影像檔案"));
    connect(openFileAction,SIGNAL(triggered(bool)),this,SLOT(showOpenFile()));

    exitAction = new QAction(tr("結束&Q"),this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("退出程式"));
    connect(exitAction,SIGNAL(triggered(bool)),this,SLOT(close()));

    big = new QAction(tr("放大"),this);
    exitAction->setStatusTip(tr("放大"));
    connect(big,SIGNAL(triggered(bool)),this,SLOT(bigFile()));

    small = new QAction(tr("縮小"),this);
    exitAction->setStatusTip(tr("縮小"));
    connect(small,SIGNAL(triggered(bool)),this,SLOT(smallFile()));
    
    regionZoomAction = new QAction(tr("區域放大"),this);
    regionZoomAction->setShortcut(tr("Ctrl+Z"));
    regionZoomAction->setStatusTip(tr("選取區域放大"));
    regionZoomAction->setCheckable(true);
    connect(regionZoomAction,SIGNAL(triggered(bool)),this,SLOT(enableRegionZoom()));

    geometryAction = new QAction(tr("幾何轉換"),this);
    geometryAction->setShortcut(tr("Ctrl+G"));
    geometryAction->setStatusTip(tr("影像幾何轉換"));
    connect(geometryAction,SIGNAL(triggered(bool)),this,SLOT(showGeometryTransform()));
    connect(exitAction,SIGNAL(triggered(bool)),gWin,SLOT(close()));
}

void ImageProcessor::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("檔案&F"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(exitAction);

    fileMenu = menuBar()->addMenu(tr("工具&T"));
    fileMenu->addAction(big);
    fileMenu->addAction(small);
    fileMenu->addAction(regionZoomAction);

    fileMenu->addAction(geometryAction);
}

void ImageProcessor::createToolBars()
{
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);

    fileTool = addToolBar("tool");
    fileTool->addAction(big);
    fileTool->addAction(small);
    fileTool->addAction(regionZoomAction);

    fileTool->addAction(geometryAction);
}

void ImageProcessor::loadFile(QString filename)
{
    qDebug()<<QString("file name:%1").arg(filename);
    QByteArray ba = filename.toLatin1();
    printf("FN:%s\n",(char *) ba.data());
    img.load(filename);
    imgWin->setPixmap(QPixmap::fromImage(img));
}

void ImageProcessor::showOpenFile()
{
    filename = QFileDialog::getOpenFileName(this,tr("開啟影像"),tr("."),"bmp(*.bmp);;png(*.png)"";;Jpng(*.jpg)");
    if(!filename.isEmpty()){
        if(img.isNull()){
            loadFile(filename);
        }else{
            ImageProcessor *newIPWin = new ImageProcessor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}

void ImageProcessor::bigFile()
{
    if(!filename.isEmpty()){
        ImageProcessor *newIPWin = new ImageProcessor();
        newIPWin->show();
        newIPWin->loadFile(filename);
    }
}
void ImageProcessor::smallFile()
{
    if(!filename.isEmpty()){
        ImageProcessor *newIPWin = new ImageProcessor();
        newIPWin->show();
        newIPWin->loadFile(filename);
    }
}

void ImageProcessor::showGeometryTransform(){
    if(!img.isNull())
        gWin->srcImg=img;
    gWin->inWin->setPixmap(QPixmap::fromImage(gWin->srcImg));
    gWin->show();
}

void ImageProcessor::enableRegionZoom()
{
    selectingRegion = regionZoomAction->isChecked();
    if (selectingRegion) {
        statusBar()->showMessage(tr("區域放大模式：拖曳滑鼠選取區域"), 3000);
        imgWin->setCursor(Qt::CrossCursor);
    } else {
        statusBar()->showMessage(tr("取消區域放大模式"), 3000);
        imgWin->setCursor(Qt::ArrowCursor);
        if (rubberBand) {
            rubberBand->hide();
        }
    }
}

void ImageProcessor::openZoomWindow(const QImage &zoomedImage)
{
    ZoomWindow *zoomWin = new ZoomWindow(zoomedImage);
    zoomWin->setAttribute(Qt::WA_DeleteOnClose);
    zoomWin->show();
}

void ImageProcessor::mouseDoubleClickEvent(QMouseEvent *event){
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    statusBar()->showMessage(tr("雙擊:")+str,1000);
}
void ImageProcessor::mouseMoveEvent(QMouseEvent *event){
    // Convert window coordinates to image label coordinates
    QPoint labelPos = imgWin->mapFrom(this, event->pos());
    
    // Check if the position is within the image label bounds
    if (!img.isNull() && imgWin->rect().contains(labelPos) 
        && labelPos.x() >= 0 && labelPos.y() >= 0) {
        
        // Calculate the actual image coordinates considering scaling
        double scaleX = (double)img.width() / imgWin->width();
        double scaleY = (double)img.height() / imgWin->height();
        
        int imgX = (int)(labelPos.x() * scaleX);
        int imgY = (int)(labelPos.y() * scaleY);
        
        // Ensure coordinates are within image bounds
        if (imgX >= 0 && imgX < img.width() && imgY >= 0 && imgY < img.height()) {
            int gray = qGray(img.pixel(imgX, imgY));
            QString str = "(" + QString::number(imgX) +", " + QString::number(imgY) + ")" + " = "+QString::number(gray);
            MousePosLabel->setText(str);
        }
    }
    
    if (selectingRegion && rubberBand && !origin.isNull()) {
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    }
}
void ImageProcessor::mousePressEvent(QMouseEvent *event){
    if (selectingRegion && event->button() == Qt::LeftButton) {
        origin = event->pos();
        if (!rubberBand) {
            rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        }
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
        return;
    }
    
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    if(event->button()==Qt::LeftButton){
        statusBar()->showMessage(tr("左鍵:")+str,1000);
    }
    else if(event->button()==Qt::RightButton){
        statusBar()->showMessage(tr("右鍵:")+str,1000);
    }
    else if(event->button()==Qt::MiddleButton){
        statusBar()->showMessage(tr("中鍵:")+str,1000);
    }
}
void ImageProcessor::mouseReleaseEvent(QMouseEvent *event){
    if (selectingRegion && event->button() == Qt::LeftButton && rubberBand) {
        rubberBand->hide();
        
        QRect selectedRect = rubberBand->geometry();
        
        // Convert window coordinates to image label coordinates
        QPoint labelTopLeft = imgWin->mapFrom(this, selectedRect.topLeft());
        QPoint labelBottomRight = imgWin->mapFrom(this, selectedRect.bottomRight());
        QRect labelRect(labelTopLeft, labelBottomRight);
        
        // Check if selection is valid and within image bounds
        if (labelRect.width() > 10 && labelRect.height() > 10 && !img.isNull()) {
            // Calculate the actual image coordinates
            double scaleX = (double)img.width() / imgWin->width();
            double scaleY = (double)img.height() / imgWin->height();
            
            int imgX = qMax(0, (int)(labelRect.x() * scaleX));
            int imgY = qMax(0, (int)(labelRect.y() * scaleY));
            int imgW = qMin(img.width() - imgX, (int)(labelRect.width() * scaleX));
            int imgH = qMin(img.height() - imgY, (int)(labelRect.height() * scaleY));
            
            if (imgW > 0 && imgH > 0) {
                // Ask for zoom ratio
                bool ok;
                double zoomRatio = QInputDialog::getDouble(this, tr("放大倍率"),
                    tr("請輸入放大倍率:"), 2.0, 0.1, 10.0, 1, &ok);
                
                if (ok) {
                    // Extract the selected region
                    QImage selectedRegion = img.copy(imgX, imgY, imgW, imgH);
                    
                    // Scale the image
                    int newWidth = (int)(selectedRegion.width() * zoomRatio);
                    int newHeight = (int)(selectedRegion.height() * zoomRatio);
                    QImage zoomedImage = selectedRegion.scaled(newWidth, newHeight, 
                        Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    
                    // Open zoom window
                    openZoomWindow(zoomedImage);
                }
            }
        }
        
        origin = QPoint();
        regionZoomAction->setChecked(false);
        enableRegionZoom(); // Use the slot to ensure consistent state management
        return;
    }
    
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    statusBar()->showMessage(tr("釋放:")+str,1000);
}
