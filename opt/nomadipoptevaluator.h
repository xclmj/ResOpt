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


#ifndef NOMADIPOPTEVALUATOR_H
#define NOMADIPOPTEVALUATOR_H

#include "nomad.hpp"
#include <QList>
#include <QVector>

namespace ResOpt
{
class NomadIpoptOptimizer;
class Case;
class MINLPEvaluator;

class NomadIpoptEvaluator : public NOMAD::Evaluator
{
private:
    NomadIpoptOptimizer *p_optimizer;
    MINLPEvaluator *p_eval;

    Case *p_result_best;

    //QList<Case*> m_results;

    //QVector<double> m_best_objs;
    //QVector<double> m_best_infeas;

    //Case* solveContineousProblem(Case *discrete_vars);

    bool isBest(Case *c);



public:
    NomadIpoptEvaluator(const NOMAD::Parameters &p, NomadIpoptOptimizer *o);
    ~NomadIpoptEvaluator();

    bool eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval);

    Case* generateCase(const NOMAD::Eval_Point &x) const;
    Case* findResult(Case *c);

    //bool shouldContinue(int i, double obj, double infeas);
};

} // namespace ResOpt

#endif // NOMADIPOPTEVALUATOR_H
