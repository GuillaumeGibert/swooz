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

            void init(const SWMod &mod, const SWSeq &seq, const SWMsh &msh, const swCloud::SWRigidMotion transfoToApply = swCloud::SWRigidMotion(0.f,180.f,0.f),
                      cfloat scaleToApply = 0.025f);

            void retrieveTransformedCloud(cuint transformationId, swCloud::SWCloud &cloud, cbool applyTransfo = false);


            void retrieveTransformedMesh(cuint transformationId, swMesh::SWMesh &mesh, cbool applyTransfo= false);


            void constructCorrId(swMesh::SWMesh &mesh, cbool applyTransfo = false);


            void transformMeshWithCorrId(cuint transformationId, swMesh::SWMesh &mesh);

        private :

            SWSeq m_animationSeq;
            SWMod m_animationMod;
            SWMsh m_animationMsh;

            swCloud::SWRigidMotion m_transfoToApply;
            float m_scaleToApply;

            swMesh::SWMesh m_originalMesh;

            std::vector<int> m_idCorr;
    };
}



#endif // SWMOD_H




