
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

        l_nbValues -= 6; // remove 6 last values

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
    m_rigidMotion.clear();

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
        std::vector<float> l_rigidMotion;

        for(int ii = 0; ii < l_nbTransfo; ++ii)
        {
            float l_value;
            in >> l_value;

            if(ii < l_nbTransfo-6)
            {
                l_factors.push_back(l_value);
            }
            else
            {
                l_rigidMotion.push_back(l_value);
            }
        }

        m_transFactors.push_back(l_factors);
        m_rigidMotion.push_back(l_rigidMotion);
    }

    return true;
}




swAnimation::SWAnimation::SWAnimation() : m_pCloudCorr(NULL)
{
    m_seqFileLoaded = false;
    m_modFileLoaded = false;
    m_mshFileLoaded = false;
    m_cloudCorrLoaded = false;
    m_idCorrBuilt = false;
}

swAnimation::SWAnimation::~SWAnimation()
{
    deleteAndNullify(m_pCloudCorr);
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

    m_indexRotTrans = 0;
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

    qDebug() << " start ";

    swCloud::SWCloud cloud;
    cloud.copy(m_animationMod.cloud);
qDebug() << " --- ";
    for(uint ii = 0; ii < cloud.size(); ++ii)
    {
        cloud.coord(0)[ii] += 3* m_animationMod.m_vtx[ii][transformationId];
        cloud.coord(1)[ii] += 3* m_animationMod.m_vty[ii][transformationId];
        cloud.coord(2)[ii] += 3* m_animationMod.m_vtz[ii][transformationId];
    }
qDebug() << " ---";

    if(applyTransfo)
    {
        cloud *= 0.025f;
//        cloud.transform(m_transfoToApply.m_aFRotation, m_transfoToApply.m_aFTranslation);
    }
qDebug() << " --- ";

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
    qDebug() << l_vertices.size() << " --- " << m_animationMsh.m_idFaces.size() << " "<< l_texture.size();

    mesh.set(l_vertices, m_animationMsh.m_idFaces, l_texture);

    qDebug() << " end ";
}


void swAnimation::SWAnimation::constructCorrId(cbool applyTransfo)
{
    if(!m_cloudCorrLoaded || !m_modFileLoaded)
    {
        return;
    }

    swCloud::SWCloud l_originalCloud, l_testCloud;
    l_originalCloud.copy(m_animationMod.cloud);
    l_testCloud.copy(*m_pCloudCorr);

    if(applyTransfo)
    {
        l_originalCloud *= m_scaleToApply;
        l_originalCloud.transform(m_transfoToApply.m_aFRotation, m_transfoToApply.m_aFTranslation);
    }

    l_originalCloud.saveToObj("../data/clouds/", "originalBefore.obj");
    l_testCloud.saveToObj("../data/clouds/", "id_testBefore.obj");

    // align with nose at 0.0.0
        std::vector<float> l_offsetToApply(3,0.f);

        float l_zMax = -FLT_MAX;
        int l_idZMax = 0;
        for(int ii = 0; ii < l_originalCloud.size(); ++ii)
        {
            if(l_originalCloud.coord(2)[ii] > l_zMax)
            {
                l_zMax = l_originalCloud.coord(2)[ii];
                l_idZMax = ii;
            }
        }

        l_offsetToApply[0] = -l_originalCloud.coord(0)[l_idZMax];
        l_offsetToApply[1] = -l_originalCloud.coord(1)[l_idZMax];
        l_offsetToApply[2] = -l_originalCloud.coord(2)[l_idZMax];
        l_originalCloud += l_offsetToApply;

        l_zMax = -FLT_MAX;
        l_idZMax = 0;
        for(int ii = 0; ii < l_testCloud.size(); ++ii)
        {
            if(l_testCloud.coord(2)[ii] > l_zMax)
            {
                l_zMax = l_testCloud.coord(2)[ii];
                l_idZMax = ii;
            }
        }

        l_offsetToApply[0] = -l_testCloud.coord(0)[l_idZMax];
        l_offsetToApply[1] = -l_testCloud.coord(1)[l_idZMax];
        l_offsetToApply[2] = -l_testCloud.coord(2)[l_idZMax];
        l_testCloud += l_offsetToApply;

    // save debug
        l_originalCloud.saveToObj("../data/clouds/", "originalAfter.obj");
        l_testCloud.saveToObj("../data/clouds/", "id_testAfter.obj");

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

bool swAnimation::SWAnimation::retrieveTransfosToApply(int numLine ,QVector<float> &transfoX,QVector<float> &transfoY,QVector<float> &transfoZ, QVector<float> &rigidMotion)
{
    if(!m_seqFileLoaded || !m_modFileLoaded || !m_idCorrBuilt || !m_cloudCorrLoaded)
    {
        return false;
    }

    transfoX.clear();
    transfoY.clear();
    transfoZ.clear();
    rigidMotion.clear();

    if(numLine == m_animationSeq.m_transFactors.size())
    {
        return false;
    }

    for(uint ii = 0; ii < m_pCloudCorr->size(); ++ii)
    {
        transfoX.push_back(0.f);
        transfoY.push_back(0.f);
        transfoZ.push_back(0.f);

        for(int jj = 0; jj < m_animationMod.m_vtx[0].size(); ++jj)
        {
            transfoX.last() += (m_animationMod.m_vtx[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj])/40.f;
            transfoY.last() += (m_animationMod.m_vty[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj])/40.f;
            transfoZ.last() -= (m_animationMod.m_vtz[m_idCorr[ii]][jj]* m_animationSeq.m_transFactors[numLine][jj])/40.f;
        }
    }

    for(int ii = 0; ii < 6; ++ii)
    {
        rigidMotion.push_back(m_animationSeq.m_rigidMotion[numLine][ii]);
    }
//    qDebug() << " time -> " << ((float)(clock() - l_oProgramTime) / CLOCKS_PER_SEC);


    return true;
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

void swAnimation::SWAnimation::setCloudCorr(QString pathFile)
{
    deleteAndNullify(m_pCloudCorr);

    swMesh::SWMesh mesh(pathFile.toStdString());
    m_pCloudCorr = new swCloud::SWCloud();
    m_pCloudCorr->copy(*mesh.cloud());

    m_cloudCorrLoaded = true;
}



void swAnimation::SWAnimation::setRotTransIndex(cint index)
{
    m_indexRotTrans = index;
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


