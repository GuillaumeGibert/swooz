/**
 * \file SWApplyTemporalFilter.h
 * \brief Defines SWApplyTemporalFilter
 * \author Guillaume Gibert
 * \date 01/06/13
 */

#ifndef _SWAPPLYTEMPORALFILTER_
#define _SWAPPLYTEMPORALFILTER_

#include <itpp/itbase.h>
#include <itpp/itsignal.h>

/**
 * @brief The SWApplyTemporalFilter class
 *
 * ...
 */
class SWApplyTemporalFilter
{
	public:

            /**
             * @brief Filter the signal using itpp filter method
             * @param denomCoefFilter
             * @param numCoefFilter
             * @param input
             * @param output
             */
            void itppFilter(const itpp::vec & denomCoefFilter, const itpp::vec & numCoefFilter, const itpp::vec & input, itpp::vec & output);

            /**
             * @brief Filter the signal using itpp filter method
             * @param denomCoefFilter
             * @param numCoefFilter
             * @param input
             * @param output
             */
            void SWFilter(const itpp::vec & denomCoefFilter, const itpp::vec & numCoefFilter, const itpp::vec & input, itpp::vec & output);

	private:

            /**
             * @brief ComputeFilterInitialCondition
             * @param b
             * @param a
             * @param zi
             */
            void ComputeFilterInitialCondition(const itpp::vec & b, const itpp::vec & a, itpp::vec & zi);

            /**
             * @brief IIR_Filter
             * @param b
             * @param a
             * @param data
             * @param V0
             * @param dataFiltered
             * @param Vf
             */
            void IIR_Filter (const itpp::vec & b, const itpp::vec & a, const itpp::vec & data, const itpp::vec & V0, itpp::vec &dataFiltered, itpp::vec &Vf);

            /**
             * @brief Filtfilt
             * @param b
             * @param a
             * @param data
             * @param dataFiltered
             */
            void Filtfilt (const itpp::vec & b, const itpp::vec & a, const itpp::vec & data, itpp::vec &dataFiltered);
};

#endif
