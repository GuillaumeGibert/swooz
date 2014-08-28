#ifndef SWANIMATION_H
#define SWANIMATION_H

#include <QtGui>

#include <mesh/SWMesh.h>

namespace swAnimation
{

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

    class SWSeq
    {
        public :

            bool loadSeqFile(const QString &pathSeq);
            std::vector<std::vector<float> > m_transFactors;
            std::vector<std::vector<float> > m_rigidMotion;
    };


    class SWMsh
    {
        public :

            bool loadMshFile(const QString &pathMsh);
            std::vector<std::vector<uint> > m_idFaces;
    };


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




