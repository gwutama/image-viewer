#include "ImageCanvas.h"

ImageCanvas::ImageCanvas(wxFrame* parent)
        : wxPanel(parent), zoomFactor(1.0f), offsetX(0.0f), offsetY(0.0f), imageLoaded(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);  // Enable repainting the canvas
    Bind(wxEVT_PAINT, &ImageCanvas::OnPaint, this);

    if ( !EnableTouchEvents(wxTOUCH_ALL_GESTURES) )
    {
        wxLogError("Failed to enable touch events");
    }

    // Bind the gesture events dynamically using the window's ID
    Bind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    Bind(wxEVT_GESTURE_PAN, &ImageCanvas::OnGesturePan, this);
}

ImageCanvas::~ImageCanvas()
{
    // Unbind the gesture events
    Unbind(wxEVT_PAINT, &ImageCanvas::OnPaint, this);
    Unbind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    Unbind(wxEVT_GESTURE_PAN, &ImageCanvas::OnGesturePan, this);
}

void ImageCanvas::LoadImage(const cv::Mat& img)
{
    // Convert the OpenCV image to wxImage
    cv::Mat imgRGB;
    if (img.channels() == 3)
    {
        cv::cvtColor(img, imgRGB, cv::COLOR_BGR2RGB);  // Convert BGR to RGB
    }
    else
    {
        imgRGB = img.clone();
    }

    // Load the image into wxImage
    wxImg = wxImage(imgRGB.cols, imgRGB.rows, imgRGB.data, true);
    imageLoaded = true;

    // Fit and center the image when it's first loaded
    FitImageToCanvas();

    // Refresh the canvas to draw the new image
    Refresh();
}

void ImageCanvas::SetZoomCallback(std::function<void(float)> callback)
{
    zoomCallback = callback;
}

void ImageCanvas::FitImageToCanvas()
{
    wxSize clientSize = GetClientSize();

    // Calculate the scaling factor to fit the image within the canvas while maintaining the aspect ratio
    float scaleX = static_cast<float>(clientSize.GetWidth()) / wxImg.GetWidth();
    float scaleY = static_cast<float>(clientSize.GetHeight()) / wxImg.GetHeight();
    zoomFactor = std::min(scaleX, scaleY);  // Take the smaller factor to maintain aspect ratio

    // Center the image within the canvas
    float scaledWidth = wxImg.GetWidth() * zoomFactor;
    float scaledHeight = wxImg.GetHeight() * zoomFactor;
    offsetX = (clientSize.GetWidth() - scaledWidth) / 2.0f;
    offsetY = (clientSize.GetHeight() - scaledHeight) / 2.0f;
}

void ImageCanvas::OnPaint(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    dc.Clear();  // Clear the canvas before rendering

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (!gc || !imageLoaded)
    {
        return;
    }

    // Apply zooming and panning offsets
    gc->Translate(offsetX, offsetY);
    gc->Scale(zoomFactor, zoomFactor);

    // Draw the image
    wxBitmap bitmap(wxImg);
    gc->DrawBitmap(bitmap, 0, 0, bitmap.GetWidth(), bitmap.GetHeight());

    delete gc;
}

// Handle macOS pinch-to-zoom gestures
void ImageCanvas::OnGestureZoom(wxZoomGestureEvent& evt)
{
    // Get the current mouse position relative to the canvas (recalculate every time)
    wxPoint mousePos = ScreenToClient(wxGetMousePosition());

    // Calculate the position of the mouse relative to the image (before zooming)
    float mouseImageX = (mousePos.x - offsetX) / zoomFactor;
    float mouseImageY = (mousePos.y - offsetY) / zoomFactor;

    // Adjust the zoom factor based on the gesture's zoom factor with smoother scaling
    float zoomDelta = evt.GetZoomFactor();
    zoomFactor = std::clamp(zoomFactor * (1 + (zoomDelta - 1) * 0.5f), MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);  // Smoother zoom

    // Update zoom level in status bar
    if (zoomCallback)
    {
        zoomCallback(zoomFactor);
    }

    // Recalculate the new offsets to ensure the point under the mouse remains the same
    offsetX = mousePos.x - mouseImageX * zoomFactor;
    offsetY = mousePos.y - mouseImageY * zoomFactor;

    // Redraw the canvas after zooming
    Refresh();
}


// Handle macOS pan gestures (left, right, up, down)
void ImageCanvas::OnGesturePan(wxPanGestureEvent& evt)
{
    // Set the cursor to hand grab during panning
    if (evt.IsGestureStart())
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
    }

    // Update offsets based on the pan gesture position
    offsetX += evt.GetDelta().x;  // Use GetDelta().x for the pan gesture's X delta
    offsetY += evt.GetDelta().y;  // Use GetDelta().y for the pan gesture's Y delta

    // Redraw the canvas after panning
    Refresh();

    // After handling the pan gesture, reset the cursor back to default (optional)
    if (evt.IsGestureEnd())
    {
        SetCursor(wxNullCursor);
    }
}


