#include "catch.hpp"
#include "SegmentationRenderer.hpp"
#include "BinaryThresholding.hpp"
#include "ImageFileImporter.hpp"
#include "ImageRenderer.hpp"
#include "SimpleWindow.hpp"

using namespace fast;

TEST_CASE("SegmentationRenderer on a thresholded 2D image", "[fast][SegmentationRenderer]") {
    ImageFileImporter::pointer importer = ImageFileImporter::New();
    importer->setFilename(std::string(FAST_TEST_DATA_DIR) + "US-2Dt/US-2Dt_0.mhd");

    BinaryThresholding::pointer segmentation = BinaryThresholding::New();
    segmentation->setInputConnection(importer->getOutputPort());
    segmentation->setLowerThreshold(100);

    ImageRenderer::pointer imageRenderer = ImageRenderer::New();
    imageRenderer->addInputConnection(importer->getOutputPort());

    SegmentationRenderer::pointer renderer = SegmentationRenderer::New();
    renderer->addInputConnection(segmentation->getOutputPort());

    SimpleWindow::pointer window = SimpleWindow::New();
    window->set2DMode();
    window->addRenderer(imageRenderer);
    window->addRenderer(renderer);
    window->start();
}
