#include "gui/MainFrame/MainFrame.h"
#include "gui/UsuariosFrame/UsuariosFrame.h"
#include "utils/FuenteUtils.h"
#include "utils/GetFromFile.h"
#include "gui/AskCreatePasswordFrame/AskCreatePasswordFrame.hpp"
#include "gui/AskEnterPasswordFrame/AskEnterPasswordFrame.hpp"
#include <filesystem>
#include "constants/PASSWORD/PASSWORD_FILE.hpp"

//MENU APARIENCIA

void MainFrame::OnTemaClaro(wxCommandEvent& event) {
    temaOscuro = false;
    usarModoSistema = false;
    GuardarConfiguracion();
    AplicarTemaActual();
    ActualizarMenuApariencia();
}

void MainFrame::OnTemaOscuro(wxCommandEvent& event) {
    temaOscuro = true;
    usarModoSistema = false;
    GuardarConfiguracion();
    AplicarTemaActual();
    ActualizarMenuApariencia();
}

void MainFrame::ApplySystemMode(wxCommandEvent& event)
{
    // marcar que ahora la app debe seguir al sistema
    usarModoSistema = true;

    // Aplicar sin tocar temaOscuro (guardamos en JSON quÃÂ© opciÃÂ³n estÃÂ¡ activa)
    GuardarConfiguracion();
    AplicarTemaActual();
    ActualizarMenuApariencia();
}


//MENU CONFIGURATION

void MainFrame::OnProductosClicked(wxCommandEvent& event)
{
    // Verifica si la ventana de productos ya estÃÂ¡ abierta
    if (productosVentana && productosVentana->IsShown()) {
        // Si la ventana ya estÃÂ¡ abierta, la activa y la muestra
        if (productosVentana->IsIconized()) {
            // Si la ventana estÃÂ¡ minimizada, la restaura
            productosVentana->Iconize(false);
        }
        productosVentana->Show();
        productosVentana->Raise();
        productosVentana->SetFocus();
    }
    else {
        wxString mensaje = _("To access products, you need to empty your cart. Do you want to empty it and access?");
        bool open = DeleteCartProductsAsk(mensaje);
        if (!open) return;

        //Pedir password o crearla:
        bool succcess = PasswordWindow();
        if (!succcess) return;

        productosVentana = new ProductosFrame(this); // Crea una nueva instancia de ProductosFrame
        productosVentana->AplicarTema(temaOscuro); // Aplica el tema actual llamando a la funciÃÂ³n de ProductosFrame
        productosVentana->Show(); // Muestra la ventana de productos
        // Vincula el evento de cierre de la ventana para limpiar el puntero, el primer parÃÂ¡metro es el evento y el segundo es una lambda que se ejecuta cuando se cierra la ventana
        productosVentana->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            productosVentana = nullptr;
            evt.Skip();
            });
    }
}

bool MainFrame::PasswordWindow() {

    if (FileExists(GetPasswordPath())) {
        AskEnterPasswordFrame dlg(this);
        dlg.AplicarTema(temaOscuro);
        int result = dlg.ShowModal();
        //Solo procede si el usuario presiona ok y todo salio bien
        if (result == wxID_OK) return true;
        //User pressed X or canceled
        else if (result == wxID_CANCEL) return false;
        //Failed, puede ser wxID_ABORT u otro
        else {
            wxMessageBox(_("Error verifying password"), _("Error"), wxOK | wxICON_ERROR);
            return false;
        }
    }

    else { //If not exists ask user for password

        AskCreatePasswordFrame dlg(this);
        dlg.AplicarTema(temaOscuro);
        int result = dlg.ShowModal();

        //Solo procede si el usuario presiona ok y todo salio bien
        if (result == wxID_OK) return true;
        //User pressed X or canceled
        else if (result == wxID_CANCEL) return false;
        //Failed
        else {
            wxMessageBox(_("There was an error creating the password, try again."), _("Error"), wxOK | wxICON_ERROR);
            return false;
        }
       
    }
}


void MainFrame::OnUsersClicked(wxCommandEvent& event) {
    if (usuariosVentana && usuariosVentana->IsShown()) {
        if (usuariosVentana->IsIconized()) {
            usuariosVentana->Iconize(false);
        }
        usuariosVentana->Show();
        usuariosVentana->Raise();
        usuariosVentana->SetFocus();
    }
    else {
        usuariosVentana = new UsuariosFrame(this, this);
        usuariosVentana->AplicarTema(temaOscuro);
        usuariosVentana->Show();
        usuariosVentana->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            usuariosVentana = nullptr;
            evt.Skip();
            });
    }
}

void MainFrame::OnTicketClicked(wxCommandEvent& event) {
    if (ticketVentana && ticketVentana->IsShown()) {
        if (ticketVentana->IsIconized()) {
            ticketVentana->Iconize(false);
        }
        ticketVentana->Show();
        ticketVentana->Raise();
        ticketVentana->SetFocus();
    }
    else {
        ticketVentana = new TicketFrame(this);
        ticketVentana->AplicarTema(temaOscuro);
        ticketVentana->Show();
        ticketVentana->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            ticketVentana = nullptr;
            evt.Skip();
            });
    }
}

void MainFrame::OnLanguageClicked(wxCommandEvent& event) {
    if(LanguageWindow && LanguageWindow->IsShown()) {
        if (LanguageWindow->IsIconized()) {
            LanguageWindow->Iconize(false);
        }
        LanguageWindow->Show();
        LanguageWindow->Raise();
        LanguageWindow->SetFocus();
    }
    else {
        LanguageWindow = new LanguageFrame(this);
        LanguageWindow->AplicarTema(temaOscuro);
        LanguageWindow->ShowModal();
        LanguageWindow->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            LanguageWindow = nullptr;
            evt.Skip();
            });
	}
}

//MENU WATCH
void MainFrame::OnPreviousPurchasesClicked(wxCommandEvent& event) {
    if (previousPurchasesVentana && previousPurchasesVentana->IsShown()) {
        if (previousPurchasesVentana->IsIconized()) {
            previousPurchasesVentana->Iconize(false);
        }
        previousPurchasesVentana->Show();
        previousPurchasesVentana->Raise();
        previousPurchasesVentana->SetFocus();
    }
    else {
        previousPurchasesVentana = new PreviousPurchaseFrame(this);
        previousPurchasesVentana->AplicarTema(temaOscuro);
        previousPurchasesVentana->Show();
        previousPurchasesVentana->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            previousPurchasesVentana = nullptr;
            evt.Skip();
            });
    }
}

void MainFrame::onInfoProductsClicked(wxCommandEvent& event) {
    if (infoProductsVentana && infoProductsVentana->IsShown()) {
        if (infoProductsVentana->IsIconized()) {
            infoProductsVentana->Iconize(false);
        }
        infoProductsVentana->Show();
        infoProductsVentana->Raise();
        infoProductsVentana->SetFocus();
    }
    else {
        infoProductsVentana = new InfoProductsFrame(this);
        infoProductsVentana->AplicarTema(temaOscuro);
        infoProductsVentana->Show();
        infoProductsVentana->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& evt) {
            infoProductsVentana = nullptr;
            evt.Skip();
            });
    }
}

//TABLE PRODUCTS

void MainFrame::OnListaKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == 'E' || event.GetKeyCode() == WXK_BACK) {
        std::vector<size_t> IdsCart = ReturnSelectedItemsControlList();
        if (!IdsCart.empty()) {
            wxString mensaje = (IdsCart.size() == 1) ? _("Do you want to delete the selected item?") : _("Do you want to delete the selected items?");
            int respuesta = wxMessageBox(mensaje, _("Confirm deletion"), wxYES_NO | wxICON_QUESTION);
            if (respuesta == wxYES) DeleteSelectedProductFromCart(IdsCart);
        }
    }
    event.Skip();
}


void MainFrame::OnRealizarCompra(wxCommandEvent& event) {
    try {
        // Enum para la lÃÂ³gica interna (no traducible)
        enum MetodoPago { EFECTIVO, TARJETA };

        CreateComprasTable();

        // Opciones traducidas para mostrar al usuario
        wxArrayString opciones;
        opciones.Add(_("Cash"));
        opciones.Add(_("Card"));

        // Mostrar diÃÂ¡logo de elecciÃÂ³n
        wxSingleChoiceDialog metodoPagoDialog(
            this,
            _("Select payment method:"),
            _("Payment method"),
            opciones
        );

        if (metodoPagoDialog.ShowModal() != wxID_OK) return; // Cancelado

        // Importante: usar ÃÂ­ndice, no el texto traducido
        int seleccion = metodoPagoDialog.GetSelection();
        bool esEfectivo = (seleccion == EFECTIVO);

        // Registrar compra y calcular total
        double total = AddCompraToDB();
        double cambio = 0.0;
        double pagoCliente = 0.0;

        if (esEfectivo) {
            bool valido = false;
            do {
                wxString pagoStr = wxGetTextFromUser(wxString::Format(_("Total: $%.2f\n\nEnter amount received:"), total), _("Cash payment"));
                if (pagoStr.IsEmpty()) return; // Si presiona cancelar
                if (pagoStr.ToDouble(&pagoCliente))valido = true;
                else wxMessageBox(_("Invalid amount, please try again."), _("Error"), wxOK | wxICON_ERROR);

            } while (!valido);

            cambio = pagoCliente - total; // puede ser negativo si paga parcialmente
        }

        // Mensaje de confirmaciÃÂ³n traducible
        wxString mensaje;
        if (esEfectivo) mensaje = wxString::Format(_("Purchase registered successfully.\nTotal: $%.2f\nPaid: $%.2f\nChange: $%.2f"),total, pagoCliente, cambio);
        else mensaje = wxString::Format(_("Purchase registered successfully.\nTotal: $%.2f\nPaid with card."),total);

        // Imprimir ticket y mostrar resultado
        PrintTicket(pagoCliente, esEfectivo);
        wxMessageBox(mensaje, _("Purchase completed"), wxOK | wxICON_INFORMATION);
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format(_("Error creating table: %s"), e.what()),_("Error"),wxOK | wxICON_ERROR);
    }

    // Restaurar estado de UI
    UpdateButtonRealizarCompra();
    totalUI = 0.0;
    labelTotal->SetLabel(wxString::Format(_("Total: %.2f"), totalUI));
}


//UTILS BINDS:

void MainFrame::OnSize(wxSizeEvent& event) {
    event.Skip(); // deja que wxWidgets ajuste la ventana
    CallAfter([this]() { // CallAfter para asegurar que la ventana ya se haya redimensionado completamente
        AjustarColumnasListCtrl();
        });
    AjustarFuenteSegunAlto(this, button, nullptr, mainPanel);
}

void MainFrame::OnClose(wxCloseEvent& event) {
    wxString mensaje = _("You have products in your cart.\nDo you want to close? Cart items will be deleted.");
    bool close = DeleteCartProductsAsk(mensaje);
    if (!close) return;
    Destroy();
}