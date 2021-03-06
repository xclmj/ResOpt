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


#ifndef STREAM_H
#define STREAM_H

#include <QVector>


namespace ResOpt
{


/**
 * @brief Container for rates for a given time step
 *
 */
class Stream
{
public:
    enum units{METRIC, FIELD};

private:
    double m_time;
    double m_oil_rate;
    double m_water_rate;
    double m_gas_rate;
    double m_pressure;

    units m_input_units;

public:
/**
 * @brief
 *
 */
    Stream();
    Stream(double t, double qo, double qg, double qw, double p);
    Stream(const Stream &s);

    // misc functions

    void printToCout() const;


    /**
     * @brief Sets the values of this stream to the average values of the input vector
     *
     * @param input
     */
    void avg(const QVector<Stream*> &input, double t_start);

    // set functions

    /**
     * @brief Sets the time for the time step
     *
     * @param t time in days
     */
    void setTime(double t) {m_time = t;}

    /**
     * @brief Sets the oil rate for the time step
     *
     * @param q rate in bbl/d
     */
    void setOilRate(double q) {m_oil_rate = q;}

    /**
     * @brief Sets the water rate for the time step
     *
     * @param q rate in bbl/d
     */
    void setWaterRate(double q) {m_water_rate = q;}

    /**
     * @brief Sets the gas rate for the time step
     *
     * @param q rate in mcf/d
     */
    void setGasRate(double q) {m_gas_rate = q;}


    /**
     * @brief Sets the pressure
     *
     * @param p
     */
    void setPressure(double p) {m_pressure = p;}

    void setInputUnits(Stream::units u) {m_input_units = u;}

    // get functions

    /**
     * @brief Returns the time of the time step
     *
     * @return time in days
     */
    double time() const {return m_time;}

    /**
     * @brief Returns the oil rate for the time step
     *
     * @return oil rate in bbl/d
     */
    double oilRate(Stream::units u) const;
    double oilRate(bool input_units) const;

    /**
     * @brief Returns the water rate for the time step
     *
     * @return water rate in bbl/d
     */
    double waterRate(Stream::units u) const;
    double waterRate(bool input_units) const;

    /**
     * @brief Returns the gas rate for the time step
     *
     * @return gas rate in mcf/d
     */
    double gasRate(Stream::units u) const;
    double gasRate(bool input_units) const;


    /**
     * @brief Returns the pressure
     *
     * @return double
     */
    double pressure(Stream::units u) const;
    double pressure(bool input_units) const;

    Stream::units inputUnits() const {return m_input_units;}


    // overloaded operators

    Stream& operator=(const Stream &rhs);
    Stream& operator+=(const Stream &rhs);
    const Stream operator+(const Stream &rhs) const;
    const Stream operator*(const double &rhs) const;

};

} // namespace ResOpt

#endif // STREAM_H
