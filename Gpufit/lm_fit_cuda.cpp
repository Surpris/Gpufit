#include "lm_fit.h"

LMFitCUDA::LMFitCUDA(
    float const tolerance,
    Info const & info,
    GPUData & gpu_data,
    int const n_fits
    ) :
    info_(info),
    gpu_data_(gpu_data),
    n_fits_(n_fits),
    all_finished_(false),
    tolerance_(tolerance)
{
}

LMFitCUDA::~LMFitCUDA()
{
}

void LMFitCUDA::run()
{
    // initialize the chi-square values
	calc_curve_values();
    calc_chi_squares();
    calc_gradients();
    calc_hessians();

    gpu_data_.copy(
        gpu_data_.prev_chi_squares_,
        gpu_data_.chi_squares_,
        n_fits_);

    // loop over the fit iterations
    for (int iteration = 0; !all_finished_; iteration++)
    {
        // modify step width
        // Gauss Jordan
        // update fitting parameters
        solve_equation_system();

        // calculate fitting curve values and its derivatives
        // calculate chi-squares, gradients and hessians
		calc_curve_values();
        calc_chi_squares();
        calc_gradients();
        calc_hessians();

        // check which fits have converged
        // flag finished fits
        // check whether all fits finished
        // save the number of needed iterations by each fitting process
        // check whether chi-squares are increasing or decreasing
        // update chi-squares, curve parameters and lambdas
        evaluate_iteration(iteration);
    }
}