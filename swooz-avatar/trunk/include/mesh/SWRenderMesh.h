#ifndef SWRENDERMESH_H
#define SWRENDERMESH_H

#endif // SWRENDERMESH_H


namespace swMesh
{
    class SWRenderMesh : public swMesh
    {
        public :

            /**
             * @brief Default constructor of SWRenderMesh
             */
            SWRenderMesh();


        private :


            std::string m_sName;
            std::string m_sPath;

            std::string m_sTexturePath;


            std::vector<int> m_vUnicolor;

            std::vector<double> m_vKAmbiant;
            std::vector<double> m_vKDiffus;
            std::vector<double> m_vKSpecular;
            std::vector<double> m_vKEmission;

            double m_dShininess;      // Ns

            double m_dOpticalDensity; // Ni

            int m_illuminationMode;

//            material specular exponent


//            %   material(2).type='Ka';
//            %   material(2).data=[0.8 0.4 0.4];
//            %   material(3).type='Kd';
//            %   material(3).data=[0.8 0.4 0.4];
//            %   material(4).type='Ks';
//            %   material(4).data=[1 1 1];

//          illum sepcifies the illumination mode (0=constant, 1=diffuse, 2=diffuse+specular, ...).

//            Ka is the ambient color.

//            Kd is the diffuse color.

//            Ks is the specular color.

//            Ke is the emissive color.

//            Ns is the specular exponent (i.e. the shininess).

//            Ni is the index of reflection.

//            d, Tr and Tf are all transparency values. d and Tr are given as floats and Tf as a color.

//            sharpness...

//            illum illum_#

//            The "illum" statement specifies the illumination model to use in the
//           material.  Illumination models are mathematical equations that represent
//           various material lighting and shading effects.

//            "illum_#"can be a number from 0 to 10.  The illumination models are
//           summarized below; for complete descriptions see "Illumination models" on
//           page 5-30.

//            Illumination    Properties that are turned on in the
//            model           Property Editor

//            0		Color on and Ambient off
//            1		Color on and Ambient on
//            2		Highlight on
//            3		Reflection on and Ray trace on
//            4		Transparency: Glass on
//                   Reflection: Ray trace on
//            5		Reflection: Fresnel on and Ray trace on
//            6		Transparency: Refraction on
//                   Reflection: Fresnel off and Ray trace on
//            7		Transparency: Refraction on
//                   Reflection: Fresnel on and Ray trace on
//            8		Reflection on and Ray trace off
//            9		Transparency: Glass on
//                   Reflection: Ray trace off

    };




}
