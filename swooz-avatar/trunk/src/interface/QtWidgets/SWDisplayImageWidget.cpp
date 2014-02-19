

#include "SWDisplayImageWidget.h"

#include "moc_SWDisplayImageWidget.cpp"

#include <QResizeEvent>
#include <iostream>

using namespace std;

SWDisplayImageWidget::SWDisplayImageWidget(QWidget* oParent) : m_oSize(size())
{
	this->setParent(oParent);
}

SWDisplayImageWidget::~SWDisplayImageWidget()
{}

void SWDisplayImageWidget::paintEvent(QPaintEvent *)
{
	// update  image
	QPixmap l_oQPixRgb;
	
	QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
	l_oQPixRgb.convertFromImage(m_oQImage);

    if(l_oQPixRgb.isNull())
    {
        return;
    }

    QPixmap scaledPix = l_oQPixRgb.scaled(m_oSize,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation
                                    );
    // draw image
    painter.drawPixmap(QPoint(), scaledPix);
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
