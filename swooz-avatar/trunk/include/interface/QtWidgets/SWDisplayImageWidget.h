
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
        SWDisplayImageWidget(QWidget* oParent = 0,  const bool bScaleImage = true, const bool bActiveSelectPixelMode = false);

		/**
		* \brief destructor of SWDisplayImageWidget
		*/	    
		~SWDisplayImageWidget();


        void resizeEvent ( QResizeEvent * event );

	public slots:

		/**
		* \brief Set the new image and update the display.
		* \param [in] oQImage : qimage to display in the widget
		*/		
		void refreshDisplay(const QImage &oQImage);

        /**
         * @brief resetSelectedPoints
         */
        void resetSelectedPoints();


	protected:

		/**
		* \brief paint event
		*/		    
		void paintEvent(QPaintEvent *);

        /**
         * @brief mousePressEvent
         * @param event
         */
        void mousePressEvent ( QMouseEvent * event );

        /**
         * @brief mouseReleaseEvent
         * @param event
         */
        void mouseReleaseEvent ( QMouseEvent * event );

        /**
         * @brief mouseMoveEvent
         * @param event
         */
        void mouseMoveEvent(QMouseEvent * event);

    signals :

        void clickPoint(QPoint, QSize, bool);


	private:

		QImage m_oQImage;	/**< rgb image to display */
        QImage m_oScaledImage;

        QSize m_oSize;  /**< ... */

        bool m_bMouseClicked;
        bool m_bMouseLeftClick;
        bool m_bMouseRightClick;

        bool m_bScaleImage;

        bool m_bActiveSelectPixelMode;

    public :
        QVector<QPoint> m_vClickedPoints;
        QVector<QSize> m_vCurrentSize;
};


#endif 
