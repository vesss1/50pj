#ifndef IMAGETRANSFORM_H
#define IMAGETRANSFORM_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDial>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>

class ImageTransform : public QWidget
{
    Q_OBJECT

public:
    ImageTransform(QWidget *parent = nullptr);
    ~ImageTransform();

    QLabel          *inWin;
    QGroupBox       *mirrorGroup;
    QCheckBox       *hCheckBox;
    QCheckBox       *vCheckBox;
    QPushButton     *mirrorButton;
    QDial           *rotateDial;
    QSpacerItem     *vSpacer;
    QHBoxLayout     *mainLayout;
    QVBoxLayout     *groupLayout;
    QVBoxLayout     *leftLayout;
    QImage          srcImg;
    QImage          dstImg;

    QPushButton     *saveButton;

private slots:
    void mirroredImage();
    void rotatedImage();
    void saveImage();
};
#endif // IMAGETRANSFORM_H
