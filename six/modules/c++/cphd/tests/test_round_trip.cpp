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

#include <iostream>
#include <fstream>
#include <memory>
#include <mem/BufferView.h>
#include <mem/ScopedArray.h>
#include <str/Convert.h>
#include <logging/NullLogger.h>
#include <cli/ArgumentParser.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <cphd/Metadata.h>
#include <cphd/PVPBlock.h>
#include <cphd/CPHDReader.h>
#include <cphd/CPHDWriter.h>

/*!
 * Reads in CPHD file from InputFile
 * Writes out CPHD file to OutputFile
 * Files should match
 */
void testRoundTrip(std::string inPathname, std::string outPathname, size_t numThreads, std::vector<std::string>& schemaPathnames)
{
    //! Open the CPHD file
    cphd::CPHDReader reader(inPathname, numThreads, schemaPathnames);
    std::cout << "Succesfully finished reading from CPHD: " << inPathname << "\n";

    // Read fileheader
    const cphd::FileHeader& header = reader.getFileHeader();

    // Read metadata
    const cphd::Metadata& metadata = reader.getMetadata();

    // Read SupportBlock
    const cphd::SupportBlock& supportBlock = reader.getSupportBlock();
    mem::ScopedArray<sys::ubyte> readPtr;
    supportBlock.readAll(numThreads, readPtr);

    // Read PVPBlock
    const cphd::PVPBlock& pvpBlock = reader.getPVPBlock();

    // Read Wideband
    const cphd::Wideband& wideband = reader.getWideband();

    const cphd::SignalArrayFormat signalFormat =
            metadata.data.getSampleType();

    // Create the writer
    cphd::CPHDWriter writer(reader.getMetadata(), outPathname, schemaPathnames, numThreads);

    // Declare and allocate the wideband data storage
    mem::ScopedArray<sys::ubyte> data;
    data.reset(new sys::ubyte[header.getSignalBlockSize()]);

    // Check if signal data is compressed
    if (metadata.data.isCompressed())
    {
        // If data is compressed
        for (size_t channel = 0, idx = 0; channel < metadata.data.getNumChannels(); ++channel)
        {
            const size_t bufSize = metadata.data.getCompressedSignalSize(channel);
            wideband.read(channel, mem::BufferView<sys::ubyte>(&data[idx], bufSize));
            idx += bufSize;
        }
        writer.write(
                pvpBlock,
                data.get(),
                readPtr.get());
    }
    else
    {
        // If data is not compressed
        for (size_t channel = 0, idx = 0; channel < metadata.data.getNumChannels(); ++channel)
        {
            const size_t bufSize = metadata.data.getSignalSize(channel);
            wideband.read(channel, 0, cphd::Wideband::ALL,
                 0, cphd::Wideband::ALL, numThreads,
                 mem::BufferView<sys::ubyte>(&data[idx], bufSize));
            idx += bufSize;
        }

        // Write full CPHD not compressed data
        switch (signalFormat)
        {
        case cphd::SignalArrayFormat::CI2:
            writer.write(
                    pvpBlock,
                    reinterpret_cast<const std::complex<sys::Int8_T>* >(data.get()),
                    readPtr.get());
            break;
        case cphd::SignalArrayFormat::CI4:
            writer.write(
                    pvpBlock,
                    reinterpret_cast<const std::complex<sys::Int16_T>* >(data.get()),
                    readPtr.get());
            break;
        case cphd::SignalArrayFormat::CF8:
            writer.write(
                    pvpBlock,
                    reinterpret_cast<const std::complex<float>* >(data.get()),
                    readPtr.get());
            break;
        }
    }

    std::cout << "Succesfully finished writing to CPHD: " << outPathname << "\n";
}

int main(int argc, char** argv)
{
    try
    {
        // Parse the command line
        cli::ArgumentParser parser;
        parser.setDescription("Round trip for a CPHD file.");
        parser.addArgument("-t --threads",
                           "Specify the number of threads to use",
                           cli::STORE,
                           "threads",
                           "NUM")->setDefault(sys::OS().getNumCPUs());
        parser.addArgument("input", "Input pathname", cli::STORE, "input",
                           "CPHD", 1, 1);
        parser.addArgument("output", "Output pathname", cli::STORE, "output",
                           "CPHD", 1, 1);
        parser.addArgument("schema", "Schema pathname", cli::STORE, "schema",
                           "XSD", 1, 1);
        const std::unique_ptr<cli::Results> options(parser.parse(argc, argv));
        const std::string inPathname(options->get<std::string>("input"));
        const std::string outPathname(options->get<std::string>("output"));
        const std::string schemaPathname(options->get<std::string>("schema"));
        const size_t numThreads(options->get<size_t>("threads"));

        std::vector<std::string> schemas;
        if (!schemaPathname.empty())
        {
            schemas.push_back(schemaPathname);
        }
        testRoundTrip(inPathname, outPathname, numThreads, schemas);
        return 0;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return 1;

}
