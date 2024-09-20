#include "ImagePreview.h"


const int ImagePreview::TARGET_LOD_LOW_PIXELS = 3000000;
const int ImagePreview::TARGET_LOD_MEDIUM_PIXELS = 6000000;
const int ImagePreview::TARGET_LOD_HIGH_PIXELS = 9000000;


ImagePreview::ImagePreview() : current_lod_level(LodLevel::LOW) {
}


ImagePreview::~ImagePreview() {
}


void ImagePreview::LoadImage(const std::shared_ptr<cv::UMat> &in_image) {
    GenerateLodImages(in_image);
    SetLodLevel(LodLevel::LOW);
}


void ImagePreview::GenerateLodImages(const std::shared_ptr<cv::UMat>& in_image) {
    lod_images.clear();
    lod_images.clear();

    std::shared_ptr<cv::UMat> lod_low = GenerateLodImage(in_image, LodLevel::LOW);
    lod_images.insert({LodLevel::LOW, lod_low});
    lod_sizes.insert({LodLevel::LOW, lod_low->size()});

    std::shared_ptr<cv::UMat> lod_medium = GenerateLodImage(in_image, LodLevel::MEDIUM);
    lod_images.insert({LodLevel::MEDIUM, lod_medium});
    lod_sizes.insert({LodLevel::MEDIUM, lod_medium->size()});

    std::shared_ptr<cv::UMat> lod_high = GenerateLodImage(in_image, LodLevel::HIGH);
    lod_images.insert({LodLevel::HIGH, lod_high});
    lod_sizes.insert({LodLevel::HIGH, lod_high->size()});
}


std::shared_ptr<cv::UMat> ImagePreview::GenerateLodImage(const std::shared_ptr<cv::UMat>& in_image, LodLevel lod_level) {
    std::cout << "Generating LOD image for level " << static_cast<int>(lod_level) << std::endl;
    int target_px = 0;

    switch (lod_level) {
        case LodLevel::LOW:
            target_px = TARGET_LOD_LOW_PIXELS;
            break;
        case LodLevel::MEDIUM:
            target_px = TARGET_LOD_MEDIUM_PIXELS;
            break;
        case LodLevel::HIGH:
            target_px = TARGET_LOD_HIGH_PIXELS;
            break;
        default:
            target_px = TARGET_LOD_HIGH_PIXELS;
    }

    auto image_copy = std::make_shared<cv::UMat>(in_image->clone());
    auto cv_lod_image = ResizeImageLod(image_copy, target_px);
    return cv_lod_image;
}


std::shared_ptr<cv::UMat> ImagePreview::ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px) {
    auto out_image = std::make_shared<cv::UMat>(in_image->clone());

    // Calculate the total number of pixels in the original image
    int res = in_image->cols * in_image->rows;

    // Calculate the correct resize factor for linear dimensions
    float resize_factor = std::sqrt((float) target_px / (float) res);

    // Resize the image if the factor is less than 1 (i.e., target size is smaller than the original size)
    if (resize_factor < 1) {
        auto new_width = static_cast<int>(std::ceil(in_image->cols * resize_factor));
        ResizeImageByWidth(*in_image, *out_image, new_width);

        std::cout << "Image resized for preview to " << out_image->cols << "x" << out_image->rows
                  << " with factor " << resize_factor << std::endl;
    }

    return out_image;
}


void ImagePreview::SetLodLevel(ImagePreview::LodLevel lod_level) {
    if (lod_level == current_lod_level) {
        return;
    }

    current_lod_level = lod_level;
    partial_lod_image = std::make_shared<cv::UMat>(lod_images.at(lod_level)->clone());
}


cv::Mat ImagePreview::GetImage() const {
    std::cout << "Getting image for LOD level " << static_cast<int>(current_lod_level) << std::endl;
    cv::Mat imageMat = lod_images.at(current_lod_level)->getMat(cv::ACCESS_READ);
    return imageMat;
}


void ImagePreview::ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth) {
    // Get the original dimensions
    int originalWidth = inputImage.cols;
    int originalHeight = inputImage.rows;

    // Calculate the new height to maintain aspect ratio
    int newHeight = static_cast<int>(newWidth * (static_cast<float>(originalHeight) / originalWidth));

    // Use Lanczos interpolation for high-quality resizing
    cv::resize(inputImage, outputImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LANCZOS4);
}
