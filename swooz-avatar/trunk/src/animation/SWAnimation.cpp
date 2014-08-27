
#include "animation/SWAnimation.h"


bool swAnimation::SWMod::loadModFile(const QString &pathMod)
{
    QFile file(pathMod);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "Can not open mod file. " << std::endl;
        return false;
    }

    // skip to interesting data
            int l_pos;
            QTextStream in(&file);
            QString line;
            while (!in.atEnd())
            {
                l_pos = in.pos();

                line = in.readLine();

                if(line.left(3) == "  1")
                {

                    break;
                }


            }

        // count values

            line = in.readLine();
            bool l_empty = false;
            int l_nbValues = 1;
            for(int ii = 0; ii < line.size(); ++ii)
            {
                if(l_empty)
                {
                    if(line[ii] != ' ')
                    {
                        l_empty = false;
                        ++l_nbValues;
                    }
                }
                else if(line[ii] == ' ')
                {
                    l_empty = true;
                }
            }
            in.seek(l_pos); // return to firt line

        std::vector<float> l_vx,l_vy,l_vz;
        m_vtx.clear();
        m_vty.clear();
        m_vtz.clear();

        int l_totalLine = 1;
        int l_line = 0;

        while (!in.atEnd())
        {
            if(l_line != 0)
            {
                float l_valueV;
                in >> l_valueV;

                std::vector<float> l_vt;

                for(int ii = 0; ii < l_nbValues-1; ++ii)
                {
                    float l_valueT;
                    in >> l_valueT;
                    l_vt.push_back(l_valueT);
                }

                if(l_line == 1)
                {
                    l_vx.push_back(l_valueV);
                    m_vtx.push_back(l_vt);
                }
                else if(l_line == 2)
                {
                    l_vy.push_back(l_valueV);
                    m_vty.push_back(l_vt);
                }
                else
                {
                    l_vz.push_back(l_valueV);
                    m_vtz.push_back(l_vt);
                }
            }




            line = in.readLine();


            ++l_totalLine;
            ++l_line;
            l_line = l_line %4;
        }

        cloud.set(l_vx,l_vy,l_vz);

    return true;
}

int swAnimation::SWMod::nbTransformations() const
{
    if(m_vtx.size() > 0)
    {
        return m_vtx[0].size();
    }
    else
    {
        return -1;
    }
}


bool swAnimation::SWMsh::loadMshFile(const QString &pathMsh)
{

    m_idFaces.clear();

    QFile file(pathMsh);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "Can not open msh file. " << std::endl;
        return false;
    }

    QTextStream in(&file);

    int l_nbFaces;
    in >> l_nbFaces;
    QString line = in.readLine();


    int ii = 0;
    while (!in.atEnd() && ii < l_nbFaces)
    {
        uint id1,id2,id3;
        std::vector<uint> l_idFace;

        in >> id1;
        in >> id2;
        in >> id3;

        line = in.readLine();

        l_idFace.push_back(id1+1);
        l_idFace.push_back(id2+1);
        l_idFace.push_back(id3+1);
        m_idFaces.push_back(l_idFace);
        ++ii;
    }

    return true;
}

bool swAnimation::SWSeq::loadSeqFile(const QString &pathSeq)
{
    m_transFactors.clear();

    QFile file(pathSeq);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "Can not open seq file. " << std::endl;
        return false;
    }

    QTextStream in(&file);
    int l_nbTransfo;
    in >> l_nbTransfo;

    QString line;
    line = in.readLine();

    while (!in.atEnd())
    {
        QString l_separator;
        in >> l_separator;

        std::vector<float> l_factors;

        for(int ii = 0; ii < l_nbTransfo; ++ii)
        {
            float l_factor;
            in >> l_factor;
            l_factors.push_back(l_factor);
        }

        m_transFactors.push_back(l_factors);
    }

    return true;
}



void swAnimation::SWAnimation::init(const swAnimation::SWMod &mod, const swAnimation::SWSeq &seq,const swAnimation::SWMsh &msh,
                                    const swCloud::SWRigidMotion transfoToApply,cfloat scaleToApply)
{
    m_animationMod = mod;
    m_animationSeq = seq;
    m_animationMsh = msh;

    m_modFileLoaded = true;
    m_seqFileLoaded = true;
    m_mshFileLoaded = true;

    m_scaleToApply = scaleToApply;
    m_transfoToApply = transfoToApply;
}

void swAnimation::SWAnimation::retrieveTransformedCloud(cuint transformationId, swCloud::SWCloud &cloud, cbool applyTransfo)
{
    cloud.copy(m_animationMod.cloud);

    for(uint ii = 0; ii < cloud.size(); ++ii)
    {
        cloud.coord(0)[ii] += 3* m_animationMod.m_vtx[ii][transformationId];
        cloud.coord(1)[ii] += 3* m_animationMod.m_vty[ii][transformationId];
        cloud.coord(2)[ii] += 3* m_animationMod.m_vtz[ii][transformationId];
    }

    if(applyTransfo)
    {
        cloud *= m_scaleToApply;
        cloud.transform(m_transfoToApply.m_aFRotation, m_transfoToApply.m_aFTranslation);
    }
}

void swAnimation::SWAnimation::retrieveTransformedMesh(cuint transformationId, swMesh::SWMesh &mesh, cbool applyTransfo)
{
    mesh.clean();

    swCloud::SWCloud cloud;
    cloud.copy(m_animationMod.cloud);

    for(uint ii = 0; ii < cloud.size(); ++ii)
    {
        cloud.coord(0)[ii] += 3* m_animationMod.m_vtx[ii][transformationId];
        cloud.coord(1)[ii] += 3* m_animationMod.m_vty[ii][transformationId];
        cloud.coord(2)[ii] += 3* m_animationMod.m_vtz[ii][transformationId];
    }

    if(applyTransfo)
    {
        cloud *= m_scaleToApply;
        cloud.transform(m_transfoToApply.m_aFRotation, m_transfoToApply.m_aFTranslation);
    }

    std::vector<std::vector<float> > l_vertices;
    std::vector<std::vector<float> > l_texture(cloud.size(), std::vector<float>(2,0.f));
    for(uint ii = 0; ii < cloud.size(); ++ii)
    {
        std::vector<float> l_pt;
        l_pt.push_back(cloud.coord(0)[ii]);
        l_pt.push_back(cloud.coord(1)[ii]);
        l_pt.push_back(cloud.coord(2)[ii]);
        l_vertices.push_back(l_pt);
    }

    mesh.set(l_vertices, m_animationMsh.m_idFaces, l_texture);
}


void swAnimation::SWAnimation::constructCorrId(cbool applyTransfo)
{
    swCloud::SWCloud l_originalCloud, l_testCloud;
    l_originalCloud.copy(m_animationMod.cloud);
    qDebug() << "l_originalCloud " << l_originalCloud.size();
    l_testCloud.copy(*m_pCloudCorr);

    qDebug() << "l_originalCloud " << l_originalCloud.size() << " " << l_testCloud.size();

    if(applyTransfo)
    {
        l_originalCloud *= m_scaleToApply;
        l_originalCloud.transform(m_transfoToApply.m_aFRotation, m_transfoToApply.m_aFTranslation);
    }

    // align with nose at 0.0.0
        std::vector<float> l_offsetToApply(3,0.f);

        float l_zMin = FLT_MAX;
        int l_idZMin = 0;
        for(int ii = 0; ii < l_originalCloud.size(); ++ii)
        {
            if(l_originalCloud.coord(2)[ii] < l_zMin)
            {
                l_zMin = l_originalCloud.coord(2)[ii];
                l_idZMin = ii;
            }
        }

        l_offsetToApply[0] = -l_originalCloud.coord(0)[l_idZMin];
        l_offsetToApply[1] = -l_originalCloud.coord(1)[l_idZMin];
        l_offsetToApply[2] = -l_originalCloud.coord(2)[l_idZMin];
        l_originalCloud += l_offsetToApply;

        l_zMin = FLT_MAX;
        l_idZMin = 0;
        for(int ii = 0; ii < l_testCloud.size(); ++ii)
        {
            if(l_testCloud.coord(2)[ii] < l_zMin)
            {
                l_zMin = l_testCloud.coord(2)[ii];
                l_idZMin = ii;
            }
        }

        l_offsetToApply[0] = -l_testCloud.coord(0)[l_idZMin];
        l_offsetToApply[1] = -l_testCloud.coord(1)[l_idZMin];
        l_offsetToApply[2] = -l_testCloud.coord(2)[l_idZMin];
        l_testCloud += l_offsetToApply;

    // save debug
        l_originalCloud.saveToObj("../data/clouds/", "original.obj");
        l_testCloud.saveToObj("../data/clouds/", "id_test.obj");

    // retrieve id
        m_idCorr.clear();
        for(uint ii = 0; ii < l_testCloud.size(); ++ii)
        {
            std::vector<float> l_pt;
            l_testCloud.point(l_pt,ii);
            m_idCorr.push_back(l_originalCloud.idNearestPoint(l_pt));
        }

    m_idCorrBuilt = true;
}

void swAnimation::SWAnimation::retrieveTransfosToApply(int numLine ,QVector<float> &transfoX,QVector<float> &transfoY,QVector<float> &transfoZ)
{
    transfoX.clear();
    transfoY.clear();
    transfoZ.clear();

//    qDebug() << "mretrieveTransfosToApply " << m_animationSeq.m_transFactors.size() << " " << m_animationSeq.m_transFactors[0].size();

    qDebug() << "num line " << numLine;

    if(numLine > m_animationSeq.m_transFactors.size())
    {
        qDebug() << "end anim";
        return;
    }

    for(uint ii = 0; ii < m_pCloudCorr->size(); ++ii)
    {
        transfoX.push_back(0.f);
        transfoY.push_back(0.f);
        transfoZ.push_back(0.f);

        for(int jj = 0; jj < m_animationMod.m_vtx[0].size(); ++jj)
        {
            transfoX.last() += (3* m_animationMod.m_vtx[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj]);
            transfoY.last() += (3* m_animationMod.m_vty[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj]);
            transfoZ.last() += (3* m_animationMod.m_vtz[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj]);
        }
    }

//    qDebug()<< transfoX;

//    qDebug() << "end retrieveTransfosToApply ";
}

void swAnimation::SWAnimation::transformMeshWithCorrId(cuint transformationId, swMesh::SWMesh &mesh)
{
    for(int ii = 0; ii < mesh.pointsNumber(); ++ii)
    {
        mesh.cloud()->coord(0)[ii] *= (1.f/m_scaleToApply);
        mesh.cloud()->coord(1)[ii] *= (1.f/m_scaleToApply);
        mesh.cloud()->coord(2)[ii] *= (1.f/m_scaleToApply);
    }

    for(uint ii = 0; ii < mesh.pointsNumber(); ++ii)
    {
        mesh.cloud()->coord(0)[ii] += 3* m_animationMod.m_vtx[m_idCorr[ii]][transformationId];
        mesh.cloud()->coord(1)[ii] += 3* m_animationMod.m_vty[m_idCorr[ii]][transformationId];
        mesh.cloud()->coord(2)[ii] += 3* m_animationMod.m_vtz[m_idCorr[ii]][transformationId];
    }
}


swAnimation::SWAnimation::SWAnimation() : m_pCloudCorr(NULL)
{
    m_seqFileLoaded = false;
    m_modFileLoaded = false;
    m_mshFileLoaded = false;
    m_idCorrBuilt = false;
}

swAnimation::SWAnimation::~SWAnimation()
{
    deleteAndNullify(m_pCloudCorr);
}

void swAnimation::SWAnimation::setCloudCorr(swCloud::SWCloud *pCloudCorr)
{
    qDebug() <<"############################## setCloudCorr";
    m_pCloudCorr = pCloudCorr;
}

void swAnimation::SWAnimation::setSeq(const swAnimation::SWSeq &seq)
{
    m_animationSeq = seq;
    m_seqFileLoaded = true;
}

void swAnimation::SWAnimation::setMod(const swAnimation::SWMod &mod)
{
    m_animationMod = mod;
    m_modFileLoaded = true;
}

void swAnimation::SWAnimation::setMsh(const swAnimation::SWMsh &msh)
{
    m_animationMsh = msh;
    m_mshFileLoaded = true;
}

void swAnimation::SWAnimation::setTransformationToApply(const swCloud::SWRigidMotion transfoToApply,cfloat scaleToApply)
{
    m_scaleToApply = scaleToApply;
    m_transfoToApply = transfoToApply;
}


