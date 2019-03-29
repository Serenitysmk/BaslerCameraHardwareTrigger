#include "CameraGrabberBasler.h"
#include <string>
#include <vector>
#include <sys/time.h>



/////////////////////////////////Event Handler////////////////////////////////

BaslerImageEventHandler::BaslerImageEventHandler(CameraGrabberBasler *cameraGrabber){
    cameraGrabber_ = cameraGrabber;
}


///
/// \brief BaslerImageEventHandler::OnImageGrabbed : function called when the image grabbed event happened
/// \param camera
/// \param ptrGrabResult : a pointer used to save grabbed image buffer
///
void BaslerImageEventHandler::OnImageGrabbed(Pylon::CBaslerUsbInstantCamera &camera,
                                             const Pylon::CBaslerUsbGrabResultPtr &ptrGrabResult)
{


    int imgHeight = ptrGrabResult->GetHeight();
    int imgWidth = ptrGrabResult->GetWidth();

    std::cout << "[INFO] Image Grabbed Result Report : " << std::endl;
    std::cout << "image height = " << imgHeight << " , image width = " << imgWidth << std::endl;

    // display image in an opencv window
    cv::Size imgSize(imgWidth , imgHeight);
    cv::Mat frame(imgSize , CV_8UC1 , (uchar *)ptrGrabResult->GetBuffer());
    if(cameraGrabber_->showView_){

        cv::imshow("frame" , frame);
        cv::waitKey(1);
    }

    // save image to file

    // generate timeStamp
    ///
    /// \brief timeStamp : timestamp stands for the exactly system time when you call the function gettimeofday()
    ///tv_sec : the system time when you call the function and get the value (how many seconds)
    ///tv_usec : the microseconds

    double timeStamp = 0.0;
    timeval time;
    gettimeofday(&time , NULL);
    timeStamp = (double)time.tv_sec + ((double)time.tv_usec / (1000000.0));

    cameraGrabber_->SaveImages(frame , timeStamp);

}





////////////////////////////////////////////////Pylon Helper function//////////////////////////////////////


void CameraGrabberBasler::FindAndConnectCamera(){
    try{

        Pylon::CDeviceInfo info;
        info.SetDeviceClass(Pylon::CBaslerUsbInstantCamera::DeviceClass());
        pDevice_ = Pylon::CTlFactory::GetInstance().CreateFirstDevice(info);
        camera_ = new Pylon::CBaslerUsbInstantCamera(pDevice_);


    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
    }
}




///
/// \brief CameraGrabberBasler::CameraGrabberBasler : CameraGrabbrBasler Constructor
/// \param timeStampHandler
/// \param camConfigFile
/// \param imageQueue
/// \param showView
///
CameraGrabberBasler::CameraGrabberBasler(std::string camConfigFile, bool showView)
{

    camConfigFile_ = camConfigFile;
    showView_ = showView;

    std::cerr << "Basler Object created" << std::endl;
}

CameraGrabberBasler::~CameraGrabberBasler(){
    Destroy();
}


///
/// \brief CameraGrabberBasler::Destroy: Close and destroy the camera
///
void CameraGrabberBasler::Destroy(){
    std::cerr << "[INFO] Basler Camera Destroyed" << std::endl;

    // close the camera
    try{
        camera_->Close();

    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
    }

    Pylon::PylonTerminate();

}



///
/// \brief CameraGrabberBasler::Camera initialization
/// \return
///
int CameraGrabberBasler::Init(){
    std::cerr << "Basler Camera Initialization called" << std::endl;

    // Pylon Initialization : must be called before any pylon functions
    Pylon::PylonInitialize();
    try{

        // Find and connnect a camera
        FindAndConnectCamera();

        std::cout << "[INFO] Using camera : " << camera_->GetDeviceInfo().GetModelName() << std::endl;
        // Register some event handler
        camera_->RegisterImageEventHandler(new BaslerImageEventHandler(this) , Pylon::RegistrationMode_Append , Pylon::Cleanup_Delete);

        /// Open the camera , so that you could set camera parameter
        camera_->Open();

        /// set hardware trigger parameter
        /// here we used a Arduino microcontroller to generate hardware trigger signal
        camera_->AcquisitionMode.SetValue(Basler_UsbCameraParams::AcquisitionMode_Continuous);
        camera_->TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
        camera_->TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
        camera_->AcquisitionFrameRateEnable.SetValue(false);
        camera_->TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line1);
        camera_->ExposureMode.SetValue(Basler_UsbCameraParams::ExposureMode_TriggerWidth);


    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured :" << e.GetDescription() << std::endl;
    }



}

void CameraGrabberBasler::StartGrab(){
    // Start Grabbing image
    // Grabbing is controlled by a microcontroller
    // all the framerate setting is determined by the microcontroller

    std::cerr << "************[INFO] Start Grabbing Images************" << std::endl;
    try{

        camera_->AcquisitionStart.Execute();
        camera_->StartGrabbing(Pylon::GrabStrategy_OneByOne , Pylon::GrabLoop_ProvidedByInstantCamera);

    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured :" << e.GetDescription() << std::endl;
    }


}


void CameraGrabberBasler::StopGrab(){
    std::cerr << "************[INFO] Grabbing images end***********" << std::endl;
    try{
        camera_->AcquisitionStop.Execute();
    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
    }

}


void CameraGrabberBasler::Snap(){

    std::cout << "not implemented... , please implement me " << std::endl;

}

void CameraGrabberBasler::PrintStatus(std::string &status){
    /// This function printout the camera features
    status = "";
    std::stringstream ss;

    // There will be some interesting things to get from the camera
    // but the features should be accessed by its parameter type
    // there is some common feature type often used:
    //CInteger , CString , CFloat , CEnumeration

    std::vector<GenICam::gcstring> featuresInteger;
    std::vector<GenICam::gcstring> featuresString;
    std::vector<GenICam::gcstring> featuresFloat;
    std::vector<GenICam::gcstring> featuresEnumeration;

    //The String parameters
    featuresString.push_back("DeviceVendorName");
    featuresString.push_back("DeviceModelName");

    // The float parameters
    featuresFloat.push_back("DeviceTemperature");
    featuresFloat.push_back("ExposureTime");
    featuresFloat.push_back("AcquisitionFrameRate");
    featuresFloat.push_back("Gain");


    // some features about the Trigger information
    featuresEnumeration.push_back("TriggerMode");
    featuresEnumeration.push_back("TriggerSelector");
    featuresEnumeration.push_back("TriggerSource");


    // some features about the image
    featuresInteger.push_back("Width");
    featuresInteger.push_back("Height");
    featuresInteger.push_back("OffsetX");
    featuresInteger.push_back("OffsetY");

    // feature for string type
    std::string featureAndValue = "";

    int featureTotalIndex = 0;
    for(size_t featureIndex = 0 ; featureIndex < featuresString.size(); ++featureIndex){

        GetFeatureWithValueAsString(CSTRING , featuresString[featureIndex] , featureAndValue);
        ss << featureTotalIndex + 1 << " " << featureAndValue << std::endl;
        featureTotalIndex++;
    }

    for(size_t featureIndex = 0 ; featureIndex < featuresFloat.size(); ++featureIndex){
        GetFeatureWithValueAsString(CFLOAT , featuresFloat[featureIndex] , featureAndValue);
        ss << featureTotalIndex + 1 << " " << featureAndValue << std::endl;
        featureTotalIndex++;
    }

    for(size_t featureIndex = 0 ; featureIndex < featuresEnumeration.size(); ++featureIndex){
        GetFeatureWithValueAsString(CENUMERATION , featuresEnumeration[featureIndex] , featureAndValue);
        ss << featureTotalIndex + 1 << " " << featureAndValue << std::endl;
        featureTotalIndex++;
    }

    for(size_t featureIndex = 0 ; featureIndex < featuresInteger.size(); ++featureIndex){
        GetFeatureWithValueAsString(CINTEGER , featuresInteger[featureIndex] , featureAndValue);
        ss << featureTotalIndex + 1 << " " << featureAndValue << std::endl;
        featureTotalIndex++;
    }


    status = ss.str();

}

void CameraGrabberBasler::GetFeatureWithValueAsString(FeatureType featureType , const GenICam::gcstring &featureName, std::string &featureAndValue){
    /// featureType stand for the type of the feature
    /// for now we only use 4 feature type:
    /// CInteger , CFloat , CString , CEnumeration

    std::stringstream fv;

    // get the camera feature NodeMap
    GenApi::INodeMap & nodeMap = camera_->GetNodeMap();

    switch (featureType) {
    case CINTEGER:{
        GenApi::CIntegerPtr pIntFeature(nodeMap.GetNode(featureName));
        if(GenApi::IsAvailable(pIntFeature)){
            if(GenApi::IsReadable(pIntFeature)){
                fv << featureName << " : " << pIntFeature->ToString() << " ";
            }else {
                fv << featureName << " : " << "Not Readable" << " ";
            }
        }else {
            fv << featureName << " : " << "Not Available" << " ";
        }

        break;
    }
    case CFLOAT:{

        GenApi::CFloatPtr pFloatFeature(nodeMap.GetNode(featureName));
        if(GenApi::IsAvailable(pFloatFeature)){
            if(GenApi::IsReadable(pFloatFeature)){
                fv << featureName << " : " << pFloatFeature->ToString() << " ";
            }else {
                fv << featureName << " : " << "Not Readable" << " ";
            }
        }else {
            fv << featureName << " : " << "Not Available" << " ";
        }

        break;
    }
    case CSTRING:{

        GenApi::CStringPtr pStringFeature(nodeMap.GetNode(featureName));
        if(GenApi::IsAvailable(pStringFeature)){
            if(GenApi::IsReadable(pStringFeature)){
                fv << featureName << " : " << pStringFeature->ToString() << " ";
            }else {
                fv << featureName << " : " << "Not Readable" << " ";
            }
        }else {
            fv << featureName << " : " << "Not Available" << " ";
        }

        break;
    }
    case CENUMERATION:{

        GenApi::CEnumerationPtr pEnumFeature(nodeMap.GetNode(featureName));
        if(GenApi::IsAvailable(pEnumFeature)){
            if(GenApi::IsReadable(pEnumFeature)){
                fv << featureName << " : " << pEnumFeature->ToString() << " ";
            }else {
                fv << featureName << " : " << "Not Readable" << " ";
            }
        }else {
            fv << featureName << " : " << "Not Available" << " ";
        }

        break;
    }
    default:{
        fv << featureName << " : " << "Feature type not available" << " ";
        break;
    }
    }
    featureAndValue = fv.str();
}


void CameraGrabberBasler::SaveFeaturesToFile(){
    try{
        if(!camera_->IsOpen()){
            camera_->Open();
        }

        Pylon::CFeaturePersistence::Save(camConfigFile_.c_str() , &camera_->GetNodeMap());
    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured : " << e.GetDescription() << std::endl;
    }

}

void CameraGrabberBasler::LoadFeaturesFromFile(){
    try{
        if(!camera_->IsOpen()){
            camera_->Open();
        }
        Pylon::CFeaturePersistence::Load(camConfigFile_.c_str() , &camera_->GetNodeMap() , true);

    }catch(const Pylon::GenericException & e){
        std::cerr << "[ERROR] an exception occured :" << e.GetDescription() << std::endl;
    }
}


void CameraGrabberBasler::SaveImages(const cv::Mat & frame , double timeStamp){
    std::stringstream fileName;
    unsigned long long timeSec = static_cast<unsigned long long>(timeStamp);
    unsigned long long timeUsec = static_cast<unsigned long long>(floor((timeStamp - (double)timeSec) * 1000000.0 + 0.5));
    fileName << saveImgDir_ << "/" << imgPrefix_ << "-" << timeSec << "-" << timeUsec << imgPostfix_;

    std::cout << "image path : " << fileName.str() << std::endl;
    cv::imwrite(fileName.str() , frame);

}

