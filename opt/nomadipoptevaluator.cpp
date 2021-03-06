/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2013 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "nomadipoptevaluator.h"

#include <tr1/memory>
#include <iostream>

#include "nomadipoptoptimizer.h"
//#include "nomadipoptinterface.h"
#include "runner.h"
#include "model.h"
#include "reservoirsimulator.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "case.h"
#include "minlpevaluator.h"

//#include "IpIpoptApplication.hpp"
//#include "IpSolveStatistics.hpp"

//using namespace Ipopt;


using std::tr1::shared_ptr;
using std::endl;
using std::cout;

namespace ResOpt
{

NomadIpoptEvaluator::NomadIpoptEvaluator(const NOMAD::Parameters &p, NomadIpoptOptimizer *o)
    : NOMAD::Evaluator(p),
      p_optimizer(o),
      p_result_best(0)
{
    p_eval = new MINLPEvaluator(o);
}

NomadIpoptEvaluator::~NomadIpoptEvaluator()
{
    //for(int i = 0; i < m_results.size(); ++i) delete m_results.at(i);

    if(p_result_best != 0) delete p_result_best;

    delete p_eval;
}

//-----------------------------------------------------------------------------------------------
// evaluates the model
//-----------------------------------------------------------------------------------------------
bool NomadIpoptEvaluator::eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval)
{
    cout << "//// staring new NOMAD  evaluation ////" << endl;

    cout << "//// generating case ////" << endl;
    // generating a case from the evaluation point
    Case *c = generateCase(x);

    cout << "//// solving sub-problem ////" << endl;
    // sending the case off for evaluation by IPOPT
    Case *result = p_eval->solveContineousProblem(c);

    // need to re-run the case to get constraint values (done in MINLPEValuator now...)
    //p_optimizer->runCase(result);


    cout << "//// getting objective value ////" << endl;
    // extracting the objective
    x.set_bb_output(0, -result->objectiveValue());




    cout << "//// calculating constraint values ////" << endl;

    // extracting the constraint values
    // the constraints in NOMAD must be on the form: c <= 0

    for(int i = 0; i < result->numberOfConstraints(); ++i)
    {
        double val_input;
        double val = result->constraintValue(i);
        double max = p_optimizer->runner()->model()->constraints().at(i)->max() + 0.001;
        double min = p_optimizer->runner()->model()->constraints().at(i)->min() - 0.001;


        if(val > max) val_input = val - max;

        else if(val < min) val_input = min - val;
        else
        {
            double u_slack = max - val;
            double l_slack = val - min;

            val_input = (u_slack > l_slack) ? -u_slack : -l_slack;
        }

        x.set_bb_output(i+1, val_input);
    }


    cout << "//// checking if this is the best solution ////" << endl;


    // checking if this is the best result so far
    if(isBest(result))
    {
        cout << "//    This is the best solution!    //" << endl;

        if(p_result_best != 0) delete p_result_best;

        p_result_best = new Case(*result, true);

        p_result_best->printToCout();
    }


    cout << "//// deleting case ////" << endl;


    // deleting the case from the heap
    delete c;



    cout << "//// Finished NOMAD evaluation ////" << endl;

    return true;


}


//-----------------------------------------------------------------------------------------------
// generates a case from an Eval_Point
//-----------------------------------------------------------------------------------------------
Case* NomadIpoptEvaluator::generateCase(const NOMAD::Eval_Point &x) const
{
    Case *c = new Case();

    int var_num = 0;


    for(int i = 0; i < p_optimizer->runner()->model()->binaryVariables().size(); ++i)
    {
        c->addBinaryVariableValue(x[var_num].value());
        ++var_num;
    }

    for(int i = 0; i < p_optimizer->runner()->model()->integerVariables().size(); ++i)
    {
        c->addIntegerVariableValue(x[var_num].value());
        ++var_num;
    }

    // checking if contineous vars should be copied
    if(p_result_best != 0)
    {
        cout << "//   updating cont. vars. from the best solution   //" << endl;

        for(int i = 0; i < p_result_best->numberOfRealVariables(); ++i)
        {
            cout << "Adding var #" << i << endl;
            c->addRealVariableValue(p_result_best->realVariableValue(i));
        }
    }




    return c;

}

//-----------------------------------------------------------------------------------------------
// compares c against a base case to se if it is better
//-----------------------------------------------------------------------------------------------
bool NomadIpoptEvaluator::isBest(Case *c)
{
    if(p_result_best == 0) return true;
    if(c->infeasibility() >  0.0001) return false;
    else return c->objectiveValue() > p_result_best->objectiveValue();
}

//-----------------------------------------------------------------------------------------------
// Finds a result case with int and bin var values in c
//-----------------------------------------------------------------------------------------------
Case* NomadIpoptEvaluator::findResult(Case *c)
{
    return p_eval->findResult(c);
}

/*
Case* NomadIpoptEvaluator::findResult(Case *c)
{
    for(int i = 0; i < m_results.size(); ++i)
    {
        Case *r = m_results.at(i);

        // checking the results case for match
        if(r->numberOfBinaryVariables() != c->numberOfBinaryVariables()) return 0;
        if(r->numberOfIntegerVariables() != c->numberOfIntegerVariables()) return 0;

        bool ok = true;
        for(int j = 0; j < c->numberOfBinaryVariables(); ++j)
        {
            if(c->binaryVariableValue(j) != r->binaryVariableValue(j))
            {
                ok = false;
                break;
            }

        }
        if(ok)
        {
            for(int j = 0; j < c->numberOfIntegerVariables(); ++j)
            {
                if(c->integerVariableValue(j) != r->integerVariableValue(j))
                {
                    ok = false;
                    break;
                }
            }
        }

        if(ok) return r;

    }

    return 0;


}

*/
//-----------------------------------------------------------------------------------------------
// solves the contineous sub-problem using IPOPT
//-----------------------------------------------------------------------------------------------
/*
Case* NomadIpoptEvaluator::solveContineousProblem(Case *discrete_vars)
{
    // ----- Initializing IPOPT ------- //

    // setting up the TNLP
    SmartPtr<NomadIpoptInterface> p_tnlp = new NomadIpoptInterface(p_optimizer, this, discrete_vars);

    // Create an instance of the IpoptApplication
    SmartPtr<IpoptApplication> app = IpoptApplicationFactory();

    // Change some options
    app->Options()->SetStringValue("output_file", p_optimizer->runner()->reservoirSimulator()->folder().toStdString() + "/ipopt.out");
    app->Options()->SetIntegerValue("max_iter", 100);
    app->Options()->SetNumericValue("tol", 0.01);
    app->Options()->SetNumericValue("dual_inf_tol", 5);
    app->Options()->SetNumericValue("constr_viol_tol", 0.001);
    app->Options()->SetNumericValue("compl_inf_tol", 0.01);

    //app->Options()->SetStringValue("derivative_test", "first-order");
    //app->Options()->SetStringValue("derivative_test_print_all", "yes");
    //app->Options()->SetNumericValue("derivative_test_perturbation", 0.001);


    app->Options()->SetStringValue("hessian_approximation", "limited-memory"); // exact (default, no approx) or limited-memory (quasi-Newton)

    // Initialize the IpoptApplication and process the options
    ApplicationReturnStatus status;
    app->Initialize();


    if (status != Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** Error during IPOPT initialization!" << std::endl;
        exit(1);
    }


    // ----- Starting IPOPT ------- //
    status = app->OptimizeTNLP(GetRawPtr(p_tnlp));

    if (status == Solve_Succeeded)
    {
        std::cout << std::endl << std::endl << "*** The contineous IPOPT sub-problem problem solved!" << std::endl;
    }
    else
    {
        std::cout << std::endl << std::endl << "*** The contineous IPOPT sub-problem problem FAILED!" << std::endl;
    }

    // checking if this is the best solution so far
    QVector<double> objs_current = p_tnlp->objectives();
    QVector<double> infeas_current = p_tnlp->infeasibilities();

    if(m_best_objs.size() == 0)
    {
        m_best_objs = objs_current;
        m_best_infeas = infeas_current;
    }

    else if(m_best_objs.last() > objs_current.last() && m_best_infeas.last() >= (infeas_current.last()-0.01))
    {
        cout << "---- current sub-problem has the highest obj ----" << endl;
        m_best_objs = objs_current;
        m_best_infeas = infeas_current;
    }

    // extracting the final solution from IPOPT
    return p_tnlp->bestCase();


}

*/

//-----------------------------------------------------------------------------------------------
// checks if the current contineous optimization should continue
//-----------------------------------------------------------------------------------------------
/*
bool NomadIpoptEvaluator::shouldContinue(int i, double obj, double infeas)
{
    cout << "checking objective for iteration #" << i << " against obj = " << obj << endl;
    cout << "number of best objs = " << m_best_objs.size() << endl;

    if(i < p_optimizer->terminationStart())
    {
        cout << "------------------------------------" << endl;
        cout << "dont check yet" << endl;
        cout << "------------------------------------" << endl;
        return true;
    }

    if(m_best_objs.size() == 0) return true;

    // if the current ipopt has gotten further than the current best
    if(i >= m_best_objs.size())
    {

        double obj_best;
        if(m_best_objs.last() > 0) obj_best = m_best_objs.last() / (p_optimizer->termination() + 0.0001);
        else obj_best = m_best_objs.last() * p_optimizer->termination();

        cout << "------------------------------------" << endl;
        cout << "current is further" << endl;
        cout << "obj    = " << obj << " cur = " << obj_best << endl;
        cout << "infeas = " << infeas << " cur = " << m_best_infeas.last() << endl;
        cout << "------------------------------------" << endl;


        bool ok_obj = obj_best > obj;
        bool ok_infeas = m_best_infeas.last() >= infeas;

        if(!ok_obj && !ok_infeas) return false;
        else return true;

    }

    else
    {


        double obj_best;
        if(m_best_objs.at(i) > 0) obj_best = m_best_objs.at(i) / (p_optimizer->termination() + 0.0001);
        else obj_best = m_best_objs.at(i) * p_optimizer->termination();

        cout << "------------------------------------" << endl;
        cout << "on same path" << endl;
        cout << "obj    = " << obj << " cur = " << obj_best << endl;
        cout << "infeas = " << infeas << " cur = " << m_best_infeas.at(i) << endl;
        cout << "------------------------------------" << endl;


        bool ok_obj = obj_best > obj;
        bool ok_infeas = m_best_infeas.at(i) >= infeas;

        if(!ok_obj && !ok_infeas) return false;
        else return true;


    }
}

*/

} // namespace ResOpt
