#include "ImageCanvas.h"

wxBEGIN_EVENT_TABLE(ImageCanvas, wxGLCanvas)
                EVT_PAINT(ImageCanvas::OnPaint)
                EVT_SIZE(ImageCanvas::OnResize)
                EVT_LEFT_DOWN(ImageCanvas::OnMouseDown)
                EVT_LEFT_UP(ImageCanvas::OnMouseUp)
                EVT_MOTION(ImageCanvas::OnMouseMove)
wxEND_EVENT_TABLE()

ImageCanvas::ImageCanvas(wxWindow* parent)
        : wxGLCanvas(parent, wxID_ANY, nullptr), zoomFactor(1.0f), offsetX(0.0f), offsetY(0.0f),
          imageLoaded(false), textureId(0), isDragging(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    glContext = new wxGLContext(this);
}

ImageCanvas::~ImageCanvas()
{
    if (glContext)
    {
        delete glContext;
    }
    if (textureId)
    {
        glDeleteTextures(1, &textureId);
    }
}

void ImageCanvas::LoadImage(const cv::Mat& img)
{
    // Convert OpenCV image to wxImage
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

    // Update OpenGL texture
    UpdateTexture();

    // Fit and center the image
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

    CenterImageOnCanvas();
}

void ImageCanvas::EnableGestures(bool enable)
{
    if (enable)
    {
        if (!EnableTouchEvents(wxTOUCH_ZOOM_GESTURE))
        {
            wxLogError("Failed to enable touch events");
        }

        Bind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    }
    else
    {
        Unbind(wxEVT_GESTURE_ZOOM, &ImageCanvas::OnGestureZoom, this);
    }
}

void ImageCanvas::SetZoomCallback(std::function<void(float)> callback)
{
    zoomCallback = callback;
}

void ImageCanvas::OnPaint(wxPaintEvent& evt)
{
    if (!imageLoaded)
        return;

    wxPaintDC dc(this);
    SetCurrent(*glContext);

    // Set the background color to dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    wxSize clientSize = GetClientSize();
    glOrtho(0, clientSize.GetWidth(), clientSize.GetHeight(), 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(offsetX, offsetY, 0);
    glScalef(zoomFactor, zoomFactor, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(wxImg.GetWidth(), 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(wxImg.GetWidth(), wxImg.GetHeight());
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, wxImg.GetHeight());
    glEnd();

    glDisable(GL_TEXTURE_2D);

    SwapBuffers();
}

void ImageCanvas::OnResize(wxSizeEvent& evt)
{
    FitImageToCanvas();
}

void ImageCanvas::OnMouseDown(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
        isDragging = true;
        dragStartPos = event.GetPosition();
        lastOffsetX = offsetX;
        lastOffsetY = offsetY;
    }
}

void ImageCanvas::OnMouseUp(wxMouseEvent& event)
{
    if (event.LeftUp())
    {
        SetCursor(wxCursor(wxCURSOR_NONE));
        isDragging = false;
    }
}

void ImageCanvas::OnMouseMove(wxMouseEvent& event)
{
    if (isDragging)
    {
        wxPoint currentPos = event.GetPosition();
        int deltaX = currentPos.x - dragStartPos.x;
        int deltaY = currentPos.y - dragStartPos.y;

        offsetX = lastOffsetX + deltaX;
        offsetY = lastOffsetY + deltaY;

        Refresh();  // Redraw the canvas after panning
    }
}

void ImageCanvas::OnGestureZoom(wxZoomGestureEvent& evt)
{
    // Get the current mouse position relative to the canvas
    wxPoint mousePos = ScreenToClient(wxGetMousePosition());

    // Calculate the position of the mouse relative to the image (before zooming)
    float mouseImageX = (mousePos.x - offsetX) / zoomFactor;
    float mouseImageY = (mousePos.y - offsetY) / zoomFactor;

    // Get the zoom factor from the gesture event
    float zoomDelta = evt.GetZoomFactor();

    // Scale zoomDelta to smooth the zoom operation
    float newZoomFactor = std::clamp(zoomFactor * (1 + (zoomDelta - 1) * 0.2f), MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);

    zoomFactor = newZoomFactor;

    // Recalculate the offsets so that the point under the mouse stays under the mouse after zooming
    offsetX = mousePos.x - mouseImageX * zoomFactor;
    offsetY = mousePos.y - mouseImageY * zoomFactor;

    // Update the zoom level in the status bar or UI if necessary
    if (zoomCallback)
    {
        zoomCallback(zoomFactor);
    }

    // Redraw the canvas with the new zoom level
    Refresh();
}

void ImageCanvas::UpdateTexture()
{
    if (!imageLoaded)
        return;

    SetCurrent(*glContext);

    if (textureId)
    {
        glDeleteTextures(1, &textureId);
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wxImg.GetWidth(), wxImg.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, wxImg.GetData());
}
