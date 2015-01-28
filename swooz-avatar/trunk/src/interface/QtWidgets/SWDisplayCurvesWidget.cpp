/*******************************************************************************
**                                                                            **
**  SWoOz is a software platform written in C++ used for behavioral           **
**  experiments based on interactions between people and robots               **
**  or 3D avatars.                                                            **
**                                                                            **
**  This program is free software: you can redistribute it and/or modify      **
**  it under the terms of the GNU Lesser General Public License as published  **
**  by the Free Software Foundation, either version 3 of the License, or      **
**  (at your option) any later version.                                       **
**                                                                            **
**  This program is distributed in the hope that it will be useful,           **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of            **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             **
**  GNU Lesser General Public License for more details.                       **
**                                                                            **
**  You should have received a copy of the GNU Lesser General Public License  **
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.           **
**                                                                            **
** *****************************************************************************
**          Authors: Guillaume Gibert, Florian Lance                          **
**  Website/Contact: http://swooz.free.fr/                                    **
**       Repository: https://github.com/GuillaumeGibert/swooz                 **
********************************************************************************/

#include "SWDisplayCurvesWidget.h"
#include "SWExceptions.h"

#include "moc_SWDisplayCurvesWidget.cpp"

#include <iostream>
#include <sstream>

using namespace std;

SWDisplayCurvesWidget::SWDisplayCurvesWidget(QWidget* oParent, const std::vector<std::string> &aSCurvesLabel, QSize &oSize, cfloat fScale, cuint ui32Xtic, cuint ui32YTic, cint i32TaitSize) :
                           m_ui32NumberOfCurves(static_cast<uint>(aSCurvesLabel.size())), m_oSize(oSize), m_fScaleValue(fScale), m_ui32XTic(ui32Xtic), m_ui32YTic(ui32YTic),
						   m_i32TraitSize(i32TaitSize)
{
    // set parameters
	this->setParent(oParent);
	this->resize(m_oSize);		
	m_bDrawLines	= true;
    m_bDrawCurves    = false;
    m_ui32ScrollingOffsetXTic    = 0;
    m_ui32ScrollingOffsetXLegend = 0;

    // init curves colors
    QColor l_aCol[] = {Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::yellow, Qt::gray, Qt::magenta};
    m_aColors = std::vector<QColor>(l_aCol, l_aCol + sizeof(l_aCol) / sizeof(QColor) );

    // init list of curves values
    for(uint ii = 0; ii < aSCurvesLabel.size(); ++ii)
    {
        list<float> l_lCurveValue;
        m_aLCurvesValues.push_back(l_lCurveValue);
        m_aSCurvesName.push_back(aSCurvesLabel[ii].c_str());
    }

    if(m_ui32NumberOfCurves == 0 || m_oSize.width() == 0 || m_oSize.height() == 0 || m_ui32NumberOfCurves > 10)
    {
        cerr << "Bad input values (constructor SWDisplayCurvesWidget). " << std::endl;
        // throw
    }
}

SWDisplayCurvesWidget::~SWDisplayCurvesWidget()
{}

void SWDisplayCurvesWidget::setNewValues(std::vector<float> &aFCurvesValues)
{

    if(aFCurvesValues.size() != m_ui32NumberOfCurves)
    {
        cerr << "Error, the input values number doesn't match. " << std::endl;
        m_bDrawCurves = false;
        // throw... TODO
    }
    else
    {
        bool l_bIsSroll = false;
        m_bDrawCurves = true;

        for(uint ii = 0; ii < m_ui32NumberOfCurves; ++ii)
        {
            if(m_aLCurvesValues[ii].size() == m_oSize.width() / m_i32TraitSize)
            {
                m_aLCurvesValues[ii].pop_front();
                l_bIsSroll = true;
            }

            m_aLCurvesValues[ii].push_back(aFCurvesValues[ii]);
        }

        // apply scrolling offset
        if(l_bIsSroll)
        {
            ++m_ui32ScrollingOffsetXTic;

            if(m_ui32ScrollingOffsetXTic % m_ui32XTic == 0 )
            {
                ++m_ui32ScrollingOffsetXLegend;
            }

            m_ui32ScrollingOffsetXTic = m_ui32ScrollingOffsetXTic % m_ui32XTic;
        }
    }

    update();
}

void SWDisplayCurvesWidget::paintEvent(QPaintEvent *)
{
    QPainter l_oPainter(this);

    // scale
    l_oPainter.scale(width()*1.0/m_oSize.width(), height()*1.0/m_oSize.height());

    if(m_bDrawCurves)
    {
        // draw background
        l_oPainter.fillRect(0, 0, m_oSize.width(), m_oSize.height(), Qt::black);

        // draw tics
        l_oPainter.setPen(Qt::darkGray);

        for(uint ii = 0; ii <= m_oSize.width()/m_ui32XTic/m_i32TraitSize; ++ii)
        {
            l_oPainter.drawLine(QPoint((ii * m_ui32XTic - m_ui32ScrollingOffsetXTic)*m_i32TraitSize, 0), QPoint((ii * m_ui32XTic - m_ui32ScrollingOffsetXTic)*m_i32TraitSize, m_oSize.height()));
        }

        for(uint ii = 0; ii < m_oSize.height()/m_ui32YTic/2; ++ii)
        {
            l_oPainter.drawLine(QPoint(0, m_oSize.height()/2 + ii * m_ui32YTic), QPoint(m_oSize.width(), m_oSize.height()/2 + ii * m_ui32YTic));
            l_oPainter.drawLine(QPoint(0, m_oSize.height()/2 - ii * m_ui32YTic), QPoint(m_oSize.width(), m_oSize.height()/2 - ii * m_ui32YTic));
        }

        // draw 0 line
        l_oPainter.setPen(Qt::white);
        l_oPainter.drawLine(QPoint(0,  m_oSize.height()/2), QPoint(m_oSize.width(), m_oSize.height()/2));

        // draw legends
        for(uint ii = 0; ii < m_oSize.width()/m_ui32XTic/m_i32TraitSize + 1; ++ii)
        {
            ostringstream l_osXLegend;
            l_osXLegend << (ii+m_ui32ScrollingOffsetXLegend) * m_ui32XTic;
            l_oPainter.drawText(QPoint((ii * m_ui32XTic - m_ui32ScrollingOffsetXTic + m_ui32XTic/15)*m_i32TraitSize,  m_oSize.height()/2 + m_oSize.height()/20), l_osXLegend.str().c_str());
        }
        for(uint ii = 0; ii <=  m_oSize.height()/2/m_ui32YTic; ++ii)
        {
            ostringstream l_osYLegend;
            float l_fYLegend = ii * m_ui32YTic / m_fScaleValue;
            l_osYLegend << l_fYLegend;
            l_oPainter.drawText(QPoint(0, m_oSize.height()/2 - (ii * m_ui32YTic)), l_osYLegend.str().c_str());
        }
        for(uint ii = 1; ii <=  m_oSize.height()/2/m_ui32YTic; ++ii)
        {
            ostringstream l_osYLegend;
            float l_fYLegend = ii * m_ui32YTic / m_fScaleValue;
            l_osYLegend << -l_fYLegend;
            l_oPainter.drawText(QPoint(0, m_oSize.height()/2 + (ii * m_ui32YTic)), l_osYLegend.str().c_str());
        }


        uint l_ui32LegendTextWidth = m_oSize.width() / static_cast<uint>(m_aLCurvesValues.size());

        for(uint ii = 0; ii < m_aLCurvesValues.size(); ++ii)
        {
            l_oPainter.setPen(m_aColors[ii]);
            l_oPainter.drawText(QPoint( ii * l_ui32LegendTextWidth + m_oSize.width()/20,  m_oSize.height()/20), m_aSCurvesName[ii]);
        }

        // draw values
        std::vector<std::vector<QPoint> > l_aAP3fPointsToDraw;
        for(uint ii = 0; ii < m_aLCurvesValues.size(); ++ii)
        {
            std::vector<QPoint> l_aP3fCurvePoints;

            uint jj = 0;
            for(std::list<float>::iterator it = m_aLCurvesValues[ii].begin(); it != m_aLCurvesValues[ii].end(); ++it, ++jj)
            {
                float l_fVal = (*it) * m_fScaleValue + m_oSize.height()/2;

                if(l_fVal > m_oSize.height())
                {
                    l_fVal = m_oSize.height();
                }
                else if(l_fVal < 0)
                {
                    l_fVal = 0;
                }

                l_fVal = m_oSize.height() - l_fVal; // qt origin is top left

                l_aP3fCurvePoints.push_back(QPoint(jj*m_i32TraitSize, (int)l_fVal));
            }

            l_aAP3fPointsToDraw.push_back(l_aP3fCurvePoints);
        }

        for(uint ii = 0; ii < l_aAP3fPointsToDraw.size(); ++ii)
        {
            l_oPainter.setPen(m_aColors[ii]);

            if(m_bDrawLines)
            {
                for(uint jj = 0; jj < l_aAP3fPointsToDraw[ii].size()-1; ++jj)
                {
                    l_oPainter.drawLine(l_aAP3fPointsToDraw[ii].at(jj), l_aAP3fPointsToDraw[ii].at(jj+1));
                }
            }
            else
            {
                for(uint jj = 0; jj < l_aAP3fPointsToDraw[ii].size(); ++jj)
                {
                    l_oPainter.drawPoint(l_aAP3fPointsToDraw[ii].at(jj));
                }
            }
        }
    }
}


