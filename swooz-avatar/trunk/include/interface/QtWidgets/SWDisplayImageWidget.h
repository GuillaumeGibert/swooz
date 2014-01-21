
/**
 * \file SWDisplayImageWidget.h
 * \brief Defines SWDisplayImageWidget
 * \author Florian Lance
 * \date 22/01/13
 */


#ifndef _SWDISPLAYIMAGEWIDGET_
#define _SWDISPLAYIMAGEWIDGET_

#include <QPainter>
#include <QWidget>

/**
 * \class SWDisplayImageWidget
 * \brief Widget displaying a qimage
 * \author Florian Lance
 * \date 22/12/12
 */
class SWDisplayImageWidget : public QWidget
{
	Q_OBJECT

	public:

		/**
		* \brief default constructor of SWDisplayImageWidget
		* \param [in] oParent : ... 
		*/		
		SWDisplayImageWidget(QWidget* oParent = 0 );

		/**
		* \brief destructor of SWDisplayImageWidget
		*/	    
		~SWDisplayImageWidget();

	public slots:

		/**
		* \brief Set the new image and update the display.
		* \param [in] oQImage : qimage to display in the widget
		*/		
		void refreshDisplay(const QImage &oQImage);

	protected:

		/**
		* \brief paint event
		*/		    
		void paintEvent(QPaintEvent *);

	private:

		QImage m_oQImage;	/**< rgb image to display */
};


#endif 