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

#include "well.h"

#include <iostream>

#include "wellconnection.h"
#include "stream.h"
#include "pipe.h"
#include "constraint.h"


using std::cout;
using std::endl;

namespace ResOpt
{


Well::Well()
    : m_bhp_inj(WellControl::QWAT)

{


}

//-----------------------------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------------------------
Well::Well(const Well &w)
{

    // copying basic types
    m_type = w.m_type;
    m_name = w.m_name;
    m_group = w.m_group;
    m_bhp_limit = w.m_bhp_limit;
    m_bhp_inj = w.m_bhp_inj;

    // copying connections
    for(int i = 0; i < w.numberOfConnections(); i++)
    {
        m_connections.push_back(new WellConnection(*w.m_connections.at(i)));
    }

    // copying controls
    for(int i = 0; i < w.numberOfControls(); i++)
    {
        m_schedule.push_back(new WellControl(*w.m_schedule.at(i)));
    }

    // copying streams
    for(int i = 0; i < w.numberOfStreams(); i++)
    {
        m_streams.push_back(new Stream(*w.m_streams.at(i)));
    }
}


Well::~Well()
{
    for(int i = 0; i < m_connections.size(); i++)
    {
        delete m_connections.at(i);
    }

    for(int i = 0; i < m_schedule.size(); i++)
    {
        delete m_schedule.at(i);
    }

    for(int i = 0; i < m_streams.size(); i++)
    {
        delete m_streams.at(i);
    }


}

//-----------------------------------------------------------------------------------------------
// deletes all the streams
//-----------------------------------------------------------------------------------------------
void Well::deleteStreams()
{
    // deleting the objects
    for(int i = 0; i < m_streams.size(); i++) delete m_streams.at(i);

    // dereferencing
    m_streams.clear();
}


//-----------------------------------------------------------------------------------------------
// adds a well control to the schedule
//-----------------------------------------------------------------------------------------------
void Well::addControl(WellControl *c)
{
    m_schedule.push_back(c);
}

} // namespace ResOpt
