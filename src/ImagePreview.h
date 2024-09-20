#ifndef IMAGE_VIEWER_IMAGEPREVIEW_H
#define IMAGE_VIEWER_IMAGEPREVIEW_H


#include <map>
#include <OpenGL/gl.h>

#include <opencv2/opencv.hpp>

class ImagePreview {
public:
    enum class LodLevel {
        LOW,
        MEDIUM,
        HIGH,
    };

    ImagePreview();
    ~ImagePreview();

    void LoadImage(const std::shared_ptr<cv::UMat>& in_image);

    void SetLodLevel(LodLevel lod_level);
    std::map<LodLevel, cv::Size> GetLodSizes() const { return lod_sizes; }

    cv::Mat GetImage() const;
    cv::Size GetSize() const { return lod_sizes.at(current_lod_level); }

    cv::Size GetSize(LodLevel lodLevel) const { return lod_sizes.at(lodLevel); }

private:
    void GenerateLodImages(const std::shared_ptr<cv::UMat>& in_image);
    std::shared_ptr<cv::UMat> GenerateLodImage(const std::shared_ptr<cv::UMat>& in_image, LodLevel lod_level);
    std::shared_ptr<cv::UMat> ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px);
    void ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth);

private:
    static const int TARGET_LOD_LOW_PIXELS;
    static const int TARGET_LOD_MEDIUM_PIXELS;
    static const int TARGET_LOD_HIGH_PIXELS;
    std::map<LodLevel, std::shared_ptr<cv::UMat>> lod_images;
    std::shared_ptr<cv::UMat> partial_lod_image;
    LodLevel current_lod_level;
    std::map<LodLevel, cv::Size> lod_sizes;
};


#endif //IMAGE_VIEWER_IMAGEPREVIEW_H
