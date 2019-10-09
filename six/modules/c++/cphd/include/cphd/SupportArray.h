/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CPHD_SUPPORT_ARRAY_H__
#define __CPHD_SUPPORT_ARRAY_H__

#include <ostream>
#include <vector>
#include <map>
#include <stddef.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>


namespace cphd
{

/*!
 * (Optional) Parameters that describe the binary support
 * array(s) content and grid coordinates.
 */
struct SupportArrayParameter
{
    //! Constructor
    SupportArrayParameter();

    //! Custom constructor
    SupportArrayParameter(std::string, size_t, double, double, double, double);

    //! Equality operator
    bool operator==(const SupportArrayParameter& other) const
    {
        return identifier == other.identifier &&
                elementFormat == other.elementFormat &&
                x0 == other.x0 && y0 == other.y0 &&
                xSS == other.xSS && ySS == other.ySS;
    }
    bool operator!=(const SupportArrayParameter& other) const
    {
        return !((*this) == other);
    }

    //! Set unique identifier
    void setIdentifier(size_t identifierIn)
    {
        identifier = identifierIn;
    }

    //! Get unique identifier
    inline size_t getIdentifier() const
    {
        return identifier;
    }

    //! Data element format
    std::string elementFormat;

    //! Row 0 X coordinate
    double x0;

    //! Column 0 Y coordinate.
    double y0;

    //! Row coordinate (X) sample spacing
    double xSS;

    //! Column coordinate (Y) sample spacing. 
    double ySS;

protected:
    void initializeParams();
private:
    size_t identifier;
};

/*!
 *Added Array: Z(m,n) is a two dimensional array
 * of data elements. Content and format of each
 * element is user defined. Array coordinates X(m)
 * and Y(m) are also user defined.
 */
struct AdditionalSupportArray : SupportArrayParameter
{
    //! Constructor
    AdditionalSupportArray();

    //! Custom constructor
    AdditionalSupportArray(std::string, std::string,
                           double, double, double, double,
                           std::string, std::string,
                           std::string);

    //! Equality operator
    bool operator==(const AdditionalSupportArray& other) const
    {
        return elementFormat == other.elementFormat &&
                x0 == other.x0 && y0 == other.y0 &&
                xSS == other.xSS && ySS == other.ySS &&
                identifier == other.identifier &&
                xUnits == other.xUnits && yUnits == other.yUnits &&
                zUnits == other.zUnits && parameter == other.parameter;
    }
    bool operator!=(const AdditionalSupportArray& other) const
    {
        return !((*this) == other);
    }

    //! Unique identifier of support array
    std::string identifier;

    //! Defines the X units of the sampled grid
    std::string xUnits;

    //! Defines the Y units of the sampled grid
    std::string yUnits;

    //! Defines the units of each component of the Data
    //! Element
    std::string zUnits;

    //! (Optional) Text field that can be used to further
    //! describe the added Support Array
    six::ParameterCollection parameter;
};

/*
 * (Optional) Parameters that describe the binary support
 * array(s) content and grid coordinates.
 * See section 2.3
 */
struct SupportArray
{
    //! Equality operators
    bool operator==(const SupportArray& other) const
    {
        return iazArray == other.iazArray &&
                antGainPhase == other.antGainPhase &&
                addedSupportArray == other.addedSupportArray;
    }
    bool operator!=(const SupportArray& other) const
    {
        return !((*this) == other);
    }

    //! Get IAZ support array by unique id
    SupportArrayParameter getIAZSupportArray(const std::string key) const
    {
        size_t keyNum = str::toType<size_t>(key);
        if (iazArray.size() <= keyNum)
        {
            std::ostringstream oss;
            oss << "SA_ID was not found " << (key);
            throw except::Exception(Ctxt(oss.str()));
        }
        return iazArray[keyNum];
    }

    //! Get AGP support array by unique id
    SupportArrayParameter getAGPSupportArray(const std::string key) const
    {
        size_t keyNum = str::toType<size_t>(key);
        if (antGainPhase.size() <= keyNum)
        {
            std::ostringstream oss;
            oss << "SA_ID was not found " << (key);
            throw except::Exception(Ctxt(oss.str()));
        }
        return antGainPhase[keyNum];
    }

    //! Get AGP support array by unique id
    AdditionalSupportArray getAddedSupportArray(const std::string key) const
    {
        if (addedSupportArray.count(key) == 0 || addedSupportArray.count(key) > 1)
        {
            std::ostringstream oss;
            oss << "SA_ID was not found " << (key);
            throw except::Exception(Ctxt(oss.str()));
        }
        return addedSupportArray.find(key)->second;
    }

    //! Vector of IAZ type arrays
    std::vector<SupportArrayParameter> iazArray;

    //! Vector of AGP type arrays
    std::vector<SupportArrayParameter> antGainPhase;

    //! Map of additonally defined support arrays
    std::map<std::string, AdditionalSupportArray> addedSupportArray;
};

// Ostream operators
std::ostream& operator<< (std::ostream& os, const SupportArrayParameter& s);
std::ostream& operator<< (std::ostream& os, const AdditionalSupportArray& a);
std::ostream& operator<< (std::ostream& os, const SupportArray& s);
}

#endif