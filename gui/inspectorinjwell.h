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


#ifndef INSPECTORINJWELL_H
#define INSPECTORINJWELL_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QtWidgets/QGroupBox>
#include <QScrollArea>

namespace ResOpt
{
class InjectionWell;
}

using ResOpt::InjectionWell;

namespace ResOptGui
{

class InspectorVariable;
class InspectorWellControl;
class InspectorWellConnectionVariable;
class InspectorWellPath;

class InspectorInjWell : public QWidget
{
    Q_OBJECT
private:
    InjectionWell *p_well;

    QVector<InspectorWellControl*> m_controls;
    QVector<InspectorWellConnectionVariable*> m_varcons;
    InspectorWellPath *p_inspector_wellpath;


    QWidget *widget;

    QGroupBox *box_control;
    QGroupBox *box_varcon;
    QGroupBox *box_wellpath;

    QPushButton *p_btn_control;
    QPushButton *p_btn_varcon;
    QPushButton *p_btn_wellpath;

    QPushButton m_btn_close;
    QPushButton m_btn_ok;
    QPushButton m_btn_plot;

    void construct();


public:
    explicit InspectorInjWell(InjectionWell *well, QWidget *parent = 0);

signals:
    void sendMsg(QString);

public slots:
    void saveAndClose();
    void openPlot();
    void hideControls(bool b);
    void hideConnectionVariables(bool b);
    void hideWellPath(bool b);

};

} // namespace

#endif // INSPECTORINJWELL_H
