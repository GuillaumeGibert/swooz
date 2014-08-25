#ifndef SWANIMATION_H
#define SWANIMATION_H

#include <QtGui>

#include <mesh/SWMesh.h>

namespace swAnimation
{

    class SWMod
    {
        public :

            bool loadModFile(const QString &pathMod, const swCloud::SWRigidMotion transfoToApply = swCloud::SWRigidMotion(0.f,180.f,0.f),
                         cfloat scaleToApply = 0.025f);

            int nbTransformations() const;


        private :


            swCloud::SWCloud cloud;

            std::vector<std::vector<float> > m_vtx;
            std::vector<std::vector<float> > m_vty;
            std::vector<std::vector<float> > m_vtz;

    };

    class SWSeq
    {
        public :

            bool loadSeqFile(const QString &pathSeq);


        private :


        std::vector<float> t;
    };


    class SWAnimation
    {
        public :

            void init(const SWMod &mod, const SWSeq &seq);


        private :

            SWSeq m_animationSeq;
            SWMod m_animationMod;
    };
}



#endif // SWMOD_H



//static void extractPointFromModFile(QString pathModFile, swCloud::SWCloud &cloud)
//{
//    qDebug() << "start extractPointFromModFile";
//    QFile file(pathModFile);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    qDebug() << "mod file opened";

//    QTextStream in(&file);
//    while (!in.atEnd())
//    {
//        QString line = in.readLine();

//        if(line.left(3) == "  1")
//        {
//            break;
//        }
//    }


//    int ii = 0;

//    std::vector<float> vx,vy,vz;
//    while (!in.atEnd())
//    {
//        double px,py,pz;

//        in >> px;
//        QString line = in.readLine();
//        in >> py;
//        line = in.readLine();
//        in >> pz;
//        line = in.readLine();
//        line = in.readLine();
////        qDebug() << ii << " " << px << " " << py << " " << pz;
//        ++ii;
//        vx.push_back(px);
//        vy.push_back(py);
//        vz.push_back(pz);
//    }

//    cloud.set(vx,vy,vz);
//}

//static void extractFacesFromMshFile(QString pathMshFile, const swCloud::SWCloud &cloud, swMesh::SWMesh &mesh)
//{

//    qDebug() << "start extractFacesFromMshFile";
//    QFile file(pathMshFile);
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    qDebug() << "msh file opened";

//    QTextStream in(&file);

//    int l_nbFaces;
//    in >> l_nbFaces;
//    QString line = in.readLine();
////    QString line


//    std::vector<std::vector<float> > l_vertices;
//    std::vector<std::vector<uint> > l_idFaces;
//    std::vector<std::vector<float> > l_texture(cloud.size(), std::vector<float>(2,0.f));




//    int ii = 0;
//    while (!in.atEnd() && ii < l_nbFaces)
//    {
//        uint id1,id2,id3;
//        std::vector<uint> l_idFace;

////        line = in.readLine();

//        in >> id1;
//        in >> id2;
//        in >> id3;

//        line = in.readLine();

////        qDebug() << ii << " " << id1 << " " << id2 << " " << id3;

//        l_idFace.push_back(id1+1);
//        l_idFace.push_back(id2+1);
//        l_idFace.push_back(id3+1);
//        l_idFaces.push_back(l_idFace);

//        ++ii;
//    }

//    for(int ii = 0; ii < cloud.size(); ++ii)
//    {
//        std::vector<float> l_pt;
//        l_pt.push_back(cloud.coord(0)[ii]);
//        l_pt.push_back(cloud.coord(1)[ii]);
//        l_pt.push_back(cloud.coord(2)[ii]);
//        l_vertices.push_back(l_pt);
//    }

//    mesh.set(l_vertices, l_idFaces, l_texture);
//}

