
#include "animation/SWAnimation.h"


bool swAnimation::SWMod::loadModFile(const QString &pathMod, const swCloud::SWRigidMotion transfoToApply,cfloat scaleToApply)
{
    qDebug() << "start loadModFile";
    QFile file(pathMod);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    qDebug() << "mod file opened";

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

    //
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

//            std::cout << l_valueV << " ";
            std::vector<float> l_vt;

            for(int ii = 0; ii < l_nbValues; ++ii)
            {
                float l_valueT;
                in >> l_valueT;
                l_vt.push_back(l_valueT);
//                std::cout << l_valueT << " ";
            }

//            std::cout << std::endl;

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

        l_line = l_line %4;

        line = in.readLine();

        if(l_line == 0)
        {
            ++l_totalLine;
//            qDebug() << line;
        }
    }

    qDebug() << "l_nbValues : " << l_nbValues;
    qDebug() << "l_vx : " << l_vx.size();
    qDebug() << "l_vtx : " << m_vtx.size();
    qDebug() << "l_totalLine : " << l_totalLine;


    cloud.set(l_vx,l_vy,l_vz);

    cloud *= scaleToApply;
    cloud.transform(transfoToApply.m_aFRotation, transfoToApply.m_aFTranslation);

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

bool swAnimation::SWSeq::loadSeqFile(const QString &pathSeq)
{
    qDebug() << "start loadSeqFile";
    QFile file(pathSeq);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    qDebug() << "seq file opened";

    QTextStream in(&file);
    int l_nbTransfo;
    in >> l_nbTransfo;


    QString line;
    line = in.readLine();


//    while (!in.atEnd())
//    {
//        QString sep;
//        in >> sep;

//        std::cout << sep.toStdString() << " ";

//        for(int ii = 0; ii < l_nbTransfo; ++ii)
//        {
////            .toStdString()
////            std::cout <<
//        }
//    }

    // count values

//        line = in.readLine();
//        bool l_empty = false;
//        int l_nbValues = 1;
//        for(int ii = 0; ii < line.size(); ++ii)
//        {
//            if(l_empty)
//            {
//                if(line[ii] != ' ')
//                {
//                    l_empty = false;
//                    ++l_nbValues;
//                }
//            }
//            else if(line[ii] == ' ')
//            {
//                l_empty = true;
//            }
//        }
//        in.seek(l_pos); // return to firt line

//        qDebug() << "l_nbTransfo seq : " << l_nbTransfo;


    return true;
}







void swAnimation::SWAnimation::init(const swAnimation::SWMod &mod, const swAnimation::SWSeq &seq)
{
    m_animationMod = mod;
    m_animationSeq = seq;
}


