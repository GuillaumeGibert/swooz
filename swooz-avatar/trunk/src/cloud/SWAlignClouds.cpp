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
 * \file SWAlignClouds.cpp
 * \author LANCE Florian
 * \date 07/12/12
 * \brief defines SWAlignClouds
 */

#include "cloud/SWAlignClouds.h"


#include <iostream>

using namespace std;
using namespace registration;
using namespace swCloud;


// ############################################# CONSTRUCTORS / DESTRUCTORS

SWAlignClouds::SWAlignClouds(cbool bVerbose) :  m_bVerbose(bVerbose), m_fReductionCloud1(1.f), m_fReductionCloud2(1.f), m_ui32K(0), m_fHTrans(25), m_fHRot(25),
    m_oTemplate(NULL), m_oTarget(NULL)
{
    // set default emicp parameters
        m_SParam.sigma_p2     = 0.01f;
        m_SParam.sigma_inf    = 0.00001f;
        m_SParam.sigma_factor = 0.85f;
        m_SParam.d_02         = 0.01f;
}

SWAlignClouds::~SWAlignClouds()
{}

// ############################################# METHODS - SWAlignClouds


void SWAlignClouds::setTemplateCloud(const SWCloud &oTemplateCloud)
{
    deleteAndNullify(m_oTemplate);
    m_oTemplate = new SWCloud();
    m_oTemplate->copy(oTemplateCloud);
}

void SWAlignClouds::setTargetCloud(const SWCloud &oTargetCloud)
{
    deleteAndNullify(m_oTarget);
    m_oTarget = new SWCloud();
    m_oTarget->copy(oTargetCloud);
}

void SWAlignClouds::setClouds(const SWCloud &oTargetCloud, const SWCloud &oTemplateCloud)
{
    setTargetCloud(oTargetCloud);
    setTemplateCloud(oTemplateCloud);
}

void SWAlignClouds::setCloudDownscale(cfloat fReductionCloud1, cfloat fReductionCloud2)
{
    m_fReductionCloud1 = (fReductionCloud1 > 1.f) ? 1.f : ((fReductionCloud1 < 0.01f) ? 0.01f : fReductionCloud1);
    m_fReductionCloud2 = (fReductionCloud2 > 1.f) ? 1.f : ((fReductionCloud2 < 0.01f) ? 0.01f : fReductionCloud2);
}

void SWAlignClouds::setEmicpParams(cfloat fP2, cfloat fINF, cfloat fFactor, cfloat fD02)
{
    m_SParam.sigma_p2     = fP2;
    m_SParam.sigma_inf    = fINF;
    m_SParam.sigma_factor = fFactor;
    m_SParam.d_02         = fD02;
}

void SWAlignClouds::setSmoothingParams(cuint ui32K, cfloat fSmoothTransConst, cfloat fSmoothRotConst)
{
    m_ui32K   = ui32K;
    m_fHTrans = fSmoothTransConst;
    m_fHRot   = fSmoothRotConst;
}

void SWAlignClouds::alignClouds()
{
    // launch emicp
        bool l_bEmicpNoError = computeEmicp();

    // retrieve the computed rigid motion
        if(l_bEmicpNoError)
        {
            float *l_fRotationMatrix    = new float[9];
            float *l_fTranslationMatrix = new float[3];
            rigidMotion(l_fRotationMatrix, l_fTranslationMatrix);

            m_oRigidMotion = SWRigidMotion(l_fRotationMatrix, l_fTranslationMatrix);

            delete[] l_fRotationMatrix;
            delete[] l_fTranslationMatrix;

            if(m_ui32K > 0)
            {
                // compute the smoothed rigid motion
                //	 stock emicp result
                m_lRigidMotion.push_front(SWRigidMotion(m_oRigidMotion));

                // 	pop first result in the list if the size is over the k window
                while(m_ui32K < m_lRigidMotion.size())
                {
                    m_lRigidMotion.pop_back();
                }

                float l_fMaxDist = 0;

                // 	compute the max distance among all the  translation vectors
                for (std::list<SWRigidMotion>::iterator it= m_lRigidMotion.begin(); it != m_lRigidMotion.end(); ++it)
                {
                    // 	get vector i
                    float l_fX = (*it).m_aFTranslation[0];
                    float l_fY = (*it).m_aFTranslation[1];
                    float l_fZ = (*it).m_aFTranslation[2];

                    // 	compute norm for vector i
                    float l_fCurrDist = sqrt(l_fX*l_fX + l_fY*l_fY + l_fZ*l_fZ);

                    // 	update the max distance
                    if(l_fCurrDist > l_fMaxDist)
                    {
                        l_fMaxDist = l_fCurrDist;
                    }
                }

                // 	init sums
                float l_fSumTransWi = 0, l_fSumRotWi = 0;

                SWRigidMotion l_oSumWiTij;

                int ii = 0;
                for(std::list<SWRigidMotion>::iterator it = m_lRigidMotion.begin(); it != m_lRigidMotion.end(); ++it)
                {
                    float l_fTransWi = exp((float)(-ii) * m_fHTrans * l_fMaxDist);
                    float l_fRotWi   = exp((float)(-ii) * m_fHRot   * l_fMaxDist);

                    for(uint jj = 0; jj < 3; ++jj)
                    {
                        l_oSumWiTij.m_aFTranslation[jj] += (*it).m_aFTranslation[jj] * l_fTransWi;
                        l_oSumWiTij.m_aFRotAngles[jj]   += (*it).m_aFRotAngles[jj]   * l_fRotWi;
                    }

                    l_fSumTransWi += l_fTransWi;
                    l_fSumRotWi   += l_fRotWi;
                    ++ii;
                }
                std::cout << std::endl;

                for(uint jj = 0; jj < 3; ++jj)
                {
                    l_oSumWiTij.m_aFTranslation[jj] /= l_fSumTransWi;
                    l_oSumWiTij.m_aFRotAngles[jj]   /= l_fSumRotWi;
                }

                // TEST
//                    for(std::list<SWRigidMotion>::iterator it = m_lRigidMotion.begin(); it != m_lRigidMotion.end(); ++it)
//                    {
//                        for(int jj = 0; jj < 3; ++jj)
//                        {
//                            l_oSumWiTij.m_aFTranslation[jj] += (*it).m_aFTranslation[jj];
//                            l_oSumWiTij.m_aFRotAngles[jj]   += (*it).m_aFRotAngles[jj];
//                        }
//                    }

//                    for(uint jj = 0; jj < 3; ++jj)
//                    {
//                        l_oSumWiTij.m_aFTranslation[jj] /= m_lRigidMotion.size();
//                        l_oSumWiTij.m_aFRotAngles[jj]   /= m_lRigidMotion.size();
//                    }



                // update matrix rotation from new angles
                l_oSumWiTij.computeRotationMatrixWithRotationAngles();

                // set the smoothed rigid motion
                m_oSmoothedRigidMotion = SWRigidMotion(l_oSumWiTij);

                // display result
                if(m_bVerbose)
                {
                    std::cout << "K  : "; rigidMotion().display();
                }
            }
        }
        else
        {
            std::cerr << "K : error during emicp computing, keep last good rigid motion result. " << std::endl;
        }
}

SWRigidMotion SWAlignClouds::rigidMotion()
{
    if(m_ui32K > 0)
    {
        return m_oSmoothedRigidMotion;
    }

    return m_oRigidMotion;
}

void SWAlignClouds::rigidMotion(float *fRotationMatrix, float *fTranslationMatrix)
{
    for(uint ii = 0; ii < 9; ++ii)
    {
        fRotationMatrix[ii] = m_fRotationMatrix[ii];

        if(ii < 3)
        {
            fTranslationMatrix[ii] = m_fTranslationMatrix[ii];
        }
    }
}

void SWAlignClouds::transformedCloud(SWCloud &oCloudToTransform)
{
    // transform the input cloud
    oCloudToTransform.transform(rigidMotion().m_aFRotation ,rigidMotion().m_aFTranslation);
}

void SWAlignClouds::initRT()
{
    // set to Identity matrix
    m_fRotationMatrix[0] = 1.0f; // r11
    m_fRotationMatrix[1] = 0.0f; // r12
    m_fRotationMatrix[2] = 0.0f; // r13
    m_fRotationMatrix[3] = 0.0f; // r21
    m_fRotationMatrix[4] = 1.0f; // r22
    m_fRotationMatrix[5] = 0.0f; // r23
    m_fRotationMatrix[6] = 0.0f; // r31
    m_fRotationMatrix[7] = 0.0f; // r32
    m_fRotationMatrix[8] = 1.0f; // r33

    m_fTranslationMatrix[0] = 0.0f;
    m_fTranslationMatrix[1] = 0.0f;
    m_fTranslationMatrix[2] = 0.0f;
}

bool SWAlignClouds::computeEmicp()
{
    if(m_fReductionCloud1 < 1.f)
    {
        m_oTarget->reduce(m_fReductionCloud1);
    }

    if(m_fReductionCloud2 < 1.f)
    {
        m_oTemplate->reduce(m_fReductionCloud2);
    }

    // reinitialize rigid motion for avoiding persistent bad alignment
    initRT();

    if(!emicp(m_oTarget->size(), m_oTemplate->size(), m_oTarget->coord(0), m_oTemplate->coord(0),
          m_fRotationMatrix, m_fTranslationMatrix, m_SParam))
    {
        initRT();
        return false;
    }

    return true;
}

