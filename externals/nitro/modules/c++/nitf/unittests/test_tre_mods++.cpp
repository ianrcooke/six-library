/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.hpp>
#include <iostream>
#include "TestCase.h"

namespace
{
TEST_CASE(setFields)
{
    // create an ACFTA TRE
    nitf::TRE tre("ACFTA");

    // set a field
    tre.setField("AC_MSN_ID", "fly-by");
    TEST_ASSERT_EQ(tre.getField("AC_MSN_ID").toString(),
                   std::string("fly-by    "));

    // re-set the field
    tre.setField("AC_MSN_ID", 1.2345678);
    TEST_ASSERT_EQ(tre.getField("AC_MSN_ID").toString(),
                   std::string("1.2345678 "));

    // try setting an invalid tag
    TEST_EXCEPTION(tre.setField("invalid-tag", "some data"));
}

TEST_CASE(setBinaryFields)
{
    nitf::TRE tre("RPFHDR");
    const int value = 123;
    tre.setField("LOCSEC", value);

    nitf::Field field = tre.getField("LOCSEC");
    const int readValue = *reinterpret_cast<int*>(field.getRawData());
    TEST_ASSERT_EQ(readValue, value);
}

TEST_CASE(cloneTRE)
{
    nitf::TRE tre("JITCID");
    tre.setField("FILCMT", "fyi");

    // try cloning the tre
    nitf::TRE dolly = tre.clone();

    // the two should NOT be equal -- underlying object is different
    TEST_ASSERT(tre != dolly);
    TEST_ASSERT_EQ(tre.getField("FILCMT").toString(), std::string("fyi"));
}

TEST_CASE(basicIteration)
{
    nitf::TRE tre("ACCPOB");

    // The entire TRE is one loop, and we haven't told it
    // how many elements there are, so there's nothing to
    // iterate over
    size_t numFields = 0;
    for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        ++numFields;
    }
    TEST_ASSERT_EQ(numFields, 1);

    numFields = 0;
    tre.setField("NUMACPO", 2, true);
    tre.setField("NUMPTS[0]", 3);
    tre.setField("NUMPTS[1]", 2);
    for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        ++numFields;
    }
    TEST_ASSERT_EQ(numFields, 29);
}

TEST_CASE(populateWhileIterating)
{
    nitf::TRE tre("ACCPOB");
    size_t numFields = 0;
    for (nitf::TRE::Iterator it = tre.begin(); it != tre.end(); ++it)
    {
        ++numFields;
        const std::string fieldName((*it).first());
        if (fieldName == "NUMACPO")
        {
            tre.setField("NUMACPO", 2);
        }
        else if (fieldName == "NUMPTS[0]")
        {
            tre.setField("NUMPTS[0]", 3);
        }
        else if (fieldName == "NUMPTS[1]")
        {
            tre.setField("NUMPTS[1]", 2);
        }
    }
    TEST_ASSERT_EQ(numFields, 29);
}

TEST_CASE(overflowingNumericFields)
{
    nitf::TRE tre("CSCRNA");

    // This field has a length of 9, so check that it's properly
    // truncated
    tre.setField("ULCNR_LAT", 1.0 / 9);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "0.1111111");

    tre.setField("ULCNR_LAT", 123456789);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "123456789");

    tre.setField("ULCNR_LAT", 12345678.);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "012345678");

    tre.setField("ULCNR_LAT", 12345678.9);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "012345678");

    tre.setField("ULCNR_LAT", 1);
    TEST_ASSERT_EQ(tre.getField("ULCNR_LAT").toString(), "000000001");

    // If we run out of digits before hitting the decimal, there's no
    // saving it
    TEST_EXCEPTION(tre.setField("ULCNR_LAT", 123456789012LL));
}
}

int main(int /*argc*/, char** /*argv*/)
{
    TEST_CHECK(setFields);
    TEST_CHECK(setBinaryFields);
    TEST_CHECK(cloneTRE);
    TEST_CHECK(basicIteration);
    TEST_CHECK(populateWhileIterating);
    TEST_CHECK(overflowingNumericFields);
    return 0;
}
