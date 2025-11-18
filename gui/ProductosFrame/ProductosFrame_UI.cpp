#include "gui/ProductosFrame/ProductosFrame.h"
#include "utils/window/WindowUtils.h"
#include "utils/FuenteUtils.h"

void ProductosFrame::Widgets() {
    mainPanel = new wxPanel(this);
    vSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* texto = new wxStaticText(mainPanel, wxID_ANY, _("Manage your products and categories.\nWhen registering a product, remember to update its inventory (stock)."));
    vSizer->Add(texto, 0, wxEXPAND | wxALL, 10);

    wxBoxSizer* hSizerBotones = new wxBoxSizer(wxHORIZONTAL);

    btnAgregarCategoria = new wxButton(mainPanel, ID_AGREGAR_CATEGORIA, _("New category"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE);
    btnAgregarProducto = new wxButton(mainPanel, ID_AGREGAR_PRODUCTO, _("New Product"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE);
    btnEliminar = new wxButton(mainPanel, ID_ELIMINAR, _("Delete"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE);
    btnModify = new wxButton(mainPanel, ID_MODIFICAR, _("Modify"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE); btnModify->Hide();
    btnStock = new wxButton(mainPanel, ID_GESTIONAR_STOCK, "Stock", wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE); btnStock->Hide();

    hSizerBotones->Add(btnAgregarCategoria, 0, wxALL, 5);
    hSizerBotones->Add(btnAgregarProducto, 0, wxALL, 5);
    hSizerBotones->Add(btnEliminar, 0, wxALL, 5);
    hSizerBotones->Add(btnModify, 0, wxALL, 5);
    hSizerBotones->Add(btnStock, 0, wxALL, 5);

    vSizer->Add(hSizerBotones, 0, wxEXPAND | wxALL, 5);

    CrearPanelBusqueda();
    vSizer->Add(panelBusqueda, 0, wxEXPAND | wxALL, 5);

    arbolCategorias = new wxTreeCtrl(mainPanel, ID_ARBOL, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_SINGLE | wxTR_HAS_BUTTONS);
    vSizer->Add(arbolCategorias, 1, wxEXPAND | wxALL, 10);

    
    btnAgregarCategoria->Bind(wxEVT_BUTTON, &ProductosFrame::OnAgregarCategoria, this);
    btnAgregarProducto->Bind(wxEVT_BUTTON, &ProductosFrame::OnAgregarProducto, this);
    
    btnEliminar->Bind(wxEVT_BUTTON, &ProductosFrame::OnEliminar, this);
    //arbolCategorias->Bind(wxEVT_TREE_SEL_CHANGED, &ProductosFrame::OnSeleccionCambiada, this);
    

    btnAgregarCategoria->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnAgregarCategoria->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
    btnAgregarProducto->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnAgregarProducto->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
    btnEliminar->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnEliminar->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
    btnModify->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnModify->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
    btnStock->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnStock->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);

    mainPanel->SetSizer(vSizer);
    mainPanel->Layout();
    SetMinSize(wxSize(800, 600));
    CenterOnParent();

    Bind(wxEVT_SIZE, &ProductosFrame::OnSize, this);

    ContextualMenu();
    AppearButtonsModify();
}

void ProductosFrame::ContextualMenu() {
    arbolCategorias->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, [this](wxTreeEvent& event) {
        wxTreeItemId itemClick = event.GetItem();
        if (!itemClick.IsOk() || itemClick == arbolCategorias->GetRootItem()) return;

        bool esCategoria = treeItemId_Category_Map.find(itemClick) != treeItemId_Category_Map.end() && treeItemId_Category_Map[itemClick]; //Si el item se encuentra en el mapa es una categorÃÂ­a

        wxMenu menu;
        menu.Append(1, esCategoria ? _("Edit category") : _("Edit product"));
        if (!esCategoria) menu.Append(2, _("Edit stock"));

        // Capturamos solo lo necesario
        wxTreeItemId item = itemClick;
        Bind(wxEVT_MENU, [this, item, esCategoria](wxCommandEvent& evt) {
            int id = evt.GetId(); //Get the ID of the selected menu
            if (id == 1)  ChangeInfo(item, esCategoria);
            else if (id == 2) StockProduct(item);

            }, wxID_ANY); // Usar wxID_ANY para evitar conflictos

        PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
        });
}

void ProductosFrame::AppearButtonsModify() {
    arbolCategorias->Bind(wxEVT_TREE_SEL_CHANGED, [this](wxTreeEvent& event) {
        wxTreeItemId item = event.GetItem();
        if (item.IsOk()) { // hay un ÃÂ­tem seleccionado
            bool esCategoria = treeItemId_Category_Map.find(item) != treeItemId_Category_Map.end() && treeItemId_Category_Map[item];
            if (esCategoria) {
                btnStock->Hide();
                btnModify->SetLabel(_("Edit category"));
                btnModify->Show();
            }
            else {
                btnModify->SetLabel(_("Edit product"));
                btnModify->Show();
                btnStock->Show();
                btnStock->Bind(wxEVT_BUTTON, [this, item](wxCommandEvent&) { //Bind is here because stock can only be done on products
                    StockProduct(item);
                    }, wxID_ANY);
            }
            btnModify->Bind(wxEVT_BUTTON, [this, item, esCategoria](wxCommandEvent&) {
                ChangeInfo(item, esCategoria);
                }, wxID_ANY); // The bind is put here because change can be done on products and category
        }
        else { btnModify->Hide(); btnStock->Hide(); }
        mainPanel->Layout();
        });
        
}


void ProductosFrame::OnButtonEnter(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    SetButtonHover(btn, true, temaOscuro);
    event.Skip();
}

void ProductosFrame::OnButtonLeave(wxMouseEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    btn->SetCursor(wxCursor(wxCURSOR_ARROW));
    SetButtonHover(btn, false, temaOscuro);
    event.Skip();
}

void ProductosFrame::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;
    //Como los panel heredan de wxWindow, se puede usar la funciÃÂ³n genÃÂ©rica ApplyTheme para aplicar el tema a todos los paneles y sus hijos
    ApplyTheme(mainPanel, oscuro);
    ApplyTheme(panelBusqueda, oscuro);
    // Si estamos en modo bÃÂºsqueda, restaurar y volver a aplicar formato
    if (enModoBusqueda) {
        wxString terminoActual = textoBusqueda->GetValue();
        if (!terminoActual.IsEmpty()) {

        }
    }

    Refresh();
    Update();
}


void ProductosFrame::OnSize(wxSizeEvent& event) {
    for (wxWindow* child : mainPanel->GetChildren()) {
        if (auto* btn = dynamic_cast<wxButton*>(child)) {
            AjustarFuenteSegunAlto(this, btn, nullptr, mainPanel);
        }
    }
    event.Skip();
}

void ProductosFrame::CrearPanelBusqueda() {
    panelBusqueda = new wxPanel(mainPanel);
    wxBoxSizer* hSizerBusqueda = new wxBoxSizer(wxHORIZONTAL);

    // Etiqueta
    wxStaticText* labelBuscar = new wxStaticText(panelBusqueda, wxID_ANY, _("Search:"));
    hSizerBusqueda->Add(labelBuscar, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    // Campo de texto para bÃÂºsqueda
    textoBusqueda = new wxTextCtrl(panelBusqueda, ID_TEXTO_BUSQUEDA, "",
        wxDefaultPosition, wxSize(300, -1), wxTE_PROCESS_ENTER);
    textoBusqueda->SetHint(_("Enter category or product name, barcode, or price..."));
    hSizerBusqueda->Add(textoBusqueda, 1, wxEXPAND | wxRIGHT, 10);

    // BotÃÂ³n buscar (NUEVO)
    btnBuscar = new wxButton(panelBusqueda, ID_BUSCAR, _("Search"),
        wxDefaultPosition, wxSize(80, -1), wxBORDER_NONE);
    hSizerBusqueda->Add(btnBuscar, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    // BotÃÂ³n limpiar (YA EXISTÃÂA)
    btnLimpiarBusqueda = new wxButton(panelBusqueda, ID_LIMPIAR_BUSQUEDA, _("Clean"),
        wxDefaultPosition, wxSize(80, -1), wxBORDER_NONE);
    hSizerBusqueda->Add(btnLimpiarBusqueda, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

    // Label para mostrar resultados
    labelResultados = new wxStaticText(panelBusqueda, wxID_ANY, "");
    labelResultados->SetForegroundColour(wxColour(100, 100, 100));
    hSizerBusqueda->Add(labelResultados, 0, wxALIGN_CENTER_VERTICAL);

    panelBusqueda->SetSizer(hSizerBusqueda);

    
    textoBusqueda->Bind(wxEVT_TEXT_ENTER, &ProductosFrame::OnBuscar, this); // Buscar con Enter
    btnBuscar->Bind(wxEVT_BUTTON, &ProductosFrame::OnBuscar, this);         // Buscar con botÃÂ³n
    btnLimpiarBusqueda->Bind(wxEVT_BUTTON, &ProductosFrame::OnLimpiarBusqueda, this);
    

    // Efectos hover (NUEVOS PARA btnBuscar):
    btnBuscar->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnBuscar->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
    btnLimpiarBusqueda->Bind(wxEVT_ENTER_WINDOW, &ProductosFrame::OnButtonEnter, this);
    btnLimpiarBusqueda->Bind(wxEVT_LEAVE_WINDOW, &ProductosFrame::OnButtonLeave, this);
}



