

/**
 * \file SWComputeTemporalFilterCoefficients.h
 * \brief Defines SWComputeTemporalFilterCoefficients
 * \author Guillaume Gibert
 * \date ../../..
 */

#ifndef _SWCOMPUTETEMPORTALFILTERCOEFFICIENTS_
#define _SWCOMPUTETEMPORTALFILTERCOEFFICIENTS_

#include <itpp/itstat.h>
#include <itpp/itsignal.h>
#include <commonTypes.h>
// #include "../../ovp_defines.h"
// #include <openvibe/ov_all.h>
// #include <openvibe-toolkit/ovtk_all.h>

#define PREC 27
#define MAXEXP 1024
#define MINEXP -1077
#define MAXNUM 1.79769313486231570815E308

#define OVP_TypeId_FilterType_HighPass 0
#define OVP_TypeId_FilterType_LowPass 1
#define OVP_TypeId_FilterType_BandStop 2
#define OVP_TypeId_FilterType_BandPass 3
#define OVP_TypeId_FilterMethod_Butterworth 4
#define OVP_TypeId_FilterMethod_Chebychev 5
#define OVP_TypeId_FilterMethod_YuleWalker 6

/**
 * \struct cmplex
 * \brief Complex type structure
 * \author Guillaume Gibert
 * \date ../../..
 */
typedef struct
{
	double real; /**< real member of the complex */
	double imag; /**< imaginary member of the complex */
}cmplex;


namespace SWPlugins
{
		/**
		 * \class CComputeTemporalFilterCoefficients
		 * \brief ...
		 * \author Guillaume Gibert
		 * \date ../../..
		 */	
		class CComputeTemporalFilterCoefficients
		{
			
			public:
			
				/**
				 * \brief Release
				 */				
				virtual void release(void) { delete this; }

				// virtual OpenViBE::boolean initialize(void);
				// virtual OpenViBE::boolean uninitialize(void);
				// virtual OpenViBE::boolean process(void);
				// _IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients);

				// Functions for Butterworth and Chebychev filters
				
				/**
				 * \brief compute s plane poles and zeros
				 */					
				void findSPlanePolesAndZeros(void);
				
				/**
				 * \brief convert s plane poles and zeros to the z plane.
				 */					
				void convertSPlanePolesAndZerosToZPlane(void);
				
				// Functions for Complex arithmetic
				
				/**
				 * \brief Compute abs of a complex
				 * \param [in] z : input complex
				 * \return abs of the complex
				 */				
				double absComplex(cmplex *z);
				
				/**
				 * \brief Divide 2 complex numbers
				 * \param [in] a : first complex 
				 * \param [in] b : second complex
				 * \param [in] c : result complex
				 */					
				void divComplex(cmplex *a, cmplex *b, cmplex *c);
				
				/**
				 * \brief Compute sqrt of a complex number
				 * \param [in] z : input complex
				 * \param [in] w : result complex
				 */					
				void sqrtComplex(cmplex *z, cmplex  *w);
				
				/**
				 * \brief Add 2 complexes
				 * \param [in] a : first complex 
				 * \param [in] b : second complex
				 * \param [in] c : result complex
				 */					
				void addComplex(cmplex *a, cmplex *b,cmplex *c);
				
				/**
				 * \brief Multiply 2 complexes
				 * \param [in] a : first complex 
				 * \param [in] b : second complex
				 * \param [in] c : result complex
				 */							
				void mulComplex(cmplex *a, cmplex *b,cmplex *c);
				
				/**
				 * \brief Substract 2 complex
				 * \param [in] a : first complex 
				 * \param [in] b : second complex
				 * \param [in] c : result complex
				 */							
				void subComplex(cmplex *a, cmplex *b,cmplex *c);

				/**
				 * \brief ...
				 * \param [in] float64LowPassBandEdge  : ...
				 * \param [in] float64HighPassBandEdge : ...
				 * \param [in] ui32SamplingRate        : ...
				 * \param [in] ui32FilterOrder	       : ...
				 * \param [in] ui64FilterType 	       : ...
				 * \param [in] ui64FilterMethod        : ...
				 */			
				void initialize(double float64LowPassBandEdge, double float64HighPassBandEdge, 
						uint32 ui32SamplingRate, uint32 ui32FilterOrder, uint64 ui64FilterType, uint64 ui64FilterMethod);
					
				/**
				 * \brief ...
				 */						
				void process(void);

				/**
				 * \brief ...
				 * \return ...
				 */	
				inline itpp::vec * getVecNumCoefFilter()	{ return & m_vecNumCoefFilter;}
				
				/**
				 * \brief ...
				 * \return ...
				 */					
				inline itpp::vec * getVecDenomCoefFilter()	{ return & m_vecDenomCoefFilter;}

			protected:

				// OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingFrequency;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterMethod;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterType;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterOrder;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64LowCutFrequency;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64HighCutFrequency;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64BandPassRipple;
				// OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pOutputMatrix;

				//REDEFINIR
			
				uint32 m_ui32FilterOrder; 		/**< ... */
				uint32  m_ui32DimensionSize;		/**< ... */
				uint32 m_ui32ArraySize;			/**< ... */
				uint32 m_ui32SamplingRate;		/**< ... */
				uint32 m_ui32NyquistFrequency;		/**< ... */
				uint32  m_ui32NbPoles;			/**< ... */
				uint32  m_ui32NbZeros;			/**< ... */
				uint32  m_ui32zord;			/**< ... */
			
				uint64 m_ui64FilterMethod; 		/**< ... */
				uint64 m_ui64FilterType;		/**< ... */
			
				double m_float64LowPassBandEdge;	/**< ... */
				double m_float64HighPassBandEdge;	/**< ... */
				double m_float64PassBandRipple;		/**< ... */
				double m_float64Phi;			/**< ... */
				double m_float64Scale;			/**< ... */
				double m_float64TanAng;			/**< ... */
				double m_float64CosGam;			/**< ... */
				double m_float64cbp;			/**< ... */
				double m_float64Rho;			/**< ... */
				double m_float64Eps;			/**< ... */				
				
				itpp::vec m_vecNumCoefFilter;		/**< ... */
				itpp::vec m_vecDenomCoefFilter;		/**< ... */
				itpp::vec m_vecZs;			/**< ... */
		};

		// class CComputeTemporalFilterCoefficientsDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		// {
		// public:

		// 	virtual void release(void) { }

		// 	virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Compute Filter Coefficients"); }
		// 	virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
		// 	virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
		// 	virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
		// 	virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
		// 	virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Signal processing/Filter"); }
		// 	virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

		// 	virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients; }
		// 	virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CComputeTemporalFilterCoefficients(); }

		// 	virtual OpenViBE::boolean getAlgorithmPrototype(
		// 		OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
		// 	{
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_SamplingFrequency, "Sampling frequency",         OpenViBE::Kernel::ParameterType_UInteger);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterMethod,      "Filter method",              OpenViBE::Kernel::ParameterType_UInteger);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterType,        "Filter type",                OpenViBE::Kernel::ParameterType_UInteger);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterOrder,       "Filter order",               OpenViBE::Kernel::ParameterType_UInteger);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_LowCutFrequency,   "Low cut frequency",          OpenViBE::Kernel::ParameterType_Float);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_HighCutFrequency,  "High cut frequency",         OpenViBE::Kernel::ParameterType_Float);
		// 		rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_BandPassRipple,    "Band pass ripple",           OpenViBE::Kernel::ParameterType_Float);
		// 		rAlgorithmProto.addOutputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_OutputParameterId_Matrix,           "Matrix",                     OpenViBE::Kernel::ParameterType_Matrix);
		// 		rAlgorithmProto.addInputTrigger(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_Initialize,             "Initialize");
		// 		rAlgorithmProto.addInputTrigger(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_ComputeCoefficients,    "Compute coefficients");

		// 		return true;
		// 	}

		// 	_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficientsDesc);
		// };
};

#endif
