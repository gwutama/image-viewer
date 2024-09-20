#ifndef IMAGE_CANVAS_H
#define IMAGE_CANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <opencv2/opencv.hpp>
#include "ImagePreview.h"

class ImageCanvas : public wxGLCanvas
{
public:
    ImageCanvas(wxWindow* parent, std::shared_ptr<ImagePreview> imagePreview);
    ~ImageCanvas();

    // Load image into ImagePreview
    void LoadImage(const std::shared_ptr<cv::UMat>& img);

    // Set zoom level manually
    void SetZoomLevel(float zoom);
    float GetZoomLevel() { return zoomFactor; }

    // Center the image on the canvas
    void CenterImageOnCanvas();

    // Fit the image to the canvas dimensions
    void FitImageToCanvas();

    // Set a callback to update zoom level in the status bar
    void SetZoomCallback(std::function<void(float)> callback);

    // Enable or disable touch gestures
    void EnableGestures(bool enable);

protected:
    void OnPaint(wxPaintEvent& evt);
    void OnResize(wxSizeEvent& evt);

    // Mouse event handlers for panning
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    // Gesture event handlers for zooming and panning
    void OnGestureZoom(wxZoomGestureEvent& evt);

private:
    void UpdateTexture();               // Update OpenGL texture
    void UpdateLodLevel();              // Update the LOD level based on zoom

    std::shared_ptr<ImagePreview> imagePreview;  // ImagePreview object
    bool imageLoaded;                   // Flag to check if an image is loaded
    GLuint textureId;                   // OpenGL texture ID
    wxGLContext* glContext;             // OpenGL context

    float zoomFactor;                   // Zoom factor for the image
    float offsetX, offsetY;             // Offset for panning
    bool isDragging;                    // Flag to track dragging state
    wxPoint dragStartPos;               // Initial mouse position for panning
    float lastOffsetX, lastOffsetY;     // Offset values before panning

    std::function<void(float)> zoomCallback;  // Callback to update zoom level in status bar

    static constexpr float MIN_ZOOM_FACTOR = 0.1f;  // Minimum zoom factor
    static constexpr float MAX_ZOOM_FACTOR = 4.0f;  // Maximum zoom factor

    ImagePreview::LodLevel currentLodLevel;   // Current LOD level

wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_CANVAS_H
