

#include "SWDisplayImageWidget.h"

#include "moc_SWDisplayImageWidget.cpp"


#include <iostream>

using namespace std;

SWDisplayImageWidget::SWDisplayImageWidget(QWidget* oParent)
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
	l_oQPixRgb.convertFromImage(m_oQImage);
	
	// draw image
	painter.drawPixmap(QPoint(0,0),l_oQPixRgb);
}

void SWDisplayImageWidget::refreshDisplay(const QImage &oQImage)
{
	m_oQImage = oQImage.copy();
	update();
}



