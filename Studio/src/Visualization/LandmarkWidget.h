
#include <vtkSmartPointer.h>
#include <vector>

class vtkHandleWidget;
class vtkSphereSource;

namespace shapeworks {

class Viewer;
class LandmarkCallback;
//! LandmarkWidget
/*!
 * Widget to display and manipulate landmarks
 *
 */
class LandmarkWidget {
public:

  LandmarkWidget(Viewer* viewer);

  void update_landmarks();

  void handle_callback(vtkObject *caller);
private:

  vtkSmartPointer<vtkHandleWidget> create_handle();

  Viewer *viewer_ = nullptr;

  std::vector<vtkSmartPointer<vtkHandleWidget>> handles_;

  vtkSmartPointer<vtkSphereSource> sphere_;
  vtkSmartPointer<LandmarkCallback> callback_;
};

}
