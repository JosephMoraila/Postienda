#include "gui/ProductosFrame/ChangeProductDialog/ChangeProductDialog.h"
#include "utils/window/WindowUtils.h"

ChangeProductDialog::ChangeProductDialog(wxWindow* parent,Producto& product)
    : ProductoDialog(parent), m_product(product)
{
	AplicarIconoPrincipal(this);
	SetTitle(_("Edit Product"));
	//Widgets(); No hace falta llamar a Widgets porque ya se llama en el constructor de ProductoDialog
	EstablecerDatos();
}

void ChangeProductDialog::EstablecerDatos() {
	wxString nombreProducto = wxString::FromUTF8(m_product.nombre);
	txtNombre->SetValue(nombreProducto);

	double precioProducto = m_product.precio;
	txtPrecio->SetValue(wxString::Format("%.2f", precioProducto));

	std::string codigoBarrasProducto = m_product.codigoBarras;
	txtCodigoBarras->SetValue(codigoBarrasProducto);

	bool esPorPesoProducto = m_product.porPeso;
	chkPorPeso->SetValue(esPorPesoProducto);

	/*Se necesita poner el if porque si se es True no se pone el precio por kilo y en negrita, asÃÂ­ que con el if se hace vÃÂ¡lido*/
	if(esPorPesoProducto) {
		precioLabel->SetLabel(_("Price per kilo:"));
		wxFont font = precioLabel->GetFont();
		font.SetWeight(wxFONTWEIGHT_BOLD);
		precioLabel->SetFont(font);
	}
	else {
		precioLabel->SetLabel(_("Price:"));
		wxFont font = precioLabel->GetFont();
		font.SetWeight(wxFONTWEIGHT_NORMAL);  // quita la negrita
		precioLabel->SetFont(font);
	}
}