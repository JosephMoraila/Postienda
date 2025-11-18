#include "gui/ProductosFrame/StockDialog/StockDialog.hpp"
#include "utils/window/WindowUtils.h"
#include "utils/MathUtils.hpp"

StockDialog::StockDialog(wxWindow* parent, std::variant<unsigned long long, double> availableStock)
    : wxDialog(parent, wxID_ANY, "Stock", wxDefaultPosition, wxSize(350, 300)),
    m_availableStock(std::move(availableStock))   // guardamos en el miembro
{

	// PRIMERO: Determinar el tipo de producto antes de crear los widgets para informar al usuario si es por peso o por unidad.
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double>) {
            m_productByWeight = true;
            if (arg < 0.0) m_availableStock = 0.0;
        }
        else {
            m_productByWeight = false;
			if (arg < 0ULL) m_availableStock = 0ULL;
        }
    }, m_availableStock);

    Widgets();

    // Mostrar el stock disponible
    CallAfter([this]() {
        FormatavailableStockToValue(m_availableStock);
        });

    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& e) { EndModal(wxID_CANCEL); });
}


void StockDialog::Widgets() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxString ifByWeight = m_productByWeight ? _("\nEnter stock per kilo.") : _("\nEnter stock per unit");
    // Etiqueta
    mainSizer->Add(new wxStaticText(this, wxID_ANY, _("Product stock.\nPlease note that it decreases as purchases are made.") + ifByWeight), 0, wxALL, 5);

    // Horizontal: TextCtrl + SpinButton
    wxBoxSizer* stockSizer = new wxBoxSizer(wxHORIZONTAL);

    txtStock = new wxTextCtrl(this, wxID_ANY, "0");
    stockSizer->Add(txtStock, 1, wxEXPAND | wxALL, 5);

    // SpinButton (solo flechas, sin caja de texto)
    spin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(30, -1), wxSP_VERTICAL);
    stockSizer->Add(spin, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(stockSizer, 0, wxEXPAND | wxALL, 5);

    // Eventos
    spin->Bind(wxEVT_SPIN_UP, &StockDialog::OnSpinUp, this);
    spin->Bind(wxEVT_SPIN_DOWN, &StockDialog::OnSpinDown, this);
    txtStock->Bind(wxEVT_MOUSEWHEEL, &StockDialog::OnMouseWheel, this);
    txtStock->Bind(wxEVT_TEXT, [this](wxCommandEvent&) {
        FormatTextCtrlWithCommas(txtStock);
        });

	wxButton* btnOk = new wxButton(this, wxID_OK, _("Accept"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	mainSizer->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 10);
    btnOk->Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {OnButtonEnter(event, temaOscuro);});
	btnOk->Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {OnButtonLeave(event, temaOscuro);});

    SetSizerAndFit(mainSizer);
}

void StockDialog::OnSpinUp(wxSpinEvent& event) {
    AdjustStock(1);
}

void StockDialog::OnSpinDown(wxSpinEvent& event) {
    AdjustStock(-1);
}

void StockDialog::OnMouseWheel(wxMouseEvent& event) {
    if (event.GetWheelRotation() > 0) AdjustStock(1);
    else AdjustStock(-1);
}

void StockDialog::AdjustStock(double delta) {
    wxString raw = txtStock->GetValue();
    raw.Replace(",", ""); // elimina todas las comas

    if (m_productByWeight) {
        // --- Producto por peso (usa double) ---
        double value = 0.0;
        raw.ToDouble(&value);

        value += delta; 
        if (value < 0) value = 0;

        txtStock->SetValue(FormatFloatWithCommas(value, 3));
    }
    else {
        // --- Producto por piezas (usa entero) ---
        unsigned long long value = 0;
        raw.ToULongLong(&value);

        // delta puede ser double, pero lo forzamos a int
        long long newValue = static_cast<long long>(value) + static_cast<long long>(delta);
        if (newValue < 0) newValue = 0;

        txtStock->SetValue(FormatWithCommas(static_cast<unsigned long long>(newValue)));
    }
}


void StockDialog::FormatTextCtrlWithCommas(wxTextCtrl* ctrl) {
    if (!ctrl) return;

    long pos = ctrl->GetInsertionPoint();
    wxString raw = ctrl->GetValue();

    if (raw.IsEmpty()) return;

    wxString intPart, fracPart;
    int dotPos = raw.Find('.');
    bool hadDot = (dotPos != wxNOT_FOUND);

    if (m_productByWeight && dotPos != wxNOT_FOUND) {
        // --- Producto por peso: permitir decimales ---
		intPart = raw.Left(dotPos); // corta antes del '.'
        fracPart = raw.Mid(dotPos); // incluye el '.'
    }
    else {
        // --- Producto por piezas: CORTAR todo despuÃÂÃÂ©s del punto ---
        if (dotPos != wxNOT_FOUND) {
            intPart = raw.Left(dotPos);  // corta antes del '.'
        }
        else {
            intPart = raw;
        }
        fracPart = ""; // Sin decimales para productos por unidad
    }

    intPart.Replace(",", "");

    wxString formattedInt;
    try {
        if (!intPart.IsEmpty()) formattedInt = FormatWithCommas(std::stoull(std::string(intPart.mb_str())));
        else formattedInt = "";
        
    }
    catch (const std::exception&) {
        formattedInt = intPart;
    }

    wxString formatted = formattedInt + fracPart;

    // Solo aplicar cambio si realmente hay diferencia
    if (formatted != ctrl->GetValue()) {
        ctrl->ChangeValue(formatted);

        // Ajustar posiciÃÂÃÂ³n del cursor
        if (!m_productByWeight && hadDot) {
            // Si cortamos un punto en producto por unidad
            // Mantener el cursor donde estaba, pero no mÃÂÃÂ¡s allÃÂÃÂ¡ del final del texto
            long newPos = pos;
            if (dotPos != wxNOT_FOUND && pos > dotPos) {
                // Si el cursor estaba despuÃÂÃÂ©s del punto, ponerlo al final de la parte entera
                newPos = formatted.Length();
            }
            else {
                // Si el cursor estaba antes o en el punto, mantener posiciÃÂÃÂ³n relativa
                long diff = formatted.Length() - raw.Length();
                newPos = pos + diff;
            }

            // Asegurar que la posiciÃÂÃÂ³n estÃÂÃÂ© dentro del rango vÃÂÃÂ¡lido
            if (newPos < 0) newPos = 0;
            if (newPos > static_cast<long>(formatted.Length())) newPos = formatted.Length();

            ctrl->SetInsertionPoint(newPos);
        }
        else {
            // Comportamiento normal para productos por peso o cuando no hay punto
            long diff = formatted.Length() - raw.Length();
            long newPos = pos + diff;

            if (newPos < 0) newPos = 0;
            if (newPos > static_cast<long>(formatted.Length())) newPos = formatted.Length();

            ctrl->SetInsertionPoint(newPos);
        }
    }
}

void StockDialog::AplicarTema(bool oscuro) {
    temaOscuro = oscuro;

    ApplyTheme(this, oscuro);

    Refresh();
    Update();
}

std::variant<unsigned long long, double> StockDialog::GetStock() const {
    wxString raw = txtStock->GetValue();
    raw.Replace(",", ""); // elimina comas

    if (m_productByWeight) {
        double value = 0.0;
        raw.ToDouble(&value);

        // Redondear a 3 decimales
		value = round3(value);

        return value;
    }
    else {
        unsigned long long value = 0;
        raw.ToULongLong(&value);
        return value;
    }
}

void StockDialog::FormatavailableStockToValue(const std::variant<unsigned long long, double>& availableStock) {
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double>) {
            m_productByWeight = true;
            txtStock->SetValue(FormatFloatWithCommas(arg, 3));
        }
        else {
            m_productByWeight = false;
            txtStock->SetValue(FormatWithCommas(arg));
        }
    }, availableStock);
}
