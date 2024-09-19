#include "ImageCanvas.h"

ImageCanvas::ImageCanvas(wxFrame* parent)
        : wxPanel(parent), zoomFactor(1.0f), offsetX(0.0f), offsetY(0.0f), isPanning(false), imageLoaded(false)
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
    float zoomDelta = evt.GetZoomFactor();  // Use the gesture's zoom factor
    float minZoom = 0.1f;
    float maxZoom = 10.0f;

    zoomFactor = std::clamp(zoomDelta, minZoom, maxZoom);

    // Update zoom level in status bar
    if (zoomCallback)
    {
        zoomCallback(zoomFactor);
    }

    ConstrainPan();  // Ensure the image stays within the canvas bounds
    Refresh();  // Redraw the canvas after zooming
}

// Handle macOS pan gestures (left, right, up, down)
void ImageCanvas::OnGesturePan(wxPanGestureEvent& evt)
{
    // Update offsets based on the pan gesture position
    offsetX += evt.GetDelta().x;  // Use GetPosition().x for the pan gesture's X delta
    offsetY += evt.GetDelta().y;  // Use GetPosition().y for the pan gesture's Y delta

    ConstrainPan();  // Ensure the image stays within the canvas bounds
    Refresh();  // Redraw the canvas after panning
}

void ImageCanvas::ConstrainPan()
{
    wxSize clientSize = GetClientSize();
    float scaledWidth = wxImg.GetWidth() * zoomFactor;
    float scaledHeight = wxImg.GetHeight() * zoomFactor;

    // Constrain horizontal panning
    if (scaledWidth < clientSize.GetWidth())
    {
        offsetX = (clientSize.GetWidth() - scaledWidth) / 2.0f;  // Center horizontally if image is smaller than canvas
    }
    else
    {
        if (offsetX > 0)
        {
            offsetX = 0;  // Prevent panning beyond the left edge
        }
        if (offsetX + scaledWidth < clientSize.GetWidth())
        {
            offsetX = clientSize.GetWidth() - scaledWidth;  // Prevent panning beyond the right edge
        }
    }

    // Constrain vertical panning
    if (scaledHeight < clientSize.GetHeight())
    {
        offsetY = (clientSize.GetHeight() - scaledHeight) / 2.0f;  // Center vertically if image is smaller than canvas
    }
    else
    {
        if (offsetY > 0)
        {
            offsetY = 0;  // Prevent panning beyond the top edge
        }
        if (offsetY + scaledHeight < clientSize.GetHeight())
        {
            offsetY = clientSize.GetHeight() - scaledHeight;  // Prevent panning beyond the bottom edge
        }
    }
}
