#include "UsuariosFrame.h"
#include "utils/window/WindowUtils.h"
#include <wx/grid.h>

UsuariosFrame::UsuariosFrame(wxWindow* parent, MainFrame* mainFramePtr)
    : wxFrame(parent, wxID_ANY, _("Usuarios"), wxDefaultPosition, wxDefaultSize),
    mainFramePun(mainFramePtr)
{
    AplicarIconoPrincipal(this);
    SetSizeHints(500, 400, 500, 400); // TamaÃÂÃÂ±o fijo
    SetWindowStyleFlag(GetWindowStyleFlag() & ~wxRESIZE_BORDER); // Quitar borde de redimensionar
    Widgets();
	CreateOrConnectDB(); // Crea o conecta a la base de datos al iniciar la ventana
    GetUsuariosFromDB();
    CrearJSONSesion(); // Crea el archivo JSON de sesiÃÂÃÂ³n si no existe
	getSessionFromJSON(); // Carga el nombre de usuario seleccionado desde el JSON
}


void UsuariosFrame::Widgets() {
    mainPanel = new wxPanel(this, wxID_ANY);

    // TÃÂÃÂ­tulo: AÃÂÃÂADIR
    wxStaticText* titulo = new wxStaticText(mainPanel, wxID_ANY, _("ADD"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    titulo->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // Campo: Nombre
    wxStaticText* nombreLabel = new wxStaticText(mainPanel, wxID_ANY, _("Name"));
    nombreInput = new wxTextCtrl(mainPanel, wxID_ANY);

    // Botones Agregar / Eliminar
    wxButton* btnAgregar = new wxButton(mainPanel, wxID_ANY, _("ADD"), wxDefaultPosition, wxSize(120, 40), wxBORDER_NONE);

    btnAgregar->Bind(wxEVT_ENTER_WINDOW, &UsuariosFrame::OnButtonEnter, this);
    btnAgregar->Bind(wxEVT_LEAVE_WINDOW, &UsuariosFrame::OnButtonLeave, this);
    btnAgregar->Bind(wxEVT_BUTTON, &UsuariosFrame::OnAgregarUsuario, this);

    wxButton* btnEliminar = new wxButton(mainPanel, wxID_ANY, _("DELETE"), wxDefaultPosition, wxSize(120, 40), wxBORDER_NONE);
    btnEliminar->Bind(wxEVT_ENTER_WINDOW, &UsuariosFrame::OnButtonEnter, this);
    btnEliminar->Bind(wxEVT_LEAVE_WINDOW, &UsuariosFrame::OnButtonLeave, this);
	btnEliminar->Bind(wxEVT_BUTTON, &UsuariosFrame::OnEliminarusuario, this);

    // Tabla de usuarios
    grid = new wxGrid(mainPanel, wxID_ANY);
    grid->CreateGrid(0, 2);  // 0 filas, 2 columnas
    grid->SetColLabelValue(0, "ID");
    grid->SetColLabelValue(1, _("User"));

    // Columna ID oculta o minimizada
    grid->SetColSize(0, 0);

    // Opcional: columnas de solo lectura
    grid->SetReadOnly(0, 0, true);  // Fila 0, columna 0
    grid->SetReadOnly(0, 1, true);

    // TamaÃÂÃÂ±o mÃÂÃÂ­nimo para asegurar visibilidad inicial
    grid->SetMinSize(wxSize(300, 200));

    // Ajustar columna "Usuario" despuÃÂÃÂ©s de que el grid estÃÂÃÂ© visible
    CallAfter([=]() {
        grid->SetColSize(0, 0); // Asegura que la columna ID estÃÂÃÂ© oculta
        int anchoTotal = grid->GetClientSize().GetWidth();
        anchoTotal -= 83;
        grid->SetColSize(1, anchoTotal);
        });

    // Texto de sesiÃÂÃÂ³n
    sesionLabel = new wxStaticText(mainPanel, wxID_ANY, _("SESSION: ") + nombreUsuarioSeleccionado);
    sesionLabel->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // Botones sesiÃÂÃÂ³n
    wxButton* btnIniciar = new wxButton(mainPanel, wxID_ANY, _("LOG IN"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnIniciar->Bind(wxEVT_ENTER_WINDOW, &UsuariosFrame::OnButtonEnter, this);
    btnIniciar->Bind(wxEVT_LEAVE_WINDOW, &UsuariosFrame::OnButtonLeave, this);
	btnIniciar->Bind(wxEVT_BUTTON, &UsuariosFrame::OnIniciarSesion, this);

    wxButton* btnCerrarTodo = new wxButton(mainPanel, wxID_ANY, _("LOG OUT OF ALL SESSIONS"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    btnCerrarTodo->Bind(wxEVT_ENTER_WINDOW, &UsuariosFrame::OnButtonEnter, this);
    btnCerrarTodo->Bind(wxEVT_LEAVE_WINDOW, &UsuariosFrame::OnButtonLeave, this);
	btnCerrarTodo->Bind(wxEVT_BUTTON, &UsuariosFrame::OnLogOut, this);

    // Layouts verticales y horizontales
    wxBoxSizer* izquierdaSizer = new wxBoxSizer(wxVERTICAL);
    izquierdaSizer->Add(titulo, 0, wxALIGN_CENTER | wxBOTTOM, 10);
    izquierdaSizer->Add(nombreLabel, 0, wxLEFT | wxRIGHT, 10);
    izquierdaSizer->Add(nombreInput, 0, wxEXPAND | wxALL, 10);
    izquierdaSizer->Add(btnAgregar, 0, wxEXPAND | wxALL, 10);
    izquierdaSizer->Add(btnEliminar, 0, wxEXPAND | wxALL, 10);

    wxBoxSizer* derechaSizer = new wxBoxSizer(wxVERTICAL);
    derechaSizer->Add(grid, 0, wxALIGN_TOP | wxALL, 10);  // No expandible

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(izquierdaSizer, 0, wxEXPAND | wxRIGHT, 10);
    topSizer->Add(derechaSizer, 0, wxALIGN_TOP);  // No expandible

    wxBoxSizer* sesionSizer = new wxBoxSizer(wxHORIZONTAL);
    sesionSizer->Add(btnIniciar, 0, wxALL, 10);
    sesionSizer->Add(btnCerrarTodo, 0, wxALL, 10);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(topSizer, 0, wxALIGN_CENTER);  // No expansiÃÂÃÂ³n vertical total
    mainSizer->Add(sesionLabel, 0, wxALIGN_CENTER | wxBOTTOM, 5);
    mainSizer->Add(sesionSizer, 0, wxALIGN_CENTER);

    mainPanel->SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
    mainPanel->Layout();
}



void UsuariosFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    ApplyTheme(mainPanel, oscuro);
    Refresh();
    Update();
}

void UsuariosFrame::OnButtonEnter(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    SetButtonHover(btn, true, temaOscuro);
    event.Skip();
}

void UsuariosFrame::OnButtonLeave(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    btn->SetCursor(wxCursor(wxCURSOR_ARROW));
    SetButtonHover(btn, false, temaOscuro);
    event.Skip();
}