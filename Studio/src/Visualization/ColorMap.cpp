#include "ColorMap.h"

#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
void ColorMap::construct_lookup_table(vtkSmartPointer<vtkLookupTable> lut) {
  if (discrete_mode_) {
    color_series_->BuildLookupTable(lut, vtkColorSeries::LUTMode::ORDINAL);
  } else {
    auto color_transfer = vtkSmartPointer<vtkColorTransferFunction>::New();

    int num_colors = color_series_->GetNumberOfColors();
    for (int i = 0; i < num_colors; i++) {
      double ratio = static_cast<double>(i) / (num_colors - 1);
      vtkColor3ub color = color_series_->GetColor(i);
      color_transfer->AddRGBPoint(ratio, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }

    lut->SetNumberOfTableValues(100);
    for (int i = 0; i < 100; i++) {
      double rgb[3];
      color_transfer->GetColor(i / 99.0, rgb);
      lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
    }
  }
  lut->Modified();
}

//-----------------------------------------------------------------------------
vtkColor3ub ColorMap::convert(QColor color) { return vtkColor3ub(color.red(), color.green(), color.blue()); }

//-----------------------------------------------------------------------------
ColorMaps::ColorMaps() {
  auto add_custom_series = [&](auto name, std::vector<QColor> colors) {
    ColorMap map;
    map.name_ = name;
    map.color_series_ = vtkSmartPointer<vtkColorSeries>::New();
    map.color_series_->ClearColors();
    for (const auto& color : colors) {
      map.color_series_->AddColor(ColorMap::convert(color));
    }
    push_back(map);
  };

  add_custom_series("Rainbow", {Qt::blue, Qt::cyan, Qt::green, Qt::yellow, Qt::red});
  add_custom_series("Reverse Rainbow", {Qt::red, Qt::yellow, Qt::green, Qt::cyan, Qt::blue});
  add_custom_series("Grayscale", {Qt::black, Qt::darkGray, Qt::lightGray, Qt::white});
  add_custom_series("Black-Body Radiation", {Qt::black, Qt::red, Qt::yellow, Qt::white});
  add_custom_series("Blue to Yellow", {Qt::blue, Qt::yellow});

  auto add_vtk_series = [&](auto series) {
    vtkNew<vtkColorSeries> colorSeries;
    int colorSeriesEnum = series;
    colorSeries->SetColorScheme(colorSeriesEnum);
    ColorMap map;
    map.color_series_ = colorSeries;
    map.name_ = colorSeries->GetColorSchemeName();
    push_back(map);
  };

  for (int i = vtkColorSeries::SPECTRUM; i < vtkColorSeries::CUSTOM; i++) {
    add_vtk_series(i);
  }
}

}  // namespace shapeworks
