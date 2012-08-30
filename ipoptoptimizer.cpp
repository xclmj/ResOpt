#include "ipoptoptimizer.h"
#include "ipoptinterface.h"

#include <iostream>
#include <QString>

using std::cout;
using std::endl;

namespace ResOpt
{

IpoptOptimizer::IpoptOptimizer(Runner *r)
    : Optimizer(r)
{}

IpoptOptimizer::~IpoptOptimizer()
{
    // nothing to do, the SmartPtr will delete itself
}

//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void IpoptOptimizer::initialize()
{

    cout << "Initializing Ipopt..." << endl;

    // setting up the TNLP
    p_tnlp = new IpoptInterface(this);

    // Create an instance of the IpoptApplication
    // We are using the factory, since this allows us to compile this
    // example with an Ipopt Windows DLL
    app = IpoptApplicationFactory();

    // Change some options
    // Note: The following choices are only examples, they might not be
    //       suitable for your optimization problem.
    // app->Options()->SetNumericValue("tol", 1e-7);
    //app->Options()->SetStringValue("mu_strategy", "adaptive");
    app->Options()->SetStringValue("output_file", "output/ipopt.out");

    // Derivative checks
    app->Options()->SetStringValue("check_derivatives_for_naninf", "yes"); // no (default) or yes (may produce a lot of output)
    app->Options()->SetStringValue("derivative_test", "first-order"); // none (default)

    // Quasi-Newton approximation of Hessian of Lagrangian
    app->Options()->SetStringValue("hessian_approximation", "limited-memory"); // exact (default, no approx) or limited-memory (quasi-Newton)
    //app->Options()->SetStringValue("limited_memory_update_type", "bfgs"); // BFGS (default) or SR1 (not working well)

    // Initialize the IpoptApplication and process the options
    ApplicationReturnStatus status;
    status = app->Initialize();
    if (status != Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** Error during initialization!" << std::endl;
        //return (int) status;
        exit(1);
    }
    else
    {
        // changing the status to initialized
        setInitialized(true);
    }

}

//-----------------------------------------------------------------------------------------------
// Starts the optimization
//-----------------------------------------------------------------------------------------------
void IpoptOptimizer::start()
{

    // checking if everything is initialized
    if(!isInitialized()) initialize();

    //Set up done, now let's run Ipopt
    ApplicationReturnStatus status;
    status = app->OptimizeTNLP(GetRawPtr(p_tnlp));

    if (status == Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** The problem solved!" << std::endl;
    }
    else
    {
        std::cout << std::endl << std::endl << "*** The problem FAILED!" << std::endl;
    }

    // As the SmartPtrs go out of scope, the reference count
    // will be decremented and the objects will automatically
    // be deleted.
    //return (int) status;

    // letting the runner know the optimization has finished
    emit finished();
}

} // namespace ResOpt
