#include "gui/MainFrame/MainFrame.h"
#include "utils/window/WindowUtils.h"

void MainFrame::MenuBar() {
    wxMenuBar* menuBar = new wxMenuBar;
    SetMenuBar(menuBar);

    // ConfiguraciÃÂ³n
    wxMenu* menuConfiguracion = new wxMenu;
    menuBar->Append(menuConfiguracion, _("Settings"));
    menuConfiguracion->Append(ID_MENU_PRODUCTOS, _("Products"));
    Bind(wxEVT_MENU, &MainFrame::OnProductosClicked, this, ID_MENU_PRODUCTOS);

    menuConfiguracion->Append(ID_MENU_USERS, _("Users"));
    Bind(wxEVT_MENU, &MainFrame::OnUsersClicked, this, ID_MENU_USERS);

    menuConfiguracion->Append(ID_MENU_TICKET, _("Ticket"));
    Bind(wxEVT_MENU, &MainFrame::OnTicketClicked, this, ID_MENU_TICKET);

    menuConfiguracion->Append(ID_MENU_LANGUAGE,_("Language"));
	Bind(wxEVT_MENU, &MainFrame::OnLanguageClicked, this, ID_MENU_LANGUAGE);

    //Ver
    wxMenu* watchMenu = new wxMenu;
    menuBar->Append(watchMenu, _("View"));

    watchMenu->Append(ID_MENU_PREVIOUS_PURCHASES, _("Previous purchases"));
    Bind(wxEVT_MENU, &MainFrame::OnPreviousPurchasesClicked, this, ID_MENU_PREVIOUS_PURCHASES);

    // Apariencia
    wxMenu* menuApariencia = new wxMenu;
    menuBar->Append(menuApariencia, _("Appearance"));

    wxString labelClaro = _("Light");
    wxString labelOscuro = _("Dark");
    wxString labelSistema = _("System");

    menuApariencia->Append(ID_MENU_CLARO, labelClaro);
    menuApariencia->Append(ID_MENU_OSCURO, labelOscuro);
    menuApariencia->Append(ID_SYSTEM_MODE, labelSistema);

    // Vincular eventos de tema
    Bind(wxEVT_MENU, &MainFrame::OnTemaClaro, this, ID_MENU_CLARO);
    Bind(wxEVT_MENU, &MainFrame::OnTemaOscuro, this, ID_MENU_OSCURO);
    Bind(wxEVT_MENU, &MainFrame::ApplySystemMode, this, ID_SYSTEM_MODE);

}

void MainFrame::ActualizarMenuApariencia()
{
    wxMenuBar* bar = GetMenuBar(); // Obtener la barra de menÃÂºs del frame
    if (!bar) return;

    wxMenu* menuApariencia = bar->GetMenu(2); // "Apariencia" es el tercer menÃÂº
    if (!menuApariencia) return;

    wxString labelClaro = _("Light");
    wxString labelOscuro = _("Dark");
    wxString labelSistema = _("System");

    wxString bullet = wxString(L" \u2022");

    if (usarModoSistema) labelSistema += bullet;
    else if (temaOscuro) labelOscuro += bullet;
    else labelClaro += bullet;

    // Actualizar las etiquetas de los elementos del menÃÂº
    menuApariencia->SetLabel(ID_MENU_CLARO, labelClaro);
    menuApariencia->SetLabel(ID_MENU_OSCURO, labelOscuro);
    menuApariencia->SetLabel(ID_SYSTEM_MODE, labelSistema);
}


void MainFrame::Widgets() {
    mainPanel = new wxPanel(this);
    wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hSizerBotones = new wxBoxSizer(wxHORIZONTAL);

    buttonDeleteProducts = new wxButton(mainPanel, ID_BUTTON_DELETE_PRODUCTS, _("Delete"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE);
    buttonDeleteProducts->Enable(false);
    buttonDeleteProducts->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    buttonDeleteProducts->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    hSizerBotones->Add(buttonDeleteProducts, 0, wxALL, 5);
    buttonRealizarCompra = new wxButton(mainPanel, wxID_ANY, _("Make purchase"), wxDefaultPosition, wxSize(150, 40), wxBORDER_NONE);
    buttonRealizarCompra->Enable(false);
    buttonRealizarCompra->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro); });
    buttonRealizarCompra->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro); });
    hSizerBotones->Add(buttonRealizarCompra, 0, wxALL, 5);
    labelTotal = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Total: %.2f", totalUI), wxDefaultPosition, wxDefaultSize);
	hSizerBotones->Add(labelTotal, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    input = new wxTextCtrl(mainPanel, ID_INPUT, wxEmptyString, wxDefaultPosition, wxSize(-1, 30), wxTE_PROCESS_ENTER);
    input->SetToolTip(_("Enter name or barcode"));
    Bind(wxEVT_TEXT_ENTER, &MainFrame::CheckInputLogic, this, ID_INPUT);

    // Crear la lista sin tamaÃÂ±o fijo
    listaProductos = new wxListCtrl(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    listaProductos->Bind(wxEVT_KEY_DOWN, &MainFrame::OnListaKeyDown, this);
    listaProductos->Bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent& event) { // Habilitar botÃÂ³n si hay selecciÃÂ³n en la lista
        if (listaProductos->GetSelectedItemCount() > 0) { // Si hay al menos un elemento seleccionado
            buttonDeleteProducts->Enable(true);
            buttonDeleteProducts->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { // Evento para eliminar productos seleccionados
                std::vector<size_t> IdsCart = ReturnSelectedItemsControlList();
                if (!IdsCart.empty()) DeleteSelectedProductFromCart(IdsCart);
                }, wxID_ANY);
        }
        event.Skip();
        });

    listaProductos->Bind(wxEVT_LIST_ITEM_DESELECTED, [this](wxListEvent& event) {
        if (listaProductos->GetSelectedItemCount() == 0) {
            buttonDeleteProducts->Enable(false); // Deshabilitar si no queda ninguno
        }
        event.Skip();
        });

    // Insertar columnas - usar -1 para que se ajusten automÃÂ¡ticamente al contenido
    // o usar un porcentaje del ancho disponible
    listaProductos->InsertColumn(0, _("Product"), wxLIST_FORMAT_LEFT, 200);
    listaProductos->InsertColumn(1, _("Price"), wxLIST_FORMAT_LEFT, 100);
    listaProductos->InsertColumn(2, _("Barcode"), wxLIST_FORMAT_LEFT, 100);
    listaProductos->InsertColumn(3, _("ID"), wxLIST_FORMAT_LEFT, 0); // ancho 0: oculta

    vSizer->Add(hSizerBotones, 0, wxEXPAND | wxALL, 5); //Botones se ponen arriba

    // El input se expande horizontalmente y va despues de los botones
    vSizer->Add(input, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    // La lista se expande en ambas direcciones
    vSizer->Add(listaProductos, 1, wxEXPAND | wxALL, 10);

    mainPanel->SetSizer(vSizer);

    // Sizer principal para el frame
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(mainPanel, 1, wxEXPAND);

    SetSizer(mainSizer);
    SetMinSize(wxSize(600, 400));

    vSizer->SetSizeHints(this);
}

void MainFrame::UpdateButtonRealizarCompra() {
    if (listaProductos->GetItemCount() > 0) {
        buttonRealizarCompra->Enable(true);
        buttonRealizarCompra->Bind(wxEVT_BUTTON, &MainFrame::OnRealizarCompra, this, wxID_ANY);
    }
    else {
        buttonRealizarCompra->Enable(false);
        buttonRealizarCompra->Unbind(wxEVT_BUTTON, &MainFrame::OnRealizarCompra, this, wxID_ANY);
    }
}

void MainFrame::AjustarColumnasListCtrl() {
    if (listaProductos) {
        wxSize clientSize = listaProductos->GetClientSize();
        int totalWidth = clientSize.GetWidth();

        if (totalWidth > 0) {
            int productoWidth = 0;
            int precioWidth = 0;
            int codigoWidth = 0;

            // Restar ancho del scrollbar si existe
            wxSize virtualSize = listaProductos->GetVirtualSize();
            if (virtualSize.GetHeight() > clientSize.GetHeight()) totalWidth -= wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

            if (!IsMaximized()) {
                // Distribuir proporcionalmente en modo normal
                productoWidth = static_cast<int>(totalWidth * 0.50);
                precioWidth = static_cast<int>(totalWidth * 0.25);
                codigoWidth = totalWidth - productoWidth - precioWidth;
            }
            else {
                // Distribuir proporcionalmente en modo maximizado
                productoWidth = static_cast<int>(totalWidth * 0.50);
                precioWidth = static_cast<int>(totalWidth * 0.20);
                codigoWidth = totalWidth - productoWidth - precioWidth;
            }

            // Aplicar anchos mÃÂ­nimos SIEMPRE
            productoWidth = wxMax(150, productoWidth);
            precioWidth = wxMax(80, precioWidth);
            codigoWidth = wxMax(100, codigoWidth);

            listaProductos->SetColumnWidth(0, productoWidth);
            listaProductos->SetColumnWidth(1, precioWidth);
            listaProductos->SetColumnWidth(2, codigoWidth);
        }
    }
}