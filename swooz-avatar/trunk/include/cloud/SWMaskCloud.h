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
 * \file SWMaskCloud.h
 * \brief defines SWMaskCloud
 * \author Florian Lance 
 * \date 06/03/13
 */

#ifndef _SWMASKCLOUD_
#define _SWMASKCLOUD_

#include "cloud/SWCloud.h"

namespace swCloud
{
	/**
	 * \class SWMaskCloud
	 * \brief Defines a SWCloud associated with an integer mask of the same size.
	 * \author Florian Lance
	 * \date 06/03/13
	 */
	class SWMaskCloud : public SWCloud
	{
		public :	
	
			// ############################################# CONSTRUCTORS / DESTRUCTORS
		
            /**
             * \brief Default constructor of SWMaskCloud
             */
            SWMaskCloud();

            /**
             * \brief Constructor of SWMaskCloud, mask array initialized with -1
             * \param [in] oCloud   : input cloud to copy
             */
            SWMaskCloud(const SWCloud &oCloud);

            /**
             * \brief Constructor of SWMaskCloud
             * \param [in] oCloud   : input cloud to copy
             * \param [in] vI32Mask : mask array of the cloud (must have the same size than the cloud number of points)
             */
            SWMaskCloud(const SWCloud &oCloud, const std::vector<int> &vI32Mask);

            /**
             * \brief destructor of SWMaskCloud
             */
            virtual ~SWMaskCloud(void);


			// ############################################# METHODS

            /**
             * \brief Set the cloud mask
             * \param [in] vI32Mask : cloud mask array
             */
            void setMask(const std::vector<int> &vI32Mask);

            /**
             * \brief Get the mask value corresponding to the input id point.
             * \return the integer mask value.
             */
            int mask(cuint ui32IdPoint) const;

            /**
             * \brief TEMP update colors
             */
            void updateMaskColors();

		
		private :

            std::vector<int> m_vI32Mask;
	};	
}
			
#endif
