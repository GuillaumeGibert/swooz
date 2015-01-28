
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

/**
 * \file SWDisplayCurvesWidget.h
 * \brief Defines SWDisplayCurvesWidget
 * \author Florian Lance
 * \date 28/05/13
 */

#ifndef _SWDISPLAYCURVESWIDGET_
#define _SWDISPLAYCURVESWIDGET_

#include <QPainter>
#include <QWidget>
#include <QVector>

#include "commonTypes.h"

/**
 * \class SWDisplayCurvesWidget
 * \brief A simple Qt widget for displaying curves with legends in real time.
 * \author Florian Lance
 * \date 28/05/13
 */
class SWDisplayCurvesWidget : public QWidget
{
	Q_OBJECT
	
	public:
		
		/**
        * \brief default constructor of SWDisplayCurvesWidget
		* \param [in] oParent 	    : parent qt widget 
		* \param [in] aSCurvesLabel : array of titles of the curves to draw
        * \param [in] oSize 	    : size of the widget
		* \param [in] fScale 	    : scale value to apply  
        * \param [in] ui32Xtic 	    : x tic of the curve (like xtics for gnuplot)
        * \param [in] ui32YTic 	    : y tic of the curve (like ytics for gnuplot)
		* \param [in] i32TaitSize   : number of pixel between two inputs to draw
		*/		
        SWDisplayCurvesWidget(QWidget* oParent = 0, const std::vector<std::string> &aSCurvesLabel = std::vector<std::string>(),
					 QSize &oSize = QSize(300,200), cfloat fScale = 1.f, cuint ui32Xtic = 5, cuint ui32YTic = 5, cint i32TaitSize = 1);

		/**
        * \brief destructor of SWDisplayCurvesWidget
		*/	    
        ~SWDisplayCurvesWidget();
	
		/**
		* \brief Set a new input set of values to draw, the array must have the same size than the title curve array used in the construtor.
		* \param [in] aFCurvesValues : array of input values, corresponding to the title curve array 
		*/		
		void setNewValues(std::vector<float> &aFCurvesValues);
	
	protected:
		
		/**
		* \brief paint event
		*/		    
		void paintEvent(QPaintEvent *);
	
	private:
	
        bool m_bDrawCurves;					/**< draw the curves ? */
		bool m_bDrawLines;					/**< draw lines ? or points */
	
        uint m_ui32NumberOfCurves;          /**< number of curves */
        uint m_ui32XTic;					/**< x tic of the curves  (like xtics for gnuplot) */
        uint m_ui32YTic;					/**< y tic of the curves  (like ytics for gnuplot)  */
        uint m_ui32ScrollingOffsetXTic;     /**< x tic scrolling offset */
        uint m_ui32ScrollingOffsetXLegend;  /**< x legend scrolling offset */
	
        int  m_i32TraitSize;                /**< number of pixel between two inputs to draw */
	
        float m_fScaleValue;                /**< scale to apply on the values */

		QSize m_oSize;						/**< size of the widget */
		
        QVector<QString> m_aSCurvesName;                    /**< array of the curves titles */
		std::vector<std::list<float> > m_aLCurvesValues; 	/**< array of the list of the current values of the curves to display */
        std::vector<QColor> m_aColors;                      /**< array of the curves colors */
};


#endif
