#include <wx/wx.h>
#include "ImageCanvas.h"
#include <opencv2/opencv.hpp>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    ImageCanvas* canvas;

    void OnOpen(wxCommandEvent& event);
    void CreateMenuBar();
    void UpdateZoomStatus(float zoomLevel);
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("Image Viewer with wxGraphicsContext");
    frame->Show(true);
    frame->Maximize(true);  // Maximize the window on start
    return true;
}

MyFrame::MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    // Create the canvas
    canvas = new ImageCanvas(this);

    // Set zoom update callback
    canvas->SetZoomCallback([this](float zoomLevel) { UpdateZoomStatus(zoomLevel); });

    // Create the menu bar
    CreateMenuBar();

    // Create a status bar
    CreateStatusBar();
    SetStatusText("Zoom: 100%");
}

void MyFrame::OnOpen(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
                                "Image files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;  // User canceled the dialog

    // Load the image using OpenCV
    wxString filePath = openFileDialog.GetPath();
    cv::Mat image = cv::imread(filePath.ToStdString());

    if (!image.empty())
    {
        // Load the image into the canvas
        canvas->LoadImage(image);
    }
    else
    {
        wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
    }
}

void MyFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open an image file");
    fileMenu->Append(wxID_EXIT, "&Quit\tCtrl-Q", "Quit the application");

    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, [this](wxCommandEvent&){ Close(true); }, wxID_EXIT);  // Correctly close the frame

    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
}

void MyFrame::UpdateZoomStatus(float zoomLevel)
{
    wxString zoomText = wxString::Format("Zoom: %.0f%%", zoomLevel * 100);
    SetStatusText(zoomText);
}
