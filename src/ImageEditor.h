#ifndef IMAGE_EDITOR_H
#define IMAGE_EDITOR_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <opencv2/opencv.hpp>
#include "ImageCanvas.h"
#include "ImagePreview.h"

class ImageEditor : public wxPanel
{
public:
    ImageEditor(wxWindow* parent, std::shared_ptr<ImagePreview> imagePreview);
    void LoadImage(std::shared_ptr<cv::UMat> img);

private:
    ImageCanvas* imageCanvas;        // Image viewer canvas
    wxComboBox* zoomComboBox;        // Combo box for zoom levels
    wxStaticText* zoomLabel;         // Label to show current zoom level
    std::shared_ptr<ImagePreview> imagePreview;  // Image preview instance

    void OnZoomSelection(wxCommandEvent& event);  // Event handler for zoom combo box selection
    void UpdateZoomComboBox(float zoomLevel);     // Update combo box selection and label when zoom level changes
    void UpdateZoomLabel();                       // Update the zoom label text

wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_EDITOR_H
