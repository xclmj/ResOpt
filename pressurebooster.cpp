#include "pressurebooster.h"

#include <iostream>
#include "cost.h"
#include "pipeconnection.h"
#include "intvariable.h"
#include "realvariable.h"
#include "constraint.h"
#include "stream.h"

using std::cout;
using std::endl;

namespace ResOpt
{

PressureBooster::PressureBooster()
    : p_cost(0),
      p_outlet_connection(0)
{
}

PressureBooster::PressureBooster(const PressureBooster &p)
    : Pipe(p)
{
    // copying base types

    // copying pointed objects
    p_cost = new Cost(*p.p_cost);
    p_outlet_connection = new PipeConnection(*p.p_outlet_connection);

    // copying variables
    p_install_time = shared_ptr<IntVariable>(new IntVariable(*p.p_install_time));
    p_pressure_change = shared_ptr<RealVariable>(new RealVariable(*p.p_pressure_change));
    p_capacity = shared_ptr<RealVariable>(new RealVariable(*p.p_capacity));

    // copying constraints
    for(int i = 0; i < p.m_capacity_constraints.size(); ++i)
    {
        m_capacity_constraints.push_back(shared_ptr<Constraint>(new Constraint(*p.m_capacity_constraints.at(i))));
    }




}


PressureBooster::~PressureBooster()
{
    if(p_cost != 0) delete p_cost;
    if(p_outlet_connection != 0) delete p_outlet_connection;
}


//-----------------------------------------------------------------------------------------------
// sets up the capacity constraints
//-----------------------------------------------------------------------------------------------
void PressureBooster::setupCapacityConstraints(const QVector<double> &master_schedule)
{

    // clearing the vector
    m_capacity_constraints.clear();

    // setting up the constraints

    cout << "Adding capacity constraints for BOOSTER #" << number() << " ..." << endl;

    for(int i = 0; i < master_schedule.size(); ++i)
    {
        shared_ptr<Constraint> c(new Constraint(0.5, 1.0, 0.0));

        c->setName("Capacity constraint for BOOSTER #" + QString::number(number()) + " for time = " + QString::number(master_schedule.at(i)));


        m_capacity_constraints.push_back(c);

    }


}

//-----------------------------------------------------------------------------------------------
// updates the values of the capacity constraints
//-----------------------------------------------------------------------------------------------
void PressureBooster::updateCapacityConstraints()
{
    // checking that the number of streams correspond to the number of constraints
    if(numberOfStreams() != m_capacity_constraints.size())
    {

        cout << endl << "### Runtime Error ###" << endl
             << "Number of streams and number of capacity constraints do not match for BOOSTER #" << number() << "..." << endl
             << "STREAMS     : " << numberOfStreams() << endl
             << "CONSTRAINTS : " << m_capacity_constraints.size() << endl << endl;

        exit(1);

    }

    // starting to update the constraint values
    for(int i = 0; i < numberOfStreams(); ++i)
    {
        // calculating a total rate for the time step
        double q_tot = stream(i)->oilRate(true) + stream(i)->waterRate(true) + stream(i)->gasRate(true);

        // calculating the constraint value
        double c = (p_capacity->value() - q_tot) / p_capacity->value();
        if(c > 1.0) c = 1.0;

        // updating the constraint value

        if(p_install_time->value() > i) m_capacity_constraints.at(i)->setValue(0.5);
        else m_capacity_constraints.at(i)->setValue(c);
    }


}

//-----------------------------------------------------------------------------------------------
// calculates the inlet pressure of the separator
//-----------------------------------------------------------------------------------------------
void PressureBooster::calculateInletPressure()
{

    // checking if the outlet connection is defined
    if(p_outlet_connection == 0)
    {
        cout << endl << "### Runtime Error ###" << endl
             << "NO Outlet pipe set for BOOSTER: " << number() << endl << endl;

        exit(1);
    }

    // checking if outlet pipe have the same number of streams

    if(numberOfStreams() != outletConnection()->pipe()->numberOfStreams())
    {

        cout << endl << "### Runtime Error ###" << endl
             << "Booster and upstream pipe do not have the same number of time steps..." << endl
             << "BOOSTER      : " << number() << ", n = " << numberOfStreams() << endl
             << "Upstream PIPE: " << outletConnection()->pipe()->number() << ", n = " << outletConnection()->pipe()->numberOfStreams() << endl << endl;

        exit(1);

    }


    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        // getting the outlet pressure
        double p_out = outletConnection()->pipe()->stream(i)->pressure(stream(i)->inputUnits());

        // if the booster is installed, the inlet pressure should be outlet - pressure_change
        // else the inlet pressure should be equal to the outlet pressure
        double p_in = p_out;
        if(p_install_time->value() <= i)
        {
            p_in -= p_pressure_change->value();
            if(p_in < 0.1)
            {
                cout << endl << "### Warning ###" << endl;
                cout << "The downstream pressure for booster #" << number() << " is less than the boosting pressure..." << endl;
                cout << "Setting the upstream pressure to 0" << endl;

                p_in = 0.1;
            }
        }

        // setting inlet pressure
        stream(i)->setPressure(p_in);
    }


}


//-----------------------------------------------------------------------------------------------
// generates a description for driver file
//-----------------------------------------------------------------------------------------------
QString PressureBooster::description() const
{
    QString str("START BOOSTER\n");
    str.append(" NUMBER " + QString::number(number()) + "\n");
    if(p_install_time != 0)
    {
        str.append(" INSTALLTIME " + QString::number(p_install_time->value()) + " " + QString::number(p_install_time->max()) + " " + QString::number(p_install_time->min()) + "\n");
    }

    str.append(" COST " + QString::number(p_cost->constantCost()) + " " + QString::number(p_cost->fractionCost())  + "\n");
    str.append(" OUTLETPIPE " + QString::number(p_outlet_connection->pipeNumber()) + "\n");
    str.append(" PRESSUREBOOST " + QString::number(p_pressure_change->value()) + " " + QString::number(p_pressure_change->max()) + " " + QString::number(p_pressure_change->min()) + "\n");
    str.append(" CAPACITY " + QString::number(p_capacity->value()) + " " + QString::number(p_capacity->max()) + " " + QString::number(p_capacity->min()) + "\n");

    str.append("END BOOSTER\n\n");
    return str;

}



} // namespace ResOpt
