#include <wx/wx.h>
#include "ImageEditor.h"
#include <opencv2/opencv.hpp>

// This will include the necessary Objective-C headers
#ifdef __WXMAC__
#import <AppKit/AppKit.h>
#endif

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    void ForceDarkMode();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    ImageEditor* editor;

    void OnOpen(wxCommandEvent& event);
    void CreateMenuBar();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    // Force dark mode on macOS
    ForceDarkMode();

    MyFrame* frame = new MyFrame("Image Viewer");
    frame->Show(true);
    frame->Maximize(true);  // Maximize the window on start
    return true;
}

void MyApp::ForceDarkMode()
{
#ifdef __WXMAC__
    // Use Objective-C API to force dark mode on macOS
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSRequiresAquaSystemAppearance"];
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"AppleInterfaceStyleSwitchesAutomatically"];
    [[NSUserDefaults standardUserDefaults] setObject:@"Dark" forKey:@"AppleInterfaceStyle"];

    // Force the appearance update
    NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
#endif
}

MyFrame::MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    editor = new ImageEditor(this);
    editor->Disable();  // Disable the editor until an image is loaded

    // Create the menu bar
    CreateMenuBar();

    // Set dark background and foreground colors
    SetBackgroundColour(wxColour(40, 40, 40));
    SetForegroundColour(wxColour(255, 255, 255));
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
        editor->LoadImage(image);
        editor->Enable();  // Enable the editor after loading the image
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
