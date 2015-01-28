

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
 * \file SWAnimation.h
 * \brief defines SWAnimation class
 * \author Florian Lance
 * \date 07/08/14
 */


#ifndef SWANIMATION_H
#define SWANIMATION_H

#include <QtGui>

#include <mesh/SWMesh.h>

namespace swAnimation
{

    /**
     * @brief The SWMod class
     */
    class SWMod
    {
        public :

            bool loadModFile(const QString &pathMod);

            int nbTransformations() const;

            swCloud::SWCloud cloud;

            std::vector<std::vector<float> > m_vtx;
            std::vector<std::vector<float> > m_vty;
            std::vector<std::vector<float> > m_vtz;

    };

    /**
     * @brief The SWSeq class
     */
    class SWSeq
    {
        public :

            bool loadSeqFile(const QString &pathSeq);
            std::vector<std::vector<float> > m_transFactors;
            std::vector<std::vector<float> > m_rigidMotion;
    };

    /**
     * @brief The SWMsh class
     */
    class SWMsh
    {
        public :

            bool loadMshFile(const QString &pathMsh);
            std::vector<std::vector<uint> > m_idFaces;
    };


    /**
     * @brief The SWAnimation class
     */
    class SWAnimation
    {
        public :

            SWAnimation();

            ~SWAnimation();

            void setCloudCorr(QString pathFile);

            void setRotTransIndex(cint index);

            void setSeq(const SWSeq &seq);

            void setMod(const SWMod &mod);

            void setMsh(const SWMsh &msh);

            void setTransformationToApply(const swCloud::SWRigidMotion transfoToApply = swCloud::SWRigidMotion(0.f,180.f,0.f),cfloat scaleToApply = 0.025f);


            void init(const SWMod &mod, const SWSeq &seq, const SWMsh &msh, const swCloud::SWRigidMotion transfoToApply = swCloud::SWRigidMotion(0.f,180.f,0.f),
                      cfloat scaleToApply = 0.025f);

            void retrieveTransformedCloud(cuint transformationId, swCloud::SWCloud &cloud, cbool applyTransfo = false);


            void retrieveTransformedMesh(cuint transformationId, swMesh::SWMesh &mesh, cbool applyTransfo= false);


            void constructCorrId(cbool applyTransfo = false);


            void transformMeshWithCorrId(cuint transformationId, swMesh::SWMesh &mesh);

            /**
             * @brief retrieveTransfosToApply
             * @param numLine
             * @param transfoX
             * @param transfoY
             * @param transfoZ
             * @param rigidMotion
             * @return
             */
            bool retrieveTransfosToApply(int numLine ,QVector<float> &transfoX,QVector<float> &transfoY,QVector<float> &transfoZ, QVector<float> &rigidMotion);

            bool m_seqFileLoaded;
            bool m_modFileLoaded;
            bool m_mshFileLoaded;
            bool m_cloudCorrLoaded;
            bool m_idCorrBuilt;

            swCloud::SWCloud *m_pCloudCorr;

            int m_indexRotTrans; /**< index of the center of rotation to apply */

        private :

            SWSeq m_animationSeq;
            SWMod m_animationMod;
            SWMsh m_animationMsh;



            swCloud::SWRigidMotion m_transfoToApply;
            float m_scaleToApply;

            std::vector<int> m_idCorr;




    };
}



#endif // SWMOD_H




