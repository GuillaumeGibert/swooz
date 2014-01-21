/********************************************************************************
** Form generated from reading UI file 'SWUI_WEmicpHeadTracking.ui'
**
** Created: Tue 21. Jan 17:26:30 2014
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef SWUI_WEMICPHEADTRACKING_H
#define SWUI_WEMICPHEADTRACKING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SWUI_WEmicpHeadTracking
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *vlMain;
    QGridLayout *glDisplay;
    QGroupBox *gbClouds;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *hlClouds;
    QGroupBox *gbVideo;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *hlVideo;
    QGroupBox *gbRotations;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *hlRotations;
    QGroupBox *gbTranslations;
    QHBoxLayout *horizontalLayout_9;
    QHBoxLayout *hlTranslations;
    QHBoxLayout *hlButtons;
    QGroupBox *gbParameters;
    QGridLayout *gridLayout_3;
    QGridLayout *glParameters;
    QCheckBox *cbSmoothRigidMotion;
    QLabel *laTemplateCloudReduction;
    QLabel *laTargetCloudReduction;
    QDoubleSpinBox *dsbScoreComputingReduction;
    QDoubleSpinBox *dsbTargetCloudReduction;
    QDoubleSpinBox *dsbP2;
    QLabel *laP2;
    QLabel *laEMICPParams;
    QDoubleSpinBox *dsbFactor;
    QDoubleSpinBox *dsbINF;
    QLabel *laFactor;
    QLabel *laInf;
    QLabel *laD02;
    QDoubleSpinBox *dsbD02;
    QDoubleSpinBox *dsbTemplateCloudReduction;
    QLabel *laScoreComputingReduction;
    QLabel *laCloudsParams;
    QLabel *laKSmooth;
    QLabel *laRigidMotion;
    QSpinBox *sbKSmooth;
    QDoubleSpinBox *dsbKTransSmoothConst;
    QDoubleSpinBox *dsbKRotSmoothConst;
    QLabel *laKTransConst;
    QLabel *laKRotSmooth;
    QGroupBox *gbActions;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *pbStop;
    QPushButton *pbStart;
    QCheckBox *cbExpertMode;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SWUI_WEmicpHeadTracking)
    {
        if (SWUI_WEmicpHeadTracking->objectName().isEmpty())
            SWUI_WEmicpHeadTracking->setObjectName(QString::fromUtf8("SWUI_WEmicpHeadTracking"));
        SWUI_WEmicpHeadTracking->resize(1165, 860);
        centralwidget = new QWidget(SWUI_WEmicpHeadTracking);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        vlMain = new QVBoxLayout();
        vlMain->setObjectName(QString::fromUtf8("vlMain"));
        glDisplay = new QGridLayout();
        glDisplay->setObjectName(QString::fromUtf8("glDisplay"));
        gbClouds = new QGroupBox(centralwidget);
        gbClouds->setObjectName(QString::fromUtf8("gbClouds"));
        horizontalLayout_7 = new QHBoxLayout(gbClouds);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        hlClouds = new QHBoxLayout();
        hlClouds->setObjectName(QString::fromUtf8("hlClouds"));

        horizontalLayout_7->addLayout(hlClouds);


        glDisplay->addWidget(gbClouds, 0, 1, 1, 1);

        gbVideo = new QGroupBox(centralwidget);
        gbVideo->setObjectName(QString::fromUtf8("gbVideo"));
        horizontalLayout_3 = new QHBoxLayout(gbVideo);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        hlVideo = new QHBoxLayout();
        hlVideo->setObjectName(QString::fromUtf8("hlVideo"));

        horizontalLayout_3->addLayout(hlVideo);


        glDisplay->addWidget(gbVideo, 0, 0, 1, 1);

        gbRotations = new QGroupBox(centralwidget);
        gbRotations->setObjectName(QString::fromUtf8("gbRotations"));
        horizontalLayout_8 = new QHBoxLayout(gbRotations);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        hlRotations = new QHBoxLayout();
        hlRotations->setObjectName(QString::fromUtf8("hlRotations"));

        horizontalLayout_8->addLayout(hlRotations);


        glDisplay->addWidget(gbRotations, 1, 0, 1, 1);

        gbTranslations = new QGroupBox(centralwidget);
        gbTranslations->setObjectName(QString::fromUtf8("gbTranslations"));
        horizontalLayout_9 = new QHBoxLayout(gbTranslations);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        hlTranslations = new QHBoxLayout();
        hlTranslations->setObjectName(QString::fromUtf8("hlTranslations"));

        horizontalLayout_9->addLayout(hlTranslations);


        glDisplay->addWidget(gbTranslations, 1, 1, 1, 1);


        vlMain->addLayout(glDisplay);

        hlButtons = new QHBoxLayout();
        hlButtons->setObjectName(QString::fromUtf8("hlButtons"));
        gbParameters = new QGroupBox(centralwidget);
        gbParameters->setObjectName(QString::fromUtf8("gbParameters"));
        gridLayout_3 = new QGridLayout(gbParameters);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        glParameters = new QGridLayout();
        glParameters->setObjectName(QString::fromUtf8("glParameters"));
        cbSmoothRigidMotion = new QCheckBox(gbParameters);
        cbSmoothRigidMotion->setObjectName(QString::fromUtf8("cbSmoothRigidMotion"));
        cbSmoothRigidMotion->setEnabled(false);
        cbSmoothRigidMotion->setChecked(false);

        glParameters->addWidget(cbSmoothRigidMotion, 1, 5, 1, 1);

        laTemplateCloudReduction = new QLabel(gbParameters);
        laTemplateCloudReduction->setObjectName(QString::fromUtf8("laTemplateCloudReduction"));

        glParameters->addWidget(laTemplateCloudReduction, 1, 2, 1, 1, Qt::AlignHCenter);

        laTargetCloudReduction = new QLabel(gbParameters);
        laTargetCloudReduction->setObjectName(QString::fromUtf8("laTargetCloudReduction"));

        glParameters->addWidget(laTargetCloudReduction, 2, 2, 1, 1, Qt::AlignHCenter);

        dsbScoreComputingReduction = new QDoubleSpinBox(gbParameters);
        dsbScoreComputingReduction->setObjectName(QString::fromUtf8("dsbScoreComputingReduction"));
        dsbScoreComputingReduction->setEnabled(false);
        dsbScoreComputingReduction->setMaximum(1);
        dsbScoreComputingReduction->setSingleStep(0.01);
        dsbScoreComputingReduction->setValue(0.1);

        glParameters->addWidget(dsbScoreComputingReduction, 3, 3, 1, 1);

        dsbTargetCloudReduction = new QDoubleSpinBox(gbParameters);
        dsbTargetCloudReduction->setObjectName(QString::fromUtf8("dsbTargetCloudReduction"));
        dsbTargetCloudReduction->setMinimum(0.01);
        dsbTargetCloudReduction->setMaximum(1);
        dsbTargetCloudReduction->setSingleStep(0.01);
        dsbTargetCloudReduction->setValue(0.2);

        glParameters->addWidget(dsbTargetCloudReduction, 2, 3, 1, 1);

        dsbP2 = new QDoubleSpinBox(gbParameters);
        dsbP2->setObjectName(QString::fromUtf8("dsbP2"));
        dsbP2->setEnabled(false);
        dsbP2->setDecimals(2);
        dsbP2->setMinimum(0);
        dsbP2->setMaximum(1000);
        dsbP2->setSingleStep(0.01);
        dsbP2->setValue(0.01);

        glParameters->addWidget(dsbP2, 1, 1, 1, 1);

        laP2 = new QLabel(gbParameters);
        laP2->setObjectName(QString::fromUtf8("laP2"));

        glParameters->addWidget(laP2, 1, 0, 1, 1, Qt::AlignHCenter);

        laEMICPParams = new QLabel(gbParameters);
        laEMICPParams->setObjectName(QString::fromUtf8("laEMICPParams"));

        glParameters->addWidget(laEMICPParams, 0, 0, 1, 2, Qt::AlignHCenter);

        dsbFactor = new QDoubleSpinBox(gbParameters);
        dsbFactor->setObjectName(QString::fromUtf8("dsbFactor"));
        dsbFactor->setEnabled(true);
        dsbFactor->setMinimum(0.01);
        dsbFactor->setMaximum(0.99);
        dsbFactor->setSingleStep(0.01);
        dsbFactor->setValue(0.75);

        glParameters->addWidget(dsbFactor, 3, 1, 1, 1);

        dsbINF = new QDoubleSpinBox(gbParameters);
        dsbINF->setObjectName(QString::fromUtf8("dsbINF"));
        dsbINF->setEnabled(false);
        dsbINF->setDecimals(6);
        dsbINF->setMinimum(1e-06);
        dsbINF->setMaximum(0.01);
        dsbINF->setSingleStep(1e-06);
        dsbINF->setValue(1e-05);

        glParameters->addWidget(dsbINF, 2, 1, 1, 1);

        laFactor = new QLabel(gbParameters);
        laFactor->setObjectName(QString::fromUtf8("laFactor"));

        glParameters->addWidget(laFactor, 3, 0, 1, 1, Qt::AlignHCenter);

        laInf = new QLabel(gbParameters);
        laInf->setObjectName(QString::fromUtf8("laInf"));

        glParameters->addWidget(laInf, 2, 0, 1, 1, Qt::AlignHCenter);

        laD02 = new QLabel(gbParameters);
        laD02->setObjectName(QString::fromUtf8("laD02"));

        glParameters->addWidget(laD02, 4, 0, 1, 1, Qt::AlignHCenter);

        dsbD02 = new QDoubleSpinBox(gbParameters);
        dsbD02->setObjectName(QString::fromUtf8("dsbD02"));
        dsbD02->setEnabled(false);
        dsbD02->setMaximum(100);
        dsbD02->setSingleStep(0.01);
        dsbD02->setValue(0.01);

        glParameters->addWidget(dsbD02, 4, 1, 1, 1);

        dsbTemplateCloudReduction = new QDoubleSpinBox(gbParameters);
        dsbTemplateCloudReduction->setObjectName(QString::fromUtf8("dsbTemplateCloudReduction"));
        dsbTemplateCloudReduction->setMinimum(0.01);
        dsbTemplateCloudReduction->setMaximum(1);
        dsbTemplateCloudReduction->setSingleStep(0.01);
        dsbTemplateCloudReduction->setValue(0.2);

        glParameters->addWidget(dsbTemplateCloudReduction, 1, 3, 1, 1);

        laScoreComputingReduction = new QLabel(gbParameters);
        laScoreComputingReduction->setObjectName(QString::fromUtf8("laScoreComputingReduction"));

        glParameters->addWidget(laScoreComputingReduction, 3, 2, 1, 1, Qt::AlignHCenter);

        laCloudsParams = new QLabel(gbParameters);
        laCloudsParams->setObjectName(QString::fromUtf8("laCloudsParams"));

        glParameters->addWidget(laCloudsParams, 0, 2, 1, 2, Qt::AlignHCenter);

        laKSmooth = new QLabel(gbParameters);
        laKSmooth->setObjectName(QString::fromUtf8("laKSmooth"));

        glParameters->addWidget(laKSmooth, 2, 4, 1, 1, Qt::AlignHCenter);

        laRigidMotion = new QLabel(gbParameters);
        laRigidMotion->setObjectName(QString::fromUtf8("laRigidMotion"));

        glParameters->addWidget(laRigidMotion, 0, 4, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);

        sbKSmooth = new QSpinBox(gbParameters);
        sbKSmooth->setObjectName(QString::fromUtf8("sbKSmooth"));
        sbKSmooth->setEnabled(false);
        sbKSmooth->setMaximum(50);
        sbKSmooth->setValue(5);

        glParameters->addWidget(sbKSmooth, 2, 5, 1, 1);

        dsbKTransSmoothConst = new QDoubleSpinBox(gbParameters);
        dsbKTransSmoothConst->setObjectName(QString::fromUtf8("dsbKTransSmoothConst"));
        dsbKTransSmoothConst->setEnabled(false);
        dsbKTransSmoothConst->setMinimum(1);
        dsbKTransSmoothConst->setMaximum(5000);
        dsbKTransSmoothConst->setSingleStep(10);
        dsbKTransSmoothConst->setValue(10);

        glParameters->addWidget(dsbKTransSmoothConst, 3, 5, 1, 1);

        dsbKRotSmoothConst = new QDoubleSpinBox(gbParameters);
        dsbKRotSmoothConst->setObjectName(QString::fromUtf8("dsbKRotSmoothConst"));
        dsbKRotSmoothConst->setEnabled(false);
        dsbKRotSmoothConst->setMinimum(1);
        dsbKRotSmoothConst->setMaximum(5000);
        dsbKRotSmoothConst->setSingleStep(10);
        dsbKRotSmoothConst->setValue(10);

        glParameters->addWidget(dsbKRotSmoothConst, 4, 5, 1, 1);

        laKTransConst = new QLabel(gbParameters);
        laKTransConst->setObjectName(QString::fromUtf8("laKTransConst"));

        glParameters->addWidget(laKTransConst, 3, 4, 1, 1, Qt::AlignHCenter);

        laKRotSmooth = new QLabel(gbParameters);
        laKRotSmooth->setObjectName(QString::fromUtf8("laKRotSmooth"));

        glParameters->addWidget(laKRotSmooth, 4, 4, 1, 1, Qt::AlignHCenter);


        gridLayout_3->addLayout(glParameters, 0, 0, 1, 1);


        hlButtons->addWidget(gbParameters);

        gbActions = new QGroupBox(centralwidget);
        gbActions->setObjectName(QString::fromUtf8("gbActions"));
        gridLayout_2 = new QGridLayout(gbActions);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pbStop = new QPushButton(gbActions);
        pbStop->setObjectName(QString::fromUtf8("pbStop"));
        pbStop->setEnabled(false);
        pbStop->setMaximumSize(QSize(100, 16777215));

        gridLayout->addWidget(pbStop, 1, 0, 1, 1);

        pbStart = new QPushButton(gbActions);
        pbStart->setObjectName(QString::fromUtf8("pbStart"));
        pbStart->setMaximumSize(QSize(100, 16777215));

        gridLayout->addWidget(pbStart, 0, 0, 1, 1);

        cbExpertMode = new QCheckBox(gbActions);
        cbExpertMode->setObjectName(QString::fromUtf8("cbExpertMode"));

        gridLayout->addWidget(cbExpertMode, 2, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        hlButtons->addWidget(gbActions);

        hlButtons->setStretch(0, 4);
        hlButtons->setStretch(1, 1);

        vlMain->addLayout(hlButtons);

        vlMain->setStretch(0, 6);
        vlMain->setStretch(1, 1);

        horizontalLayout->addLayout(vlMain);

        SWUI_WEmicpHeadTracking->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SWUI_WEmicpHeadTracking);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1165, 21));
        SWUI_WEmicpHeadTracking->setMenuBar(menubar);
        statusbar = new QStatusBar(SWUI_WEmicpHeadTracking);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        SWUI_WEmicpHeadTracking->setStatusBar(statusbar);

        retranslateUi(SWUI_WEmicpHeadTracking);
        QObject::connect(pbStart, SIGNAL(clicked(bool)), pbStart, SLOT(setEnabled(bool)));
        QObject::connect(pbStart, SIGNAL(clicked(bool)), pbStop, SLOT(setDisabled(bool)));
        QObject::connect(pbStop, SIGNAL(clicked(bool)), pbStop, SLOT(setEnabled(bool)));
        QObject::connect(pbStop, SIGNAL(clicked(bool)), pbStart, SLOT(setDisabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), cbSmoothRigidMotion, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), sbKSmooth, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbKTransSmoothConst, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbKRotSmoothConst, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbD02, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbScoreComputingReduction, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbINF, SLOT(setEnabled(bool)));
        QObject::connect(cbExpertMode, SIGNAL(toggled(bool)), dsbP2, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(SWUI_WEmicpHeadTracking);
    } // setupUi

    void retranslateUi(QMainWindow *SWUI_WEmicpHeadTracking)
    {
        SWUI_WEmicpHeadTracking->setWindowTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "MainWindow", 0, QApplication::UnicodeUTF8));
        gbClouds->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Clouds", 0, QApplication::UnicodeUTF8));
        gbVideo->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Video", 0, QApplication::UnicodeUTF8));
        gbRotations->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Rotations", 0, QApplication::UnicodeUTF8));
        gbTranslations->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Translations", 0, QApplication::UnicodeUTF8));
        gbParameters->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Parameters", 0, QApplication::UnicodeUTF8));
        cbSmoothRigidMotion->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Smooth rigid motion", 0, QApplication::UnicodeUTF8));
        laTemplateCloudReduction->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Template cloud reduction :", 0, QApplication::UnicodeUTF8));
        laTargetCloudReduction->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Target cloud reduction :", 0, QApplication::UnicodeUTF8));
        laP2->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "P2 :", 0, QApplication::UnicodeUTF8));
        laEMICPParams->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "EMICP", 0, QApplication::UnicodeUTF8));
        laFactor->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Factor :", 0, QApplication::UnicodeUTF8));
        laInf->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "INF :", 0, QApplication::UnicodeUTF8));
        laD02->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "D02 :", 0, QApplication::UnicodeUTF8));
        laScoreComputingReduction->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Score computing reduction :", 0, QApplication::UnicodeUTF8));
        laCloudsParams->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Clouds", 0, QApplication::UnicodeUTF8));
        laKSmooth->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "K smooth", 0, QApplication::UnicodeUTF8));
        laRigidMotion->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Rigid motion", 0, QApplication::UnicodeUTF8));
        laKTransConst->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "K trans smooth const", 0, QApplication::UnicodeUTF8));
        laKRotSmooth->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "K rot smooth const", 0, QApplication::UnicodeUTF8));
        gbActions->setTitle(QApplication::translate("SWUI_WEmicpHeadTracking", "Actions", 0, QApplication::UnicodeUTF8));
        pbStop->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Stop", 0, QApplication::UnicodeUTF8));
        pbStart->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Start", 0, QApplication::UnicodeUTF8));
        cbExpertMode->setText(QApplication::translate("SWUI_WEmicpHeadTracking", "Expert mode", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SWUI_WEmicpHeadTracking: public Ui_SWUI_WEmicpHeadTracking {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SWUI_WEMICPHEADTRACKING_H
