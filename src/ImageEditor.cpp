#include "ImageEditor.h"

wxBEGIN_EVENT_TABLE(ImageEditor, wxPanel)
                EVT_COMBOBOX(wxID_ANY, ImageEditor::OnZoomSelection)
wxEND_EVENT_TABLE()

ImageEditor::ImageEditor(wxWindow* parent)
        : wxPanel(parent)
{
    // Create ImageCanvas for viewing images
    imageCanvas = new ImageCanvas(this);
    imageCanvas->EnableGestures(true);

    // Create combo box for zoom levels
    wxArrayString zoomLevels;
    zoomLevels.Add("Fit to screen");
    zoomLevels.Add("10%");
    zoomLevels.Add("25%");
    zoomLevels.Add("50%");
    zoomLevels.Add("75%");
    zoomLevels.Add("100%");
    zoomLevels.Add("200%");
    zoomLevels.Add("400%");
    zoomComboBox = new wxComboBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, zoomLevels, wxCB_READONLY);

    zoomLabel = new wxStaticText(this, wxID_ANY, "");

    // Set up sizer for vertical layout
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Sizer for the combo box and label in horizontal layout
    wxBoxSizer* comboSizer = new wxBoxSizer(wxHORIZONTAL);
    comboSizer->Add(zoomComboBox, 0, wxALIGN_CENTER | wxALL, 5);
    comboSizer->Add(zoomLabel, 0, wxALIGN_CENTER | wxALL, 5);

    sizer->Add(imageCanvas, 1, wxEXPAND | wxALL, 5);
    sizer->Add(comboSizer, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(sizer);

    // Connect the ImageCanvas zoom callback to update the combo box and label when zoom level changes
    imageCanvas->SetZoomCallback([this](float zoomLevel) {
        UpdateZoomComboBox(zoomLevel);
    });
}

void ImageEditor::LoadImage(const cv::Mat& img)
{
    imageCanvas->LoadImage(img);
    zoomComboBox->SetSelection(0);  // Default to "Fit to screen" when a new image is loaded
    UpdateZoomLabel();
}

void ImageEditor::OnZoomSelection(wxCommandEvent& event)
{
    wxString selectedZoom = zoomComboBox->GetStringSelection();

    if (selectedZoom == "Fit to screen")
    {
        imageCanvas->FitImageToCanvas();
    }
    else
    {
        int zoomPercent = wxAtoi(selectedZoom.RemoveLast(1));  // Remove "%" symbol
        float zoomFactor = static_cast<float>(zoomPercent) / 100.0f;
        imageCanvas->SetZoomLevel(zoomFactor);
    }

    // Center the image after applying zoom
    imageCanvas->CenterImageOnCanvas();

    UpdateZoomLabel();
}

void ImageEditor::UpdateZoomComboBox(float zoomLevel)
{
    int zoomPercent = static_cast<int>(zoomLevel * 100);
    wxString zoomStr = wxString::Format("%d%%", zoomPercent);

    if (zoomComboBox->FindString(zoomStr) != wxNOT_FOUND)
    {
        zoomComboBox->SetStringSelection(zoomStr);
    }
    else
    {
        zoomComboBox->SetSelection(wxNOT_FOUND);
    }

    UpdateZoomLabel();
}

void ImageEditor::UpdateZoomLabel()
{
    // Update the zoom label to display the zoom level with "%"
    int zoomPercent = static_cast<int>(imageCanvas->GetZoomLevel() * 100);
    zoomLabel->SetLabel(wxString::Format("%d%%", zoomPercent));
}