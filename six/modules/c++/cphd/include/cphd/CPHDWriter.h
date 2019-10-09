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

#ifndef __CPHD_CPHD_WRITER_H__
#define __CPHD_CPHD_WRITER_H__

#include <string>
#include <vector>

#include <types/RowCol.h>
#include <io/FileOutputStream.h>
#include <sys/OS.h>
#include <sys/Conf.h>
#include <cphd/Metadata.h>
#include <cphd/PVP.h>
#include <cphd/PVPBlock.h>

namespace cphd
{
/*
 *  \class CPHDWriter
 *  \brief Used to write a CPHD file. You must be able to provide the
 *         appropriate metadata and vector based metadata.
 */
class CPHDWriter
{
public:
    /*
     *  \func Constructor
     *  \brief Sets up the internal structure of the CPHDWriter
     *
     *  The default argument for numThreads should be sys::OS().getNumCPUs().
     *  However, SWIG doesn't seem to like that.
     *  As a workaround, we pass in 0 for the default, and the ctor sets the
     *  number of threads to the number of CPUs if this happens.
     *
     *  \param metadata A filled out metadata struct for the file that will be
     *         written. The data.arraySize and data.numCPHDChannels will be
     *         filled in internally. All other data must be provided.
     *  \param numThreads The number of threads to use for processing.
     *  \param scratchSpaceSize The maximum size of internal scratch space
     *         that may be used if byte swapping is necessary.
     *         Default is 4 MB
     */
    CPHDWriter(const Metadata& metadata,
               size_t numThreads = 0,
               size_t scratchSpaceSize = 4 * 1024 * 1024);

    /*
     *  \func addImage
     *  \brief Pushes a new image to the file for writing. This only works with
     *         valid CPHDWriter data types:
     *              std::complex<float>
     *              std::complex<sys::Int16_T>
     *              std::complex<sys::Int8_T>
     *
     *  \param image The image to be added. This should be sized to match the
     *         dims parameter.
     *  \param dims The dimensions of the image.
     *  \param vbmData The vector based metadata. This should have data
     *         equal to: dims.row * metadata.data.getNumBytesVBM.
     */
    template <typename T>
    void addImage(const T* image,
                  const types::RowCol<size_t>& dims,
                  const sys::ubyte* PVPData);

    /*
     *  \func writeMetadata
     *  \brief Writes the header, and metadata into the file. This should
     *         be used in situations where you need to write out the CPHD
     *         data in chunks. Otherwise you should use addImage and write.
     *         This will internally set the number of bytes per VBP.
     *
     *  \param pathname The desired pathname of the file.
     *  \param vbm The vector based metadata to write.
     *  \param classification The classification of the file. Optional
     *         By default, CPHD will not be populated with this value.
     *  \param releaseInfo The release information for the file. Optional
     *         By default, CPHD will not be populated with this value.
     */
    void writeMetadata(const std::string& pathname,
                       const PVPBlock& PVPBlock,
                       const std::string& classification = "",
                       const std::string& releaseInfo = "");

    /*
     *  \func writeSupportData
     *  \brief (Optional) Writes the specified support Array to the file
     * 
     *  \param data A pointer to the start of the support array that
     *        will be written to file
     *  \param id The unique identifier of the support array
     */
    template <typename T>
    void writeSupportData(const T* data,
                      std::string id)
    {
        // TODO: treat multiple params as seperate params or one byte string?
        writeSupportDataImpl(reinterpret_cast<const sys::ubyte*>(data), 
                             mMetadata.data.getSupportArrayById(id).numRows * mMetadata.data.getSupportArrayById(id).numCols,
                             mMetadata.data.getSupportArrayById(id).bytesPerElement);
    }

    /*
     *  \func writePVPData
     *  \brief Writes the PVP to the file
     * 
     *  \param PVPBlock A populated PVPBlock object that will be written
     *         to the file as a block of data
     */
    void writePVPData(const PVPBlock& PVPBlock);

    /*
     *  \func writeCPHDData
     *  \brief Writes a chunk of CPHD data to disk. To create a proper
     *         CPHD file you must call writeMetadata before using this
     *         method. If you do not need to write the data in chunks,
     *         you may instead use addImage and write. This only works with
     *         valid CPHDWriter data types:
     *              std::complex<float>
     *              std::complex<sys::Int16_T>
     *              std::complex<sys::Int8_T>
     *
     *  \param data The data to write to disk.
     *  \param numElements The number of elements in data. Treat the data
     *         as complex when computing the size (do not multiply by 2
     *         for correct byte swapping this is done internally).
     */
    template <typename T>
    void writeCPHDData(const T* data,
                       size_t numElements,
                       size_t channel = 0);

    /*
     * Same as write CPHDData but for compressed CPHD
     */
    void writeCompressedCPHDData(const sys::ubyte* data,
                       size_t numElements,
                       size_t channel = 0);

    /*
     *  \func write
     *  \brief Writes the CPHD file to disk. This should only be called
     *         if you are writing using addImage.
     *
     *  \param pathname The desired pathname of the file.
     *  \param classification The classification of the file. Optional
     *         By default, CPHD will not be populated with this value.
     *  \param releaseInfo The release information for the file. Optional
     *         By default, CPHD will not be populated with this value.
     */
    void write(const std::string& pathname,
               const std::string& classification = "",
               const std::string& releaseInfo = "");

    void close()
    {
        if (mFile.isOpen())
        {
            mFile.close();
        }
    }

private:
    void writeMetadata(size_t pvpSize,
                       size_t cphdSize,
                       size_t supportSize = 0, // Optional
                       const std::string& classification = "",
                       const std::string& releaseInfo = "");

    void writePVPData(const sys::ubyte* PVPBlock,
                      size_t index);

    void writeCPHDDataImpl(const sys::ubyte* data,
                           size_t size);

    void writeCompressedCPHDDataImpl(const sys::ubyte* data,
                           size_t size, size_t channel);

    void writeSupportDataImpl(const sys::ubyte* data,
                              size_t numElements, size_t elementSize);

    class DataWriter
    {
    public:
        DataWriter(io::FileOutputStream& stream,
                   size_t numThreads);

        virtual ~DataWriter();

        virtual void operator()(const sys::ubyte* data,
                                size_t numElements,
                                size_t elementSize) = 0;

    protected:
        io::FileOutputStream& mStream;
        const size_t mNumThreads;
    };

    class DataWriterLittleEndian : public DataWriter
    {
    public:
        DataWriterLittleEndian(io::FileOutputStream& stream,
                               size_t numThreads,
                               size_t scratchSize);

        virtual void operator()(const sys::ubyte* data,
                                size_t numElements,
                                size_t elementSize);

    private:
        const size_t mScratchSize;
        const mem::ScopedArray<sys::byte> mScratch;
    };

    class DataWriterBigEndian : public DataWriter
    {
    public:
        DataWriterBigEndian(io::FileOutputStream& stream,
                            size_t numThreads);

        virtual void operator()(const sys::ubyte* data,
                                size_t numElements,
                                size_t elementSize);
    };

    std::auto_ptr<DataWriter> mDataWriter;

    Metadata mMetadata;
    const size_t mElementSize;
    const size_t mScratchSpaceSize;
    const size_t mNumThreads;

    io::FileOutputStream mFile;

    std::vector<const sys::ubyte*> mCPHDData;
    std::vector<const sys::ubyte*> mPVPData;
    std::vector<std::pair<std::string, const sys::ubyte*> > mSupportData;

    size_t mCPHDSize;
    size_t mPVPSize;
};
}

#endif