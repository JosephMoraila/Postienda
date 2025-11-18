#pragma once
#include <wx/wx.h>

class TextDialog : public wxDialog {
public:
    TextDialog(wxWindow* parent, const wxString& title = _("Add text"));
    void AplicarTema(bool oscuro); ///< Aplica el tema claro u oscuro a la ventana y todos sus widgets hijos.
    wxString GetText() const;
    void SetText(wxString text = "") const;
	bool GetIsUnderInfo() const;
	void SetIsUnderInfo(bool underInfo);

private:
    wxPanel* panel;
    wxCheckBox* checkboxUnderInfo;
    wxStdDialogButtonSizer* btnSizer;
    wxPanel* buttonPanel;
    wxBoxSizer* mainSizer;
    wxTextCtrl* textCtrl;
    bool temaOscuro = false; ///< Indica si el tema oscuro estÃ¡ activo.
};
