#include "ImageStreamer.hpp"
#include "ImageImporter.hpp"
#include "DeviceManager.hpp"
#include "Exception.hpp"
using namespace fast;

/**
 * Dummy function to get into the class again
 */
void stubStreamThread(ImageStreamer * streamer) {
    streamer->producerStream();
}

DynamicImage::pointer ImageStreamer::getOutput() {
    if(!mStreamIsStarted) {
        mStreamIsStarted = true;
        thread = new boost::thread(&stubStreamThread, this);
    }

    // Wait here for first frame
    // TODO use condition variable instead
    while(!mFirstFrameIsInserted);

    if(mOutput.isValid()) {
        mOutput->setParent(mPtr.lock());

        DynamicImage::pointer newSmartPtr;
        newSmartPtr.swap(mOutput);

        return newSmartPtr;
    } else {
        return mOutput2.lock();
    }
}

ImageStreamer::ImageStreamer() {
    mOutput = DynamicImage::New();
    mOutput2 = mOutput;
    mStreamIsStarted = false;
    mIsModified = true;
    thread = NULL;
    mFirstFrameIsInserted = false;
    mDevice = DeviceManager::getInstance().getDefaultComputationDevice();
}


inline void ImageStreamer::execute() {
    if(!mStreamIsStarted) {
        mStreamIsStarted = true;
        thread = new boost::thread(&stubStreamThread, this);
    }

    // Wait here for first frame
    // TODO use condition variable instead
    while(!mFirstFrameIsInserted);
}

void ImageStreamer::setFilenameFormat(std::string str) {
    mFilenameFormat = str;
}

void ImageStreamer::setDevice(ExecutionDevice::pointer device) {
    mDevice = device;
}

inline std::string intToString(int number) {
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

void ImageStreamer::producerStream() {
    int i = 0;
    while(true) {
        std::string filename = mFilenameFormat;
        filename.replace(
                filename.find("#"),
                1,
                intToString(i));
        std::cout << filename << std::endl;
        try {
            ImageImporter::pointer importer = ImageImporter::New();
            importer->setFilename(filename);
            importer->setDevice(mDevice);
            Image::pointer image = importer->getOutput();
            image->update();
            DynamicImage::pointer ptr = mOutput2.lock();
            if(ptr.isValid()) {
                ptr->addFrame(image);
                mFirstFrameIsInserted = true;
            } else {
                std::cout << "DynamicImage object destroyed, stream can stop." << std::endl;
                break;
            }
            i++;
        } catch(FileNotFoundException &e) {
            std::cout << "Reached end of stream" << std::endl;
            // Reached end of stream
            break;
        }
    }
}

ImageStreamer::~ImageStreamer() {
    std::cout << "Joining the thread" << std::endl;
    // TODO stop thread as well
    thread->join();
    delete thread;
}