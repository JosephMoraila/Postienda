#include "gui/ProductosFrame/ChangeProductDialog/ChangeProductDialog.h"
#include "utils/window/WindowUtils.h"
#include "gui/ProductosFrame/ProductosFrame.h"
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"

ChangeProductDialog::ChangeProductDialog(wxWindow* parent,Producto& product, const wxTreeItemId& parentId, ProductosFrame* parentFrame, const std::shared_ptr<Categoria>& fatherCategory, std::shared_ptr<Producto>& prod)
	: ProductoDialog(parent, parentId, parentFrame), m_product(product), m_fatherCategory(fatherCategory), m_parentFrame(parentFrame), m_item(parentId), //El parentId es un nombre simbolico del item que realiad se esta modificando
	m_prod(prod)
{
	AplicarIconoPrincipal(this);
	SetTitle(_("Edit Product"));
	//Widgets(); No hace falta llamar a Widgets porque ya se llama en el constructor de ProductoDialog
    // Ocultar el campo de Stock que no se usa en edición
    if (txtStockInicial && stockLabel) {
        txtStockInicial->Hide();
        stockLabel->Hide();
        Layout();
        Fit();
    }
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

void ChangeProductDialog::OnAceptar(wxCommandEvent& event) {
    // Validar nombre
    std::string nombreProducto = GetNombre().ToUTF8().data();
    std::string nombreLimpiado = LimpiarCaracteresInvalidosOnAddProduct(nombreProducto);
    std::string nombreLimpio = LimpiarYValidarNombre(nombreLimpiado);

    if (nombreLimpio.empty()) {
        wxMessageBox(_("The product name cannot be empty or contain only spaces."), WARNING_MESSAGES::WARNING, wxOK | wxICON_WARNING);
        return;
    }

    txtNombre->SetValue(nombreLimpio);

    // Validar precio
    wxString raw = txtPrecio->GetValue();
    raw.Replace(",", "");   // quitar separadores de miles
    std::string s = raw.ToStdString();
    double precio = 0.0;
    try {
        precio = std::stod(s);
    }
    catch (...) {
        wxMessageBox(_("The price must be a number greater than or equal to zero."), WARNING_MESSAGES::WARNING, wxOK | wxICON_WARNING);
        return;
    }
    if (precio < 0) {
        wxMessageBox(_("The price must be a number greater than or equal to zero."), WARNING_MESSAGES::WARNING, wxOK | wxICON_WARNING);
        return;
    }
    txtPrecio->SetValue(wxString::Format("%.2f", precio)); // Formatear a 2 decimales


    wxString codigoBarrasWx = GetCodigoBarras();
    std::string codigoBarras;

    if (!codigoBarrasWx.IsEmpty()) {
        codigoBarras = LimpiarCodigoBarras(codigoBarrasWx.ToStdString());

        txtCodigoBarras->SetValue(wxString::FromUTF8(codigoBarras));

        // Validar longitud
        if (codigoBarras.length() > 20) {
            wxMessageBox(_("The barcode cannot have more than 20 characters."), WARNING_MESSAGES::INVALID_CODEBAR, wxOK | wxICON_WARNING);
            return;
        }

        // Validar que no estÃÂ© vacÃÂ­o despuÃÂ©s de limpiar
        if (codigoBarras.empty()) {
            wxMessageBox(_("The barcode must contain at least one number or letter."), WARNING_MESSAGES::INVALID_CODEBAR, wxOK | wxICON_WARNING);
            return;
        }

        // Actualizar el campo con el cÃÂ³digo limpio
        txtCodigoBarras->SetValue(wxString::FromUTF8(codigoBarras));
    }

    nombreProducto = GetNombre().ToUTF8().data();
    codigoBarrasWx = GetCodigoBarras();
    double precioWithoutRound = GetPrecio();
    precio = round2(precioWithoutRound);
    bool esPorPeso = EsPorPeso();
    size_t idCategoriaPadre = m_fatherCategory->idCategoria; //Nesesario para la base de datos para indicar que categorÃÂ­a es padre del producto

    nombreLimpiado = LimpiarCaracteresInvalidosOnAddProduct(nombreProducto);
    nombreLimpio = LimpiarYValidarNombre(nombreLimpiado);
    if (!EsNombreValido(nombreLimpio)) {
        wxMessageBox(_("The product name is invalid. It must contain at least one valid character."), _("Invalid name"), wxOK | wxICON_WARNING);
        return;
    }
    nombreLimpio = TruncarNombre(nombreLimpio, 80);
    wxString nombreFinal = wxString::FromUTF8(nombreLimpio);
    // Validar si ya existe un producto con el mismo nombre en esta categorÃÂ­a
    wxTreeItemId CategoriaSeleccionada = m_parentFrame->arbolCategorias->GetItemParent(m_item);


    //La compraciÃÂ³n de prod.nombre != nombreLimpio es para evitar que se marque como duplicado si el nombre no ha cambiado
    if (m_parentFrame->ExistsProductNameInSameCategory(nombreFinal, idCategoriaPadre) && m_prod->nombre != nombreLimpio) {
        wxMessageBox(_("There is already a product with that name in this category."), _("Duplicate Product"), wxOK | wxICON_WARNING);
        return;
    }

    if (!codigoBarrasWx.IsEmpty()) {
        codigoBarras = LimpiarCodigoBarras(codigoBarrasWx.ToStdString());

        // Validar que el cÃÂ³digo de barras sea ÃÂºnico globalmente
        if (!codigoBarras.empty() && m_parentFrame->ExistsCodebarGlobal(codigoBarras) && m_prod->codigoBarras != codigoBarras) {
            wxMessageBox(wxString::Format(_("The barcode '%s' is already in use by another product."), wxString::FromUTF8(codigoBarras)), _("Duplicate Product"), wxOK | wxICON_WARNING);
            return;
        }
    }

    Producto oldProductoInfo = *m_prod;

    //Asiganar los valores al producto
    m_prod->nombre = nombreLimpio;
    m_prod->codigoBarras = codigoBarras;
    m_prod->precio = precio;
    m_prod->porPeso = esPorPeso;
    // Actualizar en la base de datos 
    m_parentFrame->UpdateProductInDB(oldProductoInfo, m_prod, idCategoriaPadre);

    wxString showFormatUpdatedProduct = wxString::Format("- %s (%s)", wxString::FromUTF8(m_prod->nombre), FormatFloatWithCommas(m_prod->precio));
    if (!m_prod->codigoBarras.empty()) showFormatUpdatedProduct += wxString::Format(" [%s]", wxString::FromUTF8(m_prod->codigoBarras));
    m_parentFrame->arbolCategorias->SetItemText(m_item, showFormatUpdatedProduct);
    EndModal(wxID_OK);
}
