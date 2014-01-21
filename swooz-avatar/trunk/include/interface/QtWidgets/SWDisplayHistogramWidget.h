
/**
 * \file SWDisplayHistogramWidget.h
 * \brief Defines SWDisplayHistogramWidget
 * \author Florian Lance
 * \date 28/05/13
 */

#ifndef _SWDISPLAYHISTOGRAMWIDGET_
#define _SWDISPLAYHISTOGRAMWIDGET_

#include <QPainter>
#include <QWidget>
#include <QVector>

#include "commonTypes.h"

/**
 * \class SWDisplayHistogramWidget
 * \brief A qt widget
 * \author Florian Lance
 * \date 28/05/13
 */
class SWDisplayHistogramWidget : public QWidget
{
	Q_OBJECT
	
	public:
		
		/**
		* \brief default constructor of SWDisplayHistogramWidget
		* \param [in] oParent 	    : parent qt widget 
		* \param [in] aSCurvesLabel : array of titles of the curves to draw
		* \param [in] oSize 	    : size of the histogram 
		* \param [in] fScale 	    : scale value to apply  
		* \param [in] ui32Xtic 	    : x tic of the histogram  (like xtics for gnuplot)
		* \param [in] ui32YTic 	    : y tic of the histogram  (like ytics for gnuplot) 
		* \param [in] i32TaitSize   : number of pixel between two inputs to draw
		*/		
		SWDisplayHistogramWidget(QWidget* oParent = 0, const std::vector<std::string> &aSCurvesLabel = std::vector<std::string>(), 
					 QSize &oSize = QSize(300,200), cfloat fScale = 1.f, cuint ui32Xtic = 5, cuint ui32YTic = 5, cint i32TaitSize = 1);

		/**
		* \brief destructor of SWDisplayImageWidget
		*/	    
        ~SWDisplayHistogramWidget();
	
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
	
		bool m_bDrawHisto;					/**< draw the histogram ? */
		bool m_bDrawLines;					/**< draw lines ? or points */
	
        uint m_ui32NumberOfCurves;          /**< number of curves */
		uint m_ui32XTic;					/**< x tic of the histogram  (like xtics for gnuplot) */
		uint m_ui32YTic;					/**< y tic of the histogram  (like ytics for gnuplot)  */
        uint m_ui32ScrollingOffsetXTic;     /**< ... */
        uint m_ui32ScrollingOffsetXLegend;  /**< ... */
	
        int  m_i32TraitSize;                /**< number of pixel between two inputs to draw */
	
        float m_fScaleValue;                /**< scale to apply on the values */

		QSize m_oSize;						/**< size of the widget */
		
        QVector<QString> m_aSCurvesName;                    /**< array of the curves titles */
		std::vector<std::list<float> > m_aLCurvesValues; 	/**< array of the list of the current values of the curves to display */
        std::vector<QColor> m_aColors;                      /**< array of the curves colors */
};


#endif
