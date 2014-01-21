
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
