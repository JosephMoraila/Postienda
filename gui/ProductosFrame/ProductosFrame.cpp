#include "gui/ProductosFrame/ProductosFrame.h"
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/numdlg.h>
#include <limits>
#include "gui/ProductosFrame/ProductoDialog/ProductoDialog.h"

#include "utils/ValidateStringInput.h"  
#include <iomanip>
#include <sstream>
#include "utils/window/WindowUtils.h"
#include <fstream>

ProductosFrame::ProductosFrame(wxWindow* parent): wxFrame(parent, wxID_ANY, _("Products"), wxDefaultPosition, wxSize(800, 600)), enModoBusqueda(false)  // Inicializar variable
{
	AplicarIconoPrincipal(this);
	Widgets();  // Inicializa los widgets de la ventana
    CrearConexionBD();
	LoadCategoriesFromDB();
	LoadProductsFromDB();
}

