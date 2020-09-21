#include "data.h"

#include "plot.h"

#include <shogun/base/init.h>
#include <shogun/base/some.h>
#include <shogun/evaluation/CrossValidation.h>
#include <shogun/evaluation/MeanAbsoluteError.h>
#include <shogun/evaluation/MeanSquaredError.h>
#include <shogun/evaluation/StratifiedCrossValidationSplitting.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/kernel/PolyKernel.h>
#include <shogun/kernel/normalizer/SqrtDiagKernelNormalizer.h>
#include <shogun/labels/RegressionLabels.h>
#include <shogun/modelselection/GridSearchModelSelection.h>
#include <shogun/modelselection/ModelSelectionParameters.h>
#include <shogun/modelselection/ParameterCombination.h>
#include <shogun/preprocessor/RescaleFeatures.h>
#include <shogun/regression/KernelRidgeRegression.h>
#include <shogun/regression/LinearRidgeRegression.h>

#include <iostream>
#include <random>

using namespace shogun;

int main(int, char*[])
{
    shogun::init_shogun_with_defaults();


    const size_t seed = 3463;
    const int32_t num_samples = 1000;
    SGMatrix<float64_t> x_values;
    SGVector<float64_t> y_values;
    std::tie(x_values, y_values) = GenarateData(num_samples, seed, false);

    auto x = some<CDenseFeatures<float64_t>>(x_values);
    auto y = some<CRegressionLabels>(y_values);

    auto mm = std::minmax_element(x_values.begin(), x_values.end(),
                                  [](const auto& a, const auto& b){return a < b;});
    std::pair<float64_t, float64_t> x_minmax {*mm.first, *mm.second};

    auto x_scaler = some<CRescaleFeatures>();
    x_scaler -> fit(x);
    x_scaler ->transform(x , true);

    auto kernel = some<CPolyKernel>(256, 15);
    kernel->init(x, x);
    auto kernel_normalizer = some<CSqrtDiagKernelNormalizer>();
    kernel->set_normalizer(kernel_normalizer);

    float64_t tau_regularization = 0.00000001;
    float64_t tau_regularization_max = 0.000001;
    auto model = some<CKernelRidgeRegression>(tau_regularization, kernel, y);

    auto spliting_strategy = some<CStratifiedCrossValidationSplitting>(y, 5);
    auto evaluation_criterium = some<CMeanSquaredError>();
    auto cross_validation = some<CCrossValidation>(
                model, x, y, spliting_strategy, evaluation_criterium);
    cross_validation->set_autolock(false);
    cross_validation->set_num_runs(1);

    auto param_root = some<CModelSelectionParameters>();
    auto param_tau = some<CModelSelectionParameters>("tau");
    param_root->append_child(param_tau);
    param_tau->build_values(tau_regularization, tau_regularization_max,
                            ERangeType::R_LINEAR, tau_regularization_max);
    auto param_kernel = some<CModelSelectionParameters>("kernel", kernel);
    auto param_kernel_degree = some<CModelSelectionParameters>("degree");
    param_kernel_degree->build_values(5, 15, ERangeType::R_LINEAR, 1);
    param_kernel->append_child(param_kernel_degree);
    param_root->append_child(param_kernel);

    auto model_selection =
            some<CGridSearchModelSelection>(cross_validation, param_root);
    auto best_parameters = model_selection->select_model(true);
    best_parameters->apply_to_machine(model);
    best_parameters->print_tree();

    if  (not model->train())
        std::cerr << "training failed\n";

    auto y_predict = wrap(model->apply_regression(x));
    auto mse_error = some<CMeanSquaredError>();
    auto mse = mse_error->evaluate(y_predict, y);
    std::cout << "mse = " << mse << std::endl;

    auto mae_error = some<CMeanAbsoluteError>();
    auto mae = mae_error->evaluate(y_predict, y);
    std::cout << "mae = " << mae << std::endl;

    auto new_x_value = LinSpace(x_minmax.first, x_minmax.second, 50);
    auto new_x = some<CDenseFeatures<float64_t>>(new_x_value);
    x_scaler->transform(new_x, true);
    y_predict = wrap(model->apply_regression(new_x));

    auto x_coords = x->get_feature_matrix();
    auto y_coords = y->get_labels();
    auto x_pred_values = new_x->get_feature_matrix();
    auto y_pred_values = y_predict->get_labels();

    plotcpp::Plot plt(true);
    plt.SetTerminal("png");
    plt.SetOutput("plot.png");
    plt.SetTitle("polynomial regression");
    plt.SetXLabel("x");
    plt.SetYLabel("y");
    plt.SetAutoscale();
    plt.GnuplotCommand("set grid");

    plt.Draw2D(
                plotcpp::Lines(x_pred_values.begin(), x_pred_values.end(),
                               y_pred_values.begin(), "pred", "lc rgb 'red' lw 2"),
                plotcpp::Points(x_coords.begin(), x_coords.end(), y_coords.begin()
                                ,"orig", "lc rgb 'black' pt 7"));
    plt.Flush();
    plt.Flush();

    shogun::exit_shogun();
    return 0;
}
