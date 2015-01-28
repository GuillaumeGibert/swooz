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
 * \file SWAlignClouds.h
 * \brief defines SWAlignClouds class
 * \author Florian Lance
 * \date 07/12/12
 */

#ifndef _SWALIGNCLOUD_
#define _SWALIGNCLOUD_


#include "commonTypes.h"
#include "emicp/3dregistration.h"
#include "cloud/SWCloud.h"
#include <list>

namespace swCloud
{
    /**
     * \class SWAlignClouds
     * \brief Compute alignment between two SWCloud and retrieve the rigid motion.
     * \author Florian Lance
     * \date 07/12/12
     */
    class SWAlignClouds
    {
        public:

            // ############################################# CONSTRUCTORS / DESTRUCTORS

            /**
             * \brief Default constructor of SWAlignClouds
             * \param [in] bVerbose : verbose info display             
             */
            SWAlignClouds(cbool bVerbose = false);

            /**
             * \brief destructor of SWAlignClouds
             */
            ~SWAlignClouds();


            // ############################################# METHODS

            /**
             * \brief set the template cloud
             * \param [in] oTemplateCloud : template cloud
             */
            void setTemplateCloud(const SWCloud &oTemplateCloud);

            /**
             * \brief set the target cloud
             * \param [in] oTargetCloud : target cloud
             */
            void setTargetCloud(const SWCloud &oTargetCloud);

            /**
             * \brief set the clouds for the alignment.
             * \param [in] oTargetCloud     : target cloud
             * \param [in] oTemplateCloud   : template cloud
             */
            void setClouds(const SWCloud &oTargetCloud, const SWCloud &oTemplateCloud);

            /**
             * \brief apply a downscale on the clouds to reduce their size and speed up the computing
             * \param [in] fReductionCloud1 : factor to be applyied on the size cloud [0.01-1.0]
             * \param [in] fReductionCloud2 : factor to be applyied on the size cloud [0.01-1.0]
             */
            void setCloudDownscale(cfloat fReductionCloud1, cfloat fReductionCloud2);

            /**
             * @brief Set emicp alignment parameters (do no use if you don't know how emicp works, native parameters are good for a large panel of usages)
             * @param [in] fP2      : emicp P2
             * @param [in] fINF     : emicp INF
             * @param [in] fFactor  : emicp Factor
             * @param [in] fD02     : emicp D02
             */
            void setEmicpParams(cfloat fP2, cfloat fINF, cfloat fFactor, cfloat fD02);

            /**
             * \brief Parameters used for smooth the rigid motion
             * \param [in] ui32K    : number of previous computed rigidMotion used for temporal filtering on the current result (if == 0, no smoothing will occur)
             * \param [in] fSmoothTransConst : emipiric value used to compute the translation smoothing for the rigid motion
             * \param [in] fSmoothRotConst   : emipiric value used to compute the rotation smoothing for the rigid motion
             */
            void setSmoothingParams(cuint ui32K = 5, cfloat fSmoothTransConst = 50.f, cfloat fSmoothRotConst = 50.f);

            /**
             * \brief launch the alignement of the template cloud and the target cloud
             */
            void alignClouds();

            /**
             * \brief apply the last computed rigid motion transformation on the input cloud
             */
            void transformedCloud(SWCloud &oCloudToTransform);

            /**
             * \brief get the last computed rigid motion
             * \return SWRigidMotion
             */
            SWRigidMotion rigidMotion();

            /**
             * @brief get the last computed rigid motion
             * @param [in,out] fRotationMatrix    : rotation matrix pointer    [must be allocated [9*sizeof(float)], you have the ownership]
             * @param [in,out] fTranslationMatrix : translation matrix pointer [must be allocated [3*sizeof(float)], you have the ownership]
             */
            void rigidMotion(float *fRotationMatrix, float *fTranslationMatrix);


        private :


            /**
             * \brief set the rotation matrix to identity and the translation matrix to 0
             */
            void initRT();

            /**
             * \brief Launch emicp computing
             * \return true if no error during emicp computing
             */
            bool computeEmicp();

        private :

            // parameters
            bool m_bVerbose;                                /**< display infos ? */
            uint m_ui32K;                                   /**< K window, max number of the rigid motions to keep for computing the smoothing */
            float m_fReductionCloud1;                       /**< cloud 1 reduction factor */
            float m_fReductionCloud2;                       /**< cloud 2 reduction factor */
            float m_fHRot;                                  /**< emipiric value used to compute the rotation smoothing for the rigid motion */
            float m_fHTrans;                                /**< emipiric value used to compute the translation smoothing for the rigid motion */
            registration::registrationParameters m_SParam;  /**< emicp parameters */

            // rigid motion
            float m_fRotationMatrix[9];                     /**< rotation matrix  */
            float m_fTranslationMatrix[3];                  /**< translation matrix */
            SWRigidMotion m_oRigidMotion;                   /**< last computed rigid motion */
            SWRigidMotion m_oSmoothedRigidMotion;           /**< last computed smoothed rigid motion */
            std::list<SWRigidMotion> m_lRigidMotion;        /**< list of the K last computed rigid motions */

            // clouds
            SWCloud *m_oTemplate;                           /**< template cloud */
            SWCloud *m_oTarget;                             /**< target cloud */
    };
}

#endif
