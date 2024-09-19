#ifndef IMAGE_CANVAS_H
#define IMAGE_CANVAS_H

#include <wx/wx.h>
#include <opencv2/opencv.hpp>

class ImageCanvas : public wxPanel
{
public:
    ImageCanvas(wxFrame* parent);
    ~ImageCanvas();

    // Load a single image (no LOD)
    void LoadImage(const cv::Mat& img);

    // Set a callback to update zoom level in the status bar
    void SetZoomCallback(std::function<void(float)> callback);

protected:
    void OnPaint(wxPaintEvent& evt);
    void OnMouseDown(wxMouseEvent& evt);
    void OnMouseUp(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);

    void OnGestureZoom(wxZoomGestureEvent& evt);
    void OnGesturePan(wxPanGestureEvent& evt);

private:
    wxImage wxImg;                      // wxImage to store the loaded image
    bool imageLoaded;                   // Flag to check if an image is loaded

    float zoomFactor;                   // Zoom factor for the image
    float offsetX, offsetY;             // Offset for panning and initial centering
    float lastMouseX, lastMouseY;       // Track last mouse position for panning

    std::function<void(float)> zoomCallback;  // Callback to update the zoom level in status bar

    void FitImageToCanvas();            // Function to fit and center the image on load
    void ConstrainPan();                // Prevent image from being panned out of the visible area

    static constexpr float MIN_ZOOM_FACTOR = 0.1f;  // Minimum zoom factor
    static constexpr float MAX_ZOOM_FACTOR = 10.0f; // Maximum zoom factor

//wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_CANVAS_H
