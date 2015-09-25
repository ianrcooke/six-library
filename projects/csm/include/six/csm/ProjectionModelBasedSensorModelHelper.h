#ifndef __SIX_CSM_PROJECTION_MODEL_BASED_SENSOR_MODEL_HELPER_H__
#define __SIX_CSM_PROJECTION_MODEL_BASED_SENSOR_MODEL_HELPER_H__

#include <mem/SharedPtr.h>
#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/csm/SensorModelHelper.h>

namespace six
{
namespace CSM
{
class ProjectionModelBasedSensorModelHelper : public SensorModelHelper
{
public:
    ProjectionModelBasedSensorModelHelper(
            const scene::SceneGeometry& geometry,
            mem::SharedPtr<scene::ProjectionModel> projection);

    virtual
    std::vector<double>
    computeGroundPartials(const csm::EcefCoord& groundPt) const;

    virtual int getNumParameters() const;

    virtual double getParameterValue(int index) const;

    virtual void setParameterValue(int index, double value);

    virtual double getParameterCovariance(int index1, int index2) const;

    virtual void setParameterCovariance(int index1,
                                        int index2,
                                        double covariance);

    virtual std::vector<double> getUnmodeledError(
            const csm::ImageCoord& imagePt) const;

    virtual csm::RasterGM::SensorPartials computeSensorPartials(
            int index,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::RasterGM::SensorPartials computeSensorPartials(
            int index,
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual
    std::vector<csm::RasterGM::SensorPartials> computeAllSensorPartials(
            const csm::EcefCoord& groundPt,
            csm::param::Set pSet,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual
    std::vector<csm::RasterGM::SensorPartials> computeAllSensorPartials(
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            csm::param::Set pSet,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::ImageCoord groundToImage(
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* ) const;

    virtual csm::EcefCoord imageToGround(
            const csm::ImageCoord& imagePt,
            double height,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual double getImageTime(const csm::ImageCoord& imagePt) const;

    virtual
    csm::EcefCoord getSensorPosition(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefCoord getSensorPosition(double time) const;

    virtual csm::EcefVector
    getSensorVelocity(const csm::ImageCoord& imagePt) const;

    virtual csm::EcefVector getSensorVelocity(double time) const;

    virtual double getCorrelationCoefficient(size_t cpGroupIndex,
                                             double deltaTime) const;

private:
    csm::ImageCoord groundToImageImpl(const csm::EcefCoord& groundPt,
                                      double desiredPrecision,
                                      double* achievedPrecision) const;

private:
    const mem::SharedPtr<scene::ProjectionModel> mProjection;

    // NOTE: This is computed just at the SCP once each time a new SICD/SIDD is
    //       loaded rather than being recomputed for each ground point (which
    //       the scene module does support).  This is due to avoiding a
    //       mismatch between what the imageToGround() / groundToImage()
    //       overloadings that compute covariance use (where you do have a
    //       point)  and getParameterCovariance() / setParameterCovariance()
    //       where you do not.  Given the current CSM API, the only other
    //       solution would be to increase the number of adjustable parameters
    //       and the covariance matrix size to 9 and not fold in the
    //       tropo/iono error covariance into the ephemeric error covariance.
    math::linear::MatrixMxN<7, 7> mSensorCovariance;
};
}
}

#endif