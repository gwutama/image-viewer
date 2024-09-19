#include "ImageCanvas.h"

wxBEGIN_EVENT_TABLE(ImageCanvas, wxPanel)
                // Event table definition if needed
wxEND_EVENT_TABLE()

ImageCanvas::ImageCanvas(wxWindow* parent)
        : wxPanel(parent), zoomFactor(1.0f), offsetX(0.0f), offsetY(0.0f), imageLoaded(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);  // Enable repainting the canvas
    Bind(wxEVT_PAINT, &ImageCanvas::OnPaint, this);

    EnableGestures(true);  // Enable gestures by default
}

ImageCanvas::~ImageCanvas()
{
    Unbind(wxEVT_PAINT, &ImageCanvas::OnPaint, this);
    EnableGestures(false);
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

    wxImg = wxImage(imgRGB.cols, imgRGB.rows, imgRGB.data, true);
    imageLoaded = true;
    FitImageToCanvas();
    Refresh();
}

void ImageCanvas::SetZoomLevel(float zoom)
{
    zoomFactor = std::clamp(zoom, MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);
    Refresh();
}

void ImageCanvas::CenterImageOnCanvas()
{
    wxSize clientSize = GetClientSize();
    float scaledWidth = wxImg.GetWidth() * zoomFactor;
    float scaledHeight = wxImg.GetHeight() * zoomFactor;
    offsetX = (clientSize.GetWidth() - scaledWidth) / 2.0f;
    offsetY = (clientSize.GetHeight() - scaledHeight) / 2.0f;
    Refresh();
}

void ImageCanvas::FitImageToCanvas()
{
    if (!imageLoaded)
        return;

    wxSize clientSize = GetClientSize();
    float scaleX = static_cast<float>(clientSize.GetWidth()) / wxImg.GetWidth();
    float scaleY = static_cast<float>(clientSize.GetHeight()) / wxImg.GetHeight();
    zoomFactor = std::min(scaleX, scaleY);

    float scaledWidth = wxImg.GetWidth() * zoomFactor;
    float scaledHeight = wxImg.GetHeight() * zoomFactor;
    offsetX = (clientSize.GetWidth() - scaledWidth) / 2.0f;
    offsetY = (clientSize.GetHeight() - scaledHeight) / 2.0f;

    Refresh();
}

void ImageCanvas::EnableGestures(bool enable)
{
    if (enable)
    {
        if (!EnableTouchEvents(wxTOUCH_ALL_GESTURES) )
        {
            wxLogError("Failed to enable touch events");
        }

        Bind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
        Bind(wxEVT_GESTURE_PAN, &ImageCanvas::OnGesturePan, this);
    }
    else
    {
        Unbind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
        Unbind(wxEVT_GESTURE_PAN, &ImageCanvas::OnGesturePan, this);
    }
}

void ImageCanvas::SetZoomCallback(std::function<void(float)> callback)
{
    zoomCallback = callback;
}

void ImageCanvas::OnPaint(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc || !imageLoaded)
    {
        return;
    }

    gc->Translate(offsetX, offsetY);
    gc->Scale(zoomFactor, zoomFactor);

    wxBitmap bitmap(wxImg);
    gc->DrawBitmap(bitmap, 0, 0, bitmap.GetWidth(), bitmap.GetHeight());

    delete gc;
}

void ImageCanvas::OnGestureZoom(wxZoomGestureEvent& evt)
{
    wxPoint mousePos = ScreenToClient(wxGetMousePosition());

    float mouseImageX = (mousePos.x - offsetX) / zoomFactor;
    float mouseImageY = (mousePos.y - offsetY) / zoomFactor;

    float zoomDelta = evt.GetZoomFactor();
    zoomFactor = std::clamp(zoomFactor * (1 + (zoomDelta - 1) * 0.5f), MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);

    if (zoomCallback)
    {
        zoomCallback(zoomFactor);
    }

    offsetX = mousePos.x - mouseImageX * zoomFactor;
    offsetY = mousePos.y - mouseImageY * zoomFactor;

    Refresh();
}

void ImageCanvas::OnGesturePan(wxPanGestureEvent& evt)
{
    offsetX += evt.GetDelta().x;
    offsetY += evt.GetDelta().y;
    Refresh();
}
