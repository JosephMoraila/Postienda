#include "utils/GetFromFile.h"
#include "gui/MainFrame/MainFrame.h"
#include "utils/ValidateStringInput.h"
#include "constants/DB/DB.h"
#include "constants/MESSAGES_ADVICE/WARNING/ATENCION.hpp"
#include "constants/MESSAGES_ADVICE/INFO/NO_PRODUCT.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"

void MainFrame::CheckInputLogic(wxCommandEvent& event) {
    if (productosVentana != nullptr) {
        wxMessageBox(_("Close the windows Products before continuing"), ATENCION, wxOK | wxICON_INFORMATION);
        input->Clear();
        return;
	}
    wxString texto = input->GetValue();
    wxString cleanName = LimpiarYValidarNombreWx(texto);

    bool tieneLetra = false;
    bool tieneNumero = false;

    for (wxUniChar ch : cleanName) {
        if (wxIsalpha(ch)) tieneLetra = true;
        else if (wxIsdigit(ch)) tieneNumero = true;
    }

    std::string terminoUtf8 = std::string(cleanName.utf8_str());

    if (tieneLetra) {
        size_t idSeleccionado = SeleccionarProductoPorNombre(terminoUtf8);
        if (idSeleccionado != 0) {
            // Crear un optional con el producto encontrado
            m_lastSelectedProduct = GetProductFromDBbyId(idSeleccionado);
        }
        else {
            // Usuario cancelÃÂÃÂ³ o no se encontrÃÂÃÂ³
            input->Clear();
            return;
        }
    }
    else if (tieneNumero) {
        m_lastSelectedProduct = GetProductFromDBbyBarcode(terminoUtf8);
        // Verificar si el optional tiene valor
        if (!m_lastSelectedProduct.has_value()) {
            wxMessageBox(_("That product with that barcode does not exist."), "Error", wxOK | wxICON_ERROR);
            input->Clear();
            return;
        }
    }
    else return;

    // Verificar que tenemos un producto vÃÂÃÂ¡lido antes de usarlo
    if (m_lastSelectedProduct.has_value()) {
        // Acceder al valor del optional usando .value() o *
		if (m_lastSelectedProduct->porPeso) CalculatePriceWeight(); // Si es por peso, pedir peso y calcular precio
		else { // Si no es por peso, aÃÂÃÂ±adir directamente
            wxString nombreCompleto = wxString::FromUTF8(m_lastSelectedProduct->nombre.c_str());
            double precioFinal = m_lastSelectedProduct->precio;
            m_lastSelectedProduct->setCantidad(1);
            AddProductToListCtrl(nombreCompleto, precioFinal);
        }
    }
    input->Clear();
}

void MainFrame::AddProductToListCtrl(wxString nombreCompleto, double precioFinal) {

    long index = listaProductos->InsertItem(listaProductos->GetItemCount(), nombreCompleto);
    listaProductos->SetItem(index, 1, wxString::Format("$%s", FormatFloatWithCommas(precioFinal)));
    listaProductos->SetItem(index, 2, m_lastSelectedProduct->codigoBarras);
    CreateCartTable();
	size_t lastIdProduct = m_lastSelectedProduct->id; double lastQuantity = m_lastSelectedProduct->cantidad;
    AddToCart(lastIdProduct, lastQuantity);
    size_t lastIdRow = m_lastSelectedProduct->id;
	listaProductos->SetItem(index, 3, wxString::Format("%zu", lastIdRow)); // columna oculta con el id del producto en el carrito
	totalUI += precioFinal;
    labelTotal->SetLabel(wxString::Format("Total: $%s", FormatFloatWithCommas(totalUI)));
    UpdateButtonRealizarCompra();
}

//GET PRODUCT:

void MainFrame::CalculatePriceWeight() {
    // Claves internas, no traducibles
    enum UnidadPeso { KILOS, GRAMOS };

    // Opciones visibles traducibles
    wxArrayString opciones;
    opciones.Add(_("Kilos"));
    opciones.Add(_("Grams"));

    // DiÃÂÃÂ¡logo traducible
    wxSingleChoiceDialog unidadDialog(this,_("Do you want to enter the amount in kilograms or grams?"), _("Unit of measurement"),opciones);

    if (unidadDialog.ShowModal() == wxID_OK) {
        int seleccion = unidadDialog.GetSelection();

        // Determinar la unidad con ÃÂÃÂ­ndice, no con texto traducido
        bool esKilos = (seleccion == KILOS);

        // Texto del cuadro traducible y dependiente de unidad
        wxString prompt = esKilos ? _("Enter the quantity in kilograms (example: 1.5):") : _("Enter the amount in grams (example: 750):");

        wxTextEntryDialog cantidadDialog(this, prompt, _("Quantity"));

        if (cantidadDialog.ShowModal() == wxID_OK) {
            double cantidad = 0.0;
            if (cantidadDialog.GetValue().ToDouble(&cantidad) && cantidad > 0) {

                double cantidadKilos = 0.0;
                wxString cantidadTexto;

                if (esKilos) {
                    cantidadKilos = cantidad;
                    cantidadTexto = wxString::Format(_(" (%.3f kg)"), cantidadKilos);
                }
                else {
                    cantidadKilos = cantidad / 1000.0;
                    cantidadTexto = wxString::Format(_(" (%.3f g)"), cantidad);
                }

                // Guardar la cantidad real en el producto
                m_lastSelectedProduct->setCantidad(cantidadKilos);

                // Calcular precio
                double precioFinal = m_lastSelectedProduct->precio * cantidadKilos;
                m_lastSelectedProduct->precio = precioFinal;

                wxString nombreCompleto = wxString::FromUTF8(m_lastSelectedProduct->nombre.c_str()) + cantidadTexto;

                AddProductToListCtrl(nombreCompleto, precioFinal);
            }
            else {
                wxMessageBox(_("Invalid quantity"), _("Error"), wxOK | wxICON_ERROR);
            }
        }
    }
}


//GET PRODUCT BY BARCODE:

std::optional<ProductInfo> MainFrame::GetProductFromDBbyBarcode(const std::string& barcodeToFind) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        std::optional<ProductInfo> result;

        db << "SELECT id, name, price, barcode, byWeight "
            "FROM products "
            "WHERE barcode = ?;"
            << barcodeToFind
            >> [&](size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, unsigned char porPeso) {
            // Llenamos el optional solo si hay resultado
            result = ProductInfo(id, nombre, precio, codigoBarras, porPeso);
            };

        return result; // devolvemos el optional
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(NO_PRODUCT, _("Info"), wxOK | wxICON_INFORMATION);
        return std::nullopt;
    }

    catch (std::exception& e) {
        wxMessageBox(_("Error obtaining products: ") + e.what(), "Error", wxOK | wxICON_ERROR);
        return std::nullopt; // Devolver nullopt en caso de error
	}
}


//GET PRODUCT BY NAME:

ProductInfo MainFrame::GetProductFromDBbyId(size_t productId) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        ProductInfo Product; 
        db << "SELECT id, name, price, barcode, byWeight "
            "FROM products "
            "WHERE id = ?;"
            << productId
            >> [&](size_t id, std::string nombre, double precio, std::string codigoBarras, unsigned char porPeso) {
            Product = ProductInfo(id, nombre, precio, codigoBarras, porPeso);
            };

        return Product;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(NO_PRODUCT, _("Info"), wxOK | wxICON_INFORMATION);
        return ProductInfo();
    }
    catch (std::exception& e) {
        wxMessageBox(_("Error obtaining products: ") + e.what(), "Error", wxOK | wxICON_ERROR);
        return ProductInfo(); // Devolver un objeto vacÃÂÃÂ­o en caso de error
    }
}


std::vector<ProductInfo> MainFrame::GetProductFromDBbyName(const std::string& productName) {
    std::vector<ProductInfo> resultados;

    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        db << "SELECT p.id, p.name, p.price, p.barcode, p.byWeight, c.name "
            "FROM products p "
            "JOIN categories c ON p.category_id = c.id "
            "WHERE p.name = ?;"
            << productName
            >> [&](size_t id, std::string nombre, double precio,std::string codigoBarras, unsigned char porPeso, std::string categoria) {
                    resultados.push_back({ id, nombre, precio, codigoBarras, porPeso != 0, categoria });
            };
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(NO_PRODUCT, _("Info"), wxOK | wxICON_INFORMATION);
    }
    catch (std::exception& e) {
        wxMessageBox(_("Error obtaining products: ") + e.what(), "Error", wxOK | wxICON_ERROR);
    }

    return resultados;
}

size_t MainFrame::SeleccionarProductoPorNombre(const std::string& productName) {
    auto productos = GetProductFromDBbyName(productName);

    if (productos.empty()) {
        wxMessageBox(_("No products were found with that name.."), _("Advice"),wxOK | wxICON_INFORMATION);
        return 0;
    }

    if (productos.size() == 1) {
        auto& prod = productos[0];
        return prod.id;
    }

    // Hay mÃÂÃÂ¡s de un producto con el mismo nombre ÃÂ¢ÃÂÃÂ pedir elecciÃÂÃÂ³n
    wxArrayString opciones;
    for (auto& p : productos) {
        wxString opcion;
        opcion << wxString::FromUTF8(p.nombre)
            << _(" - Category: ") << wxString::FromUTF8(p.categoria)
            << " - $" << wxString::Format("%.2f", p.precio)
            << _(" - Barcode: ") << p.codigoBarras;
        opciones.Add(opcion);
    }

    int seleccion = wxGetSingleChoiceIndex(_("Multiple products with that name were found.\nChoose one:"), _("Select product"),opciones);

    if (seleccion != -1) {
        auto& elegido = productos[seleccion];
        return elegido.id;
    }

    return 0; // usuario cancelÃÂÃÂ³
}

//GET PRODUCT BY NAME FINISHES
//GET PRODUCT FINISHES

//ADD/DELETE PRODUCT TO CART:

void MainFrame::CreateCartTable() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
		db << "PRAGMA foreign_keys = ON;";
        // Crear tabla temporal para el carrito si no existe
        db << "CREATE TABLE IF NOT EXISTS cart ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "product_id INTEGER NOT NULL, "   
            "quantity REAL NOT NULL DEFAULT 1.000 CHECK(quantity = ROUND(quantity, 3)), "
            "FOREIGN KEY(product_id) REFERENCES products(id) ON DELETE CASCADE"
            ");";

    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error creating cart table ") + e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

void MainFrame::AddToCart(size_t& productId, double& qty) {
    try {
        sqlite::database db(GetDBPath());
        db << "INSERT INTO cart (product_id, quantity) VALUES (?, ?); "
            << productId
            << qty;

        size_t rowid = 0;
        db << "SELECT last_insert_rowid();" >> rowid;
		m_lastSelectedProduct->id = rowid;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error adding product to cart:") + e.what(), "Error", wxOK | wxICON_ERROR);
    }
}

bool MainFrame::DeleteCartProductsAsk(wxString& mensaje) {
    try {
        sqlite::database db(GetDBPath());
        // Verificar primero si la tabla existe
        int tableExists = 0;
        try {
            db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='cart';" >> tableExists;
            if (tableExists == 0) {
                totalUI = 0.0; 
				labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
                return true;  // Tabla no existe, retornar ÃÂÃÂ©xito sin mostrar error
            }
        }
        catch (const std::exception&) {
            totalUI = 0.0;
            labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
            return true;  // Si falla la verificaciÃÂÃÂ³n, asumir que no existe
        }

        // La tabla existe, proceder normalmente
        int count = 0;
        db << "SELECT COUNT(*) FROM cart;" >> count;
        if (count == 0) {
            totalUI = 0.0;
            labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
            return true;
        }
        wxMessageDialog confirmDialog(this,mensaje, _("Confirm"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (confirmDialog.ShowModal() == wxID_YES) {
            try{
                DeleteCartProducts();
                totalUI = 0.0;
                labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
                return true;
            }catch (const std::exception& e) {
                wxMessageBox(wxString::Format(_("Error emptying cart: %s"), e.what()), "Error", wxOK | wxICON_ERROR);
                return false;
            }
        } return false;

    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(wxString::Format(_("SQL error while emptying the cart\n\nCode: %d\nMessage: %s"), e.get_code(), e.what()),"Error SQL",wxOK | wxICON_ERROR);
        return false;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error: %s", e.what()),"Error",wxOK | wxICON_ERROR);
        return false;
    }
}

void MainFrame::DeleteCartProducts() {
    sqlite::database db(GetDBPath());
    db << "DELETE FROM cart;";
    db << "DELETE FROM sqlite_sequence WHERE name='cart';"; // reinicia IDs
    listaProductos->DeleteAllItems();
}

void MainFrame::DeleteSelectedProductFromCart(const std::vector<size_t>& v) {
    if (v.empty()) return;
    try {
        for (size_t i = 0; i < v.size(); ++i) {
            double price = GetItemPriceById(v[i]);
			totalUI -= price;
        }
        sqlite::database db(GetDBPath());
        // Construir la clÃÂÃÂ¡usula IN: (?, ?, ?, ...)
        std::string placeholders;
        for (size_t i = 0; i < v.size(); ++i) placeholders += (i == 0 ? "?" : ",?");
        auto query = "DELETE FROM cart WHERE id IN (" + placeholders + ");";
		auto stmt = db << query; //stmt es de tipo sqlite::statement, se guarda la consulta preparada para ejecutarla luego varias veces
		for (size_t id : v) stmt << id; // enlaza cada id al placeholder correspondiente
        stmt.execute();
		//Delete the items from the list control
        for (long i = listaProductos->GetItemCount() - 1; i >= 0; --i) {
            size_t id = static_cast<size_t>(std::stoul(listaProductos->GetItemText(i, 3).ToStdString()));
            if (std::find(v.begin(), v.end(), id) != v.end()) listaProductos->DeleteItem(i);
        }
        UpdateButtonRealizarCompra();
		labelTotal->SetLabel(wxString::Format("Total: $%s", FormatFloatWithCommas(totalUI)));
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
}

double MainFrame::GetItemPriceById(const size_t searchId) {
    for (long i = 0; i < listaProductos->GetItemCount(); i++) {
        // Obtener el ID de la columna 3 (columna oculta)
        size_t rowId = static_cast<size_t>(std::stoul(listaProductos->GetItemText(i, 3).ToStdString()));
        if (rowId == searchId) {
            // Obtener el texto de la columna 1 (precio)
            wxString priceText = listaProductos->GetItemText(i, 1);
            // Remover el sÃÂÃÂ­mbolo '$' y cualquier coma
            priceText.Replace("$", "");
            priceText.Replace(",", "");
            // Convertir a double
            double price;
            if (priceText.ToDouble(&price)) return price;
        }
    }
    return 0.0;
}

//CHECKS IF PRODUCTS IN CART:

void MainFrame::CartHasProducts() {
    try {
        sqlite::database db(GetDBPath());
        // Verifica si la tabla 'cart' existe
        int tableExists = 0;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='cart';" >> tableExists;
        if (tableExists == 0) return; // La tabla no existe, no hay productos
        // La tabla existe, revisamos si tiene filas
        int count = 0;
        db << "SELECT COUNT(*) FROM cart;" >> count;
        if(count > 0) {
            int respuesta = wxMessageBox(_("The program did not close properly and there are items in the cart.\nDo you want to restore them?"), ATENCION, wxYES_NO | wxICON_INFORMATION);
            if (respuesta == wxNO) {
                try {
                    DeleteCartProducts();
                    return;
                }catch (const std::exception& e) {
                    wxMessageBox(wxString::Format(_("Error emptying cart: %s"), e.what()), "Error", wxOK | wxICON_ERROR);
                    return;
                }
            }
            else {
                db << "SELECT c.id, c.product_id, c.quantity, "
                    "p.name, p.price, p.byWeight, p.barcode "
                    "FROM cart c "
                    "JOIN products p ON c.product_id = p.id;"
                    >> [&](size_t cartId, size_t productId, double quantity,std::string name, double price, unsigned char byWeight, std::string barcode) {
					long index = -1;
                    if (byWeight) {
                        index = listaProductos->InsertItem(listaProductos->GetItemCount(), wxString::FromUTF8(name.c_str()) + wxString::Format(" (%.3f kg)", quantity));
                        listaProductos->SetItem(index, 1, wxString::Format("$%s", FormatFloatWithCommas(quantity * price)));
                    }
                    else { 
                        index = listaProductos->InsertItem(listaProductos->GetItemCount(), wxString::FromUTF8(name.c_str()));
                        listaProductos->SetItem(index, 1, wxString::Format("$%s", FormatFloatWithCommas(price)));
                    }
                    listaProductos->SetItem(index, 2, barcode);
                    listaProductos->SetItem(index, 3, wxString::Format("%zu", cartId));
					};
            }
            UpdateButtonRealizarCompra();
		}
    }
    catch (const sqlite::sqlite_exception& e) {
        return;
    }
}

//COMPRAS:

void MainFrame::CreateComprasTable() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "PRAGMA foreign_keys = ON;";
        db << "CREATE TABLE IF NOT EXISTS purchases ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "date TEXT NOT NULL, "
            "total REAL NOT NULL CHECK(total = ROUND(total, 2)),"
            "worker TEXT"
            ");";

        //Tabla para compra especÃÂÃÂ­fica
        db << "CREATE TABLE IF NOT EXISTS purchase_items ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "purchase_id INTEGER NOT NULL, "
            "product_id INTEGER NOT NULL, "
            "product_name TEXT NOT NULL, " // Guardar el nombre del producto en el momento de la compra
            "quantity REAL NOT NULL CHECK(quantity = ROUND(quantity, 3)), "
            "price_at_purchase REAL NOT NULL CHECK(price_at_purchase = ROUND(price_at_purchase, 2)), "
            "FOREIGN KEY(purchase_id) REFERENCES purchases(id) ON DELETE CASCADE, "
            "FOREIGN KEY(product_id) REFERENCES products(id)"
            ");";
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error creating the purchases table:") + wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
	}
}

double MainFrame::AddCompraToDB() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA foreign_keys = ON;";
        double total = 0.0;
        db << R"(
            SELECT SUM(p.price * c.quantity)
            FROM cart c
            JOIN products p ON p.id = c.product_id;
        )" >> total;
        wxString currentDate = wxDateTime::Now().FormatISOCombined(' ');
        std::string actualUser = getUserFromJSON<std::string>();
        if (actualUser == "Ninguno") actualUser = "";
        if (actualUser.empty()) {
            db << "INSERT INTO purchases (date, total, worker) VALUES (?, ?, NULL);"
                << std::string(currentDate.mb_str())
                << total;
        }
        else {
            db << "INSERT INTO purchases (date, total, worker) VALUES (?, ?, ?);"
                << std::string(currentDate.mb_str())
                << total
                << actualUser;
        }

        int purchaseId = 0;
        db << "SELECT last_insert_rowid();" >> purchaseId;
        db << R"(
            INSERT INTO purchase_items (purchase_id, product_id, product_name, quantity, price_at_purchase)
            SELECT ?, p.id, p.name, c.quantity, ROUND(p.price * c.quantity, 2)
            FROM cart c
            JOIN products p ON p.id = c.product_id;
        )" << purchaseId;

        db << R"(
            UPDATE stock
            SET quantity = quantity - (
                SELECT SUM(c.quantity)
                FROM cart c
                WHERE c.product_id = stock.product_id)
            WHERE product_id IN (SELECT product_id FROM cart);
        )";
        DeleteCartProducts();
        return total;
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error registering purchase: %s"), e.what()), "Error", wxOK | wxICON_ERROR);
        return -1.0;
    }
}
