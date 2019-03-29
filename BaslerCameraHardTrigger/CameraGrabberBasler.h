#ifndef CAMERAGRABBERBASLER_H
#define CAMERAGRABBERBASLER_H

#include <iostream>

//Basler Pylon include
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <pylon/PylonUtilityIncludes.h>

// opencv include
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>



enum FeatureType{
    CINTEGER,
    CFLOAT,
    CSTRING,
    CENUMERATION
};


///
/// \brief The CameraGrabberBasler class : This class used for grabbing image from Basler camera
///
class CameraGrabberBasler{
    friend class BaslerImageEventHandler;

public:

    CameraGrabberBasler(std::string camConfigFile , bool showView = false);
    ~CameraGrabberBasler();

    void Destroy();

    /// @brief major initialization routine
    int Init();

    /// @brief starts grabbing of images
    void StartGrab();

    /// @brief stops grabbing of images
    void StopGrab();

    /// @brief captures a single image
    void Snap();

    /// @brief will print status of grabber
    void PrintStatus(std::string& status);

    inline void SetSaveImgParams(std::string saveImgDir,
                          std::string imgPrefix,
                          std::string imgPostfix)
    {
        saveImgDir_ = saveImgDir;
        imgPrefix_ = imgPrefix;
        imgPostfix_ = imgPostfix;
    }


    void SaveFeaturesToFile();

    void LoadFeaturesFromFile();


protected:



    //Pylon Device and Pylon Camera objects
    Pylon::IPylonDevice * pDevice_ = NULL;
    Pylon::CBaslerUsbInstantCamera * camera_ = NULL;

    // the camera configuration file
    std::string camConfigFile_;
    bool showView_;

    void FindAndConnectCamera();
    void GetFeatureWithValueAsString(FeatureType featureType , const GenICam::gcstring &featureName , std::string & featureAndValue);


    std::string saveImgDir_;
    std::string imgPrefix_;
    std::string imgPostfix_;


    void SaveImages(const cv::Mat & frame , double timeStamp);
};


///
/// \brief The BaslerImageEventHandler class : This class is used as image grabbed event, derived from Pylon SDK
///
///
class BaslerImageEventHandler : public Pylon::CBaslerUsbImageEventHandler{
public:

    BaslerImageEventHandler(CameraGrabberBasler * cameraGrabber);

    virtual void OnImageGrabbed(Pylon::CBaslerUsbInstantCamera & camera , const Pylon::CBaslerUsbGrabResultPtr & ptrGrabResult);
protected:
    CameraGrabberBasler * cameraGrabber_;

};





#endif
