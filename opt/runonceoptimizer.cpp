/*
 * This file is part of the ResOpt project.
 *
 * Copyright (C) 2011-2012 Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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


#include "runonceoptimizer.h"

#include <iostream>
#include "runner.h"
#include "case.h"
#include "casequeue.h"

#include "model.h"
#include "realvariable.h"
#include "binaryvariable.h"
#include "constraint.h"
#include "objective.h"
#include "pipe.h"
#include <QVector>

#include <tr1/memory>



using std::tr1::shared_ptr;



using std::cout;
using std::endl;

namespace ResOpt
{


RunonceOptimizer::RunonceOptimizer(Runner *r)
    : Optimizer(r)
{
}

RunonceOptimizer::~RunonceOptimizer()
{
    // nothing to do
}


//-----------------------------------------------------------------------------------------------
// Initializes the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::initialize()
{
    // nothing to do here

    cout << "Initializing RUN_ONCE..." << endl;
}


//-----------------------------------------------------------------------------------------------
// Starts the optimizer
//-----------------------------------------------------------------------------------------------
void RunonceOptimizer::start()
{
    cout << "Starting RUN_ONCE..." << endl;

    // creating a case for the current values in the model
    Case *c = new Case(runner()->model());

    Case *c2 = new Case(*c);
    Case *c3 = new Case(*c);

    if(c2->numberOfRealVariables() > 14)
    {
        for(int i = 9; i <= 14; ++i)
        {
            c2->setRealVariableValue(i, c2->realVariableValue(i)-20);
            c3->setRealVariableValue(i, c3->realVariableValue(i)+20);
        }
    }


    CaseQueue *cq = new CaseQueue();

    cq->push_back(c);
    cq->push_back(c2);
    cq->push_back(c3);

    runCases(cq);

    // launching the runner on the case
    //runCase(c);




    delete c;
    delete c2;




    // letting the runner know that the optimization has finished
    emit finished();

    cout << "Optimization finished..." << endl;
}


//-----------------------------------------------------------------------------------------------
// builds a case for a pipe
//-----------------------------------------------------------------------------------------------
Case* RunonceOptimizer::buildCase(Pipe *p)
{
    Case *c = new Case();

    c->addRealVariableValue(0);
    c->addRealVariableValue(100);
    c->addRealVariableValue(0);
    c->addRealVariableValue(200);

    return c;
}


//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString RunonceOptimizer::description() const
{
    QString str("START OPTIMIZER\n");
    str.append(" TYPE RUNONCE \n");
    str.append(" ITERATIONS " + QString::number(maxIterations()) + "\n");
    str.append(" PERTURBATION " + QString::number(pertrurbationSize()) + "\n");
    str.append(" PARALLELRUNS " + QString::number(parallelRuns()) + "\n");
    str.append("END OPTIMIZER\n\n");
    return str;
}





} // namespace ResOpt
