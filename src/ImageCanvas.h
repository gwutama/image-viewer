#ifndef IMAGE_CANVAS_H
#define IMAGE_CANVAS_H

#include <wx/wx.h>
#include <opencv2/opencv.hpp>

class ImageCanvas : public wxPanel
{
public:
    ImageCanvas(wxWindow* parent);  // Use wxWindow* instead of wxFrame* for flexibility
    ~ImageCanvas();

    // Load a single image
    void LoadImage(const cv::Mat& img);

    // Set zoom level manually
    void SetZoomLevel(float zoom);

    // Fit the image to the canvas dimensions
    void FitImageToCanvas();

    // Set a callback to update zoom level in the status bar
    void SetZoomCallback(std::function<void(float)> callback);

    // Enable or disable touch gestures
    void EnableGestures(bool enable);

protected:
    void OnPaint(wxPaintEvent& evt);

    void OnGestureZoom(wxZoomGestureEvent& evt);
    void OnGesturePan(wxPanGestureEvent& evt);

private:
    wxImage wxImg;
    bool imageLoaded;

    float zoomFactor;
    float offsetX, offsetY;

    std::function<void(float)> zoomCallback;

    static constexpr float MIN_ZOOM_FACTOR = 0.1f;
    static constexpr float MAX_ZOOM_FACTOR = 10.0f;

wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_CANVAS_H
