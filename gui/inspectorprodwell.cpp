#include "inspectorprodwell.h"

#include "inspectorvariable.h"
#include "inspectorwellcontrol.h"
#include "inspectorconstraint.h"
#include "plotstreams.h"

#include "productionwell.h"
#include "intvariable.h"
#include "realvariable.h"
#include "wellcontrol.h"
#include "constraint.h"


#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>


using ResOpt::IntVariable;
using ResOpt::RealVariable;
using ResOpt::WellControl;
using ResOpt::Constraint;

namespace ResOptGui
{

InspectorProdWell::InspectorProdWell(ProductionWell *well, QWidget *parent) :
    QWidget(parent),
    p_well(well),
    m_btn_close("Close", this),
    m_btn_ok("Ok", this),
    m_btn_plot("Plot", this)
{

    setAttribute(Qt::WA_DeleteOnClose);


    construct();

    show();
}


//-----------------------------------------------------------------------------------------------
// constructs the view
//-----------------------------------------------------------------------------------------------
void InspectorProdWell::construct()
{
    setWindowTitle("Production Well " + p_well->name() + " Properties");

    QGridLayout *layout = new QGridLayout(this);

    setLayout(layout);

    // setting up the control variables
    QGroupBox *box_control = new QGroupBox("Control Variables", this);
    box_control->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QVBoxLayout *layout_control = new QVBoxLayout(box_control);
    box_control->setLayout(layout_control);


    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        WellControl *wc = p_well->control(i);
        InspectorWellControl *iwc = new InspectorWellControl(wc->endTime(), wc->controlVar()->value(), wc->controlVar()->max(), wc->controlVar()->min(), wc->type(), this, i == 0);
        m_controls.push_back(iwc);
        layout_control->addWidget(iwc);
    }


    layout->addWidget(box_control, 0, 0, 1, 3);

    // setting up the bhp constraints
    QGroupBox *box_bhp_con = new QGroupBox("BHP Constraints", this);
    box_bhp_con->setStyleSheet("QGroupBox{border:2px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 3px;}");

    QVBoxLayout *layout_bhp_con = new QVBoxLayout(box_bhp_con);
    box_bhp_con->setLayout(layout_bhp_con);


    for(int i = 0; i < p_well->numberOfBhpConstraints(); ++i)
    {


        InspectorConstraint *ic = new InspectorConstraint(p_well->control(i)->endTime(), p_well->bhpConstraint(i)->value(), p_well->bhpConstraint(i)->max(), p_well->bhpConstraint(i)->min(), this, i == 0);
        m_bhp_constraints.push_back(ic);
        layout_bhp_con->addWidget(ic);
    }


    layout->addWidget(box_bhp_con, 1, 0, 1, 3);



    // setting up the buttons
    layout->addWidget(&m_btn_ok, 2, 0);
    connect(&m_btn_ok, SIGNAL(clicked()), this, SLOT(saveAndClose()));

    layout->addWidget(&m_btn_plot, 2, 1);
    connect(&m_btn_plot, SIGNAL(clicked()), this, SLOT(openPlot()));

    layout->addWidget(&m_btn_close, 2, 2);
    connect(&m_btn_close, SIGNAL(clicked()), this, SLOT(close()));



}


//-----------------------------------------------------------------------------------------------
// Saves the current values to the model, and closes the window
//-----------------------------------------------------------------------------------------------
void InspectorProdWell::saveAndClose()
{

    emit sendMsg("Saving variable values for Production Well " + p_well->name() + " to model...");

    // saving control variables
    for(int i = 0; i < p_well->numberOfControls(); ++i)
    {
        p_well->control(i)->controlVar()->setValue(m_controls.at(i)->value());
        p_well->control(i)->controlVar()->setMax(m_controls.at(i)->max());
        p_well->control(i)->controlVar()->setMin(m_controls.at(i)->min());

        p_well->control(i)->setType(m_controls.at(i)->type());
    }

    close();
}

//-----------------------------------------------------------------------------------------------
// Opens the streams plot for the separator
//-----------------------------------------------------------------------------------------------
void InspectorProdWell::openPlot()
{
    QString title = "Plots for Production Well " + p_well->name();

    PlotStreams *plt = new PlotStreams(title, p_well->streams());
}



} // namespace
