

#include "SWDisplayImageWidget.h"

#include "moc_SWDisplayImageWidget.cpp"

#include <QResizeEvent>
#include <iostream>

using namespace std;

SWDisplayImageWidget::SWDisplayImageWidget(QWidget* oParent, const bool bScaleImage, const bool bActiveSelectPixelMode) : m_oSize(size()), m_bScaleImage(bScaleImage), m_bActiveSelectPixelMode(bActiveSelectPixelMode)
{
	this->setParent(oParent);
}

SWDisplayImageWidget::~SWDisplayImageWidget()
{}

void SWDisplayImageWidget::paintEvent(QPaintEvent *)
{
	// update  image
	QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(m_oQImage.isNull())
    {
        return;
    }

    if(m_bScaleImage)
    {
        m_oScaledImage = m_oQImage.scaled(m_oSize,
                      Qt::KeepAspectRatio
                      ,Qt::SmoothTransformation
                      );
    }
    else
    {
        m_oScaledImage = m_oQImage;
    }

    if(m_bActiveSelectPixelMode)
    {
        for(int ii = 0; ii < m_vClickedPoints.size(); ++ii)
        {
            float l_fScaledValueY = m_vCurrentSize[ii].width()*1.f  / m_oScaledImage.width();
            float l_fScaledValueX = m_vCurrentSize[ii].height()*1.f / m_oScaledImage.height();
            int l_i32PixelX = m_vClickedPoints[ii].x() / l_fScaledValueX;
            int l_i32PixelY = m_vClickedPoints[ii].y() / l_fScaledValueY;

            QRgb l_oCurrentColor = m_oScaledImage.pixel(l_i32PixelX, l_i32PixelY);
            QColor l_oColor(l_oCurrentColor);
            int l_i32RedValue = l_oColor.red();
            l_i32RedValue += 100;
            if(l_i32RedValue + 100 > 255)
            {
                l_i32RedValue = 255;
            }

            l_oColor.setRed(l_i32RedValue);

            m_oScaledImage.setPixel(l_i32PixelX, l_i32PixelY, l_oColor.rgb());

        }
    }

    painter.drawImage(QPoint(), m_oScaledImage);
}

void SWDisplayImageWidget::mousePressEvent(QMouseEvent *event)
{
    m_bMouseClicked = true;
}

void SWDisplayImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMouseClicked = false;
}

void SWDisplayImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bMouseClicked && m_bActiveSelectPixelMode)
    {
        int l_i32SizeBrush = 3;

        if(event->pos().x() < m_oScaledImage.width()-l_i32SizeBrush  && event->pos().x() > l_i32SizeBrush &&
           event->pos().y() < m_oScaledImage.height()-l_i32SizeBrush && event->pos().y() > l_i32SizeBrush)
        {
            for(int ii = event->pos().x()-l_i32SizeBrush; ii < event->pos().x()+l_i32SizeBrush; ++ii)
            {
                for(int jj = event->pos().y()-l_i32SizeBrush; jj < event->pos().y()+l_i32SizeBrush; ++jj)
                {
                    m_vClickedPoints.push_back(QPoint(ii,jj));
                    m_vCurrentSize.push_back(m_oScaledImage.size());
                    emit clickPoint(QPoint(ii,jj), m_oScaledImage.size());
                }
            }

            update();
        }
    }
}

void SWDisplayImageWidget::resizeEvent ( QResizeEvent * event )
{
    m_oSize = event->size();
    resize(m_oSize);
}


void SWDisplayImageWidget::refreshDisplay(const QImage &oQImage)
{
	m_oQImage = oQImage.copy();
    update();
}

void SWDisplayImageWidget::resetSelectedPoints()
{
    m_vClickedPoints.clear();
    m_vCurrentSize.clear();
    update();
}
