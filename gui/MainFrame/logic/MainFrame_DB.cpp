#include "utils/GetFromFile.h"
#include "gui/MainFrame/MainFrame.h"
#include "utils/ValidateStringInput.h"
#include "constants/DB/DB.h"
#include "constants/MESSAGES_ADVICE/WARNING/ATENCION.hpp"
#include "constants/MESSAGES_ADVICE/INFO/NO_PRODUCT.hpp"
#include "constants/FOLDERS/SETTINGS/SETTINGS_FOLDER.hpp"
#include "constants/MESSAGES_ADVICE/ERROR/DB/DB_ERRORS.hpp"
using namespace DB_ERROR_MESSAGES;

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

            size_t productId = m_lastSelectedProduct->id; //Obtenemos su id para añadir a productsIdsStock o verificar si queda stock si ya existe el id
            if (productsIdsStock.find(productId) == productsIdsStock.end()) {//Si no existe lo añadimos
                productsIdsStock[productId] = m_lastSelectedProduct->stock; 
                double stockTemp = productsIdsStock.at(productId);
                if (stockTemp <= 0) {
                    wxMessageBox(_("You can not add to cart a product without stock"), ATENCION, wxOK | wxICON_WARNING);
                    return;
                }
                productsIdsStock.at(productId) = stockTemp - 1;
            }
            else { //Si ya lo teniamos registrado tenemos que restar lo que teniamos en stock la primera vez que lo registramos menos 1 porque se esta comprando por unidad
                double stockTemp = productsIdsStock.at(productId);
                if (stockTemp <= 0) { //Si se quiere volver a agregar otra vez el producto al carrito como ya está verificado verificar que no sea 0
                    wxMessageBox(_("You can not add to cart a product without stock"), ATENCION, wxOK | wxICON_WARNING);
                    return;
                }
                productsIdsStock.at(productId) = stockTemp - 1;
            }

            //Verificar si el stock de ese producto es 0 y mandar mensaje porque no se pueden agregar productos al carrito qie ya no tienen stock
 
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
	size_t lastIdProduct = m_lastSelectedProduct->id; double lastQuantity = m_lastSelectedProduct->cantidad;
    AddToCart(lastIdProduct, lastQuantity, precioFinal);
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

                size_t productId = m_lastSelectedProduct->id; //Obtenemos su id para añadir a productsIdsStock o verificar si queda stock si ya existe el id
                if (productsIdsStock.find(productId) == productsIdsStock.end()) productsIdsStock[productId] = m_lastSelectedProduct->stock; //Si no existe lo añadimos

                //Verificar si el stock de ese producto es 0 y mandar mensaje porque no se pueden agregar productos al carrito que ya no tienen stock
                double stockTemp = productsIdsStock.at(productId);
                if (stockTemp < m_lastSelectedProduct->cantidad) {
                    wxMessageBox(_("There's not enough quantity"), ATENCION, wxOK | wxICON_WARNING);
                    return;
                }
                productsIdsStock.at(productId) = stockTemp - m_lastSelectedProduct->cantidad;

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

        db << "SELECT pro.id, pro.name, pro.price, pro.barcode, pro.byWeight, stk.quantity "
            "FROM products pro "
            "JOIN stock stk ON pro.id = stk.product_id "
            "WHERE pro.barcode = ?;"
            << barcodeToFind
            >> [&](size_t id, const std::string& nombre, double precio, const std::string& codigoBarras, unsigned char porPeso, double stock) {
            // Llenamos el optional solo si hay resultado
            result = ProductInfo(id, nombre, precio, codigoBarras, porPeso, stock);
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
        db << "SELECT pro.id, pro.name, pro.price, pro.barcode, pro.byWeight, stk.quantity "
            "FROM products pro "
            "JOIN stock stk ON pro.id = stk.product_id "
            "WHERE pro.id = ?;"
            << productId
            >> [&](size_t id, std::string nombre, double precio, std::string codigoBarras, unsigned char porPeso, double stock) {
            Product = ProductInfo(id, nombre, precio, codigoBarras, porPeso, stock);
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

        db << "SELECT p.id, p.name, p.price, p.barcode, p.byWeight, c.name, stk.quantity  "
            "FROM products p "
            "JOIN categories c ON p.category_id = c.id "
            "JOIN stock stk ON p.id = stk.product_id "
            "WHERE p.name = ?;"
            << productName
            >> [&](size_t id, std::string nombre, double precio,std::string codigoBarras, unsigned char porPeso, std::string categoria, double stock) {
                    resultados.push_back({ id, nombre, precio, codigoBarras, porPeso != 0, categoria, stock });
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

        bool exists = false;
        db << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='cart'" >> exists;

        if (!exists) {
            db << "CREATE TABLE IF NOT EXISTS cart ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "product_id INTEGER NOT NULL, "
                "quantity REAL NOT NULL DEFAULT 1.000 CHECK(quantity = ROUND(quantity, 3)), "
                "price REAL NOT NULL DEFAULT 0.00 CHECK(price = ROUND(price, 2)), "
                "FOREIGN KEY(product_id) REFERENCES products(id) ON DELETE CASCADE"
                ");";
        }
        else {
            bool columnExists = false;
            db << "SELECT COUNT(*) FROM pragma_table_info('cart') WHERE name='price'" >> columnExists;
            if (!columnExists)
                db << "ALTER TABLE cart ADD COLUMN price REAL NOT NULL DEFAULT 0.00 CHECK(price = ROUND(price, 2));";
        }
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error creating cart table ") + e.what(), "Error", wxOK | wxICON_ERROR);
    }
}


void MainFrame::AddToCart(size_t& productId, double& qty, double& precioFinal) {
    try {
        sqlite::database db(GetDBPath());
        db << "INSERT INTO cart (product_id, quantity, price) VALUES (?, ?, ?); "
            << productId
            << qty
            << precioFinal;

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
                productsIdsStock.clear(); //Limpiamos los ids de producto y su stock temporal
                return true;  // Tabla no existe, retornar ÃÂÃÂ©xito sin mostrar error
            }
        }
        catch (const std::exception&) {
            totalUI = 0.0;
            labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
            productsIdsStock.clear();
            return true;  // Si falla la verificaciÃÂÃÂ³n, asumir que no existe
        }

        // La tabla existe, proceder normalmente
        int count = 0;
        db << "SELECT COUNT(*) FROM cart;" >> count;
        if (count == 0) {
            totalUI = 0.0;
            labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
            productsIdsStock.clear();
            return true;
        }
        wxMessageDialog confirmDialog(this,mensaje, _("Confirm"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
        if (confirmDialog.ShowModal() == wxID_YES) {
            try{
                DeleteCartProducts();
                totalUI = 0.0;
                labelTotal->SetLabel(wxString::Format("Total: %.2f", totalUI));
                productsIdsStock.clear();
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
        for (const size_t& idProductCart : v) {
            double price = GetItemPriceById(idProductCart);
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

void MainFrame::ReturnStockWhenDeletItemInCart(const std::vector<size_t>& idsProductsCart) {
    try {
        sqlite::database db(GetDBPath());
        for (const size_t& idProductCart : idsProductsCart) {
            db << "SELECT quantity, product_id FROM cart WHERE id = ?" << idProductCart >> [&](double quantity, size_t productId) {
                double tempStock = productsIdsStock.at(productId);
                double updatedStock = tempStock + quantity;
                productsIdsStock.at(productId) = updatedStock;
                };
        }
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
}


double MainFrame::GetItemPriceById(size_t searchId)
{
    for (long i = 0; i < listaProductos->GetItemCount(); ++i) {

        size_t rowId = std::stoul(std::string(listaProductos->GetItemText(i, 3).mb_str()));

        if (rowId == searchId) {

            wxString text = listaProductos->GetItemText(i, 1);

            wxString clean;
            for (wxChar c : text) {
                if ((c >= '0' && c <= '9') || c == '.') {
                    clean += c;
                }
            }

            double price = 0.0;
            clean.ToDouble(&price);

            return price;
        }
    }
    return 0.0;
}


//CHECKS IF PRODUCTS IN CART:

void MainFrame::CartHasProducts() {
    try {
        double totalToShow = 0.0;
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
                std::unordered_map<size_t, double> sumaAllevarProductId;// Creamos la suma de la cantidad a llevar de un id de un producto para restarlo a productsIdsStock
                db << "SELECT c.id, c.product_id, c.quantity, "
                    "p.name, c.price, p.byWeight, p.barcode, stk.quantity "
                    "FROM cart c "
                    "JOIN products p ON c.product_id = p.id "
                    "JOIN stock stk ON stk.product_id = c.product_id"
                    >> [&](size_t cartId, size_t productId, double quantity,std::string name, double price, unsigned char byWeight, std::string barcode, double stock) {
					long index = -1;
                    if (byWeight) {
                        index = listaProductos->InsertItem(listaProductos->GetItemCount(), wxString::FromUTF8(name.c_str()) + wxString::Format(" (%.3f kg)", quantity));
                        listaProductos->SetItem(index, 1, wxString::Format("$%s", FormatFloatWithCommas(price)));
                    }
                    else { 
                        index = listaProductos->InsertItem(listaProductos->GetItemCount(), wxString::FromUTF8(name.c_str()));
                        listaProductos->SetItem(index, 1, wxString::Format("$%s", FormatFloatWithCommas(price)));
                    }
                    totalToShow += price;
                    listaProductos->SetItem(index, 2, barcode);
                    listaProductos->SetItem(index, 3, wxString::Format("%zu", cartId));
                    //Si esta el producto y su stock lo agregamos
                    
                    //Si no esta lo agregamos y el valor inicial es la cantidad
                    if (sumaAllevarProductId.find(productId) == sumaAllevarProductId.end()) sumaAllevarProductId[productId] = quantity;
                    else {//Si ya existe le sumamos la cantidad nueva
                        double totalQuantityProduct = sumaAllevarProductId.at(productId);
                        double updatedQuantity = totalQuantityProduct + quantity;
                        sumaAllevarProductId.at(productId) = updatedQuantity;
                    } 
                    if (productsIdsStock.find(productId) == productsIdsStock.end()) productsIdsStock[productId] = stock;
					};
                //Ya que terminamos de añadir el stock necesitamos restar la cantidad que hay en el cart/carrito
                for (const auto& [productId, stock] : sumaAllevarProductId) {
                    double totalQuantityProduct = sumaAllevarProductId.at(productId);
                    double productStock = productsIdsStock.at(productId);
                    productsIdsStock.at(productId) = productStock - totalQuantityProduct;
                }
            }
            totalUI += totalToShow;
            labelTotal->SetLabel(wxString::Format("Total: $%s", FormatFloatWithCommas(totalUI)));
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
            "worker TEXT,"
			"method TEXT NOT NULL DEFAULT 'Cash'"
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

std::pair<double, size_t> MainFrame::AddCompraToDB(bool esEfectivo) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA foreign_keys = ON;";
        double total = 0.0;
        db << R"(
            SELECT SUM(c.price)
            FROM cart c;
        )" >> total;
        wxString currentDate = wxDateTime::Now().FormatISOCombined(' ');
        std::string actualUser = getUserFromJSON<std::string>();
		std::string paymentMethod = esEfectivo ? "Cash" : "Card";
        if (actualUser == "Ninguno") actualUser = "";
        if (actualUser.empty()) {
            db << "INSERT INTO purchases (date, total, worker, method) VALUES (?, ?, NULL, ?);"
                << std::string(currentDate.mb_str())
                << total
				<< paymentMethod;
        }
        else {
            db << "INSERT INTO purchases (date, total, worker, method) VALUES (?, ?, ?, ?);"
                << std::string(currentDate.mb_str())
                << total
                << actualUser
				<< paymentMethod;
        }

        int purchaseId = 0;
        db << "SELECT last_insert_rowid();" >> purchaseId;
        db << R"(
            INSERT INTO purchase_items (purchase_id, product_id, product_name, quantity, price_at_purchase)
            SELECT ?, p.id, p.name, c.quantity, c.price
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
        return { total,purchaseId };
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error registering purchase: %s"), e.what()), "Error", wxOK | wxICON_ERROR);
        return { -1.0,0 };
    }
}

//CREATE TABLES:
void MainFrame::CreateTables() {
    CreateProductsCategoriesTable();
    CreateComprasTable();
    CreateDrawerTable();
    CreateCartTable();
}

//PRODUCTS AND CATEGORIES TABLES:

void MainFrame::CreateProductsCategoriesTable() {
    try {
        sqlite::database db(GetDBPath());

        // Configurar SQLite para UTF-8
        db << "PRAGMA foreign_keys = ON;";
        db << "PRAGMA encoding = 'UTF-8';";

        // --- Crear tablas normales ---
        db << "CREATE TABLE IF NOT EXISTS categories ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL COLLATE NOCASE,"
            "parent_id INTEGER,"
            "FOREIGN KEY(parent_id) REFERENCES categories(id) ON DELETE CASCADE,"
            "UNIQUE(name, parent_id)"
            ");";

        db << "CREATE TABLE IF NOT EXISTS products ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL COLLATE NOCASE,"
            "price REAL NOT NULL DEFAULT 0.00 CHECK(price = ROUND(price, 2)),"
            "barcode TEXT UNIQUE,"
            "byWeight INTEGER NOT NULL DEFAULT 0,"
            "category_id INTEGER,"
            "FOREIGN KEY(category_id) REFERENCES categories(id) ON DELETE CASCADE,"
            "UNIQUE(name, category_id)"
            ");";

        // --- Crear tabla de stock que tendrÃÂÃÂ¡ la cantidad de cada producto ---
        db << "CREATE TABLE IF NOT EXISTS stock ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "product_id INTEGER NOT NULL UNIQUE,"   // RelaciÃÂÃÂ³n 1:1 con products
            "quantity REAL NOT NULL DEFAULT 0.000 CHECK (quantity >= 0 AND quantity = ROUND(quantity, 3)),"
            "FOREIGN KEY(product_id) REFERENCES products(id) ON DELETE CASCADE"
            ");";

        //Trigger para eliminar productos al eliminar una categorÃÂÃÂ­a
        db << "CREATE TRIGGER IF NOT EXISTS delete_products_on_category_delete "
            "AFTER DELETE ON categories "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM products WHERE category_id = OLD.id; "
            "END;";

        //Trigger para eliminar categorias hijas al eliminar una categorÃÂÃÂ­a padre
        db << "CREATE TRIGGER IF NOT EXISTS delete_child_categories "
            "AFTER DELETE ON categories "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM categories WHERE parent_id = OLD.id; "
            "END;";

        // --- Crear el TRIGGER para eliminar stock asociado ---
        db << "CREATE TRIGGER IF NOT EXISTS delete_product_stock "
            "AFTER DELETE ON products "
            "FOR EACH ROW "
            "BEGIN "
            "DELETE FROM stock WHERE product_id = OLD.id; "
            "END;";


        db << "PRAGMA foreign_keys = ON;";

        // --- Asegurar categorÃÂÃÂ­a raÃÂÃÂ­z "Productos" en permanentes ya que siempre serÃÂÃÂ¡ 1---
        int root_count = 0;
        db << "SELECT COUNT(*) FROM categories WHERE id = 1;" >> root_count;
        if (root_count == 0) db << "INSERT INTO categories (name, parent_id) VALUES ('Products', NULL);";


    }
    catch (const std::exception& e) {
        wxString wxError = wxString::FromUTF8(e.what());
        wxString wxFile = wxString::FromUTF8(__FILE__);
        wxString msg = wxString::Format(DB_ERROR_LINE_MESSAGE,wxFile,__LINE__,wxError);
        wxMessageBox(msg, "Error SQLite", wxOK | wxICON_ERROR, this);
    }
}


//DRAWER TABLE:

void MainFrame::CreateDrawerTable() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "PRAGMA foreign_keys = ON;";
        db << "CREATE TABLE IF NOT EXISTS drawer ("
            "amount REAL NOT NULL DEFAULT 0.00 CHECK(amount = ROUND(amount, 2))  )";

        int count = 0;
        db << "SELECT COUNT(*) FROM drawer" >> count;
        if (count == 0) db << "INSERT INTO drawer (amount) VALUES (0.00)";

        db << "CREATE TABLE IF NOT EXISTS drawer_history ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "date TEXT NOT NULL, "
            "amount REAL NOT NULL CHECK(amount = ROUND(amount, 2)),"
            "is_addition INTEGER NOT NULL CHECK(is_addition IN (0, 1))," //0 para sustraccion y 1 para adicion
            "worker TEXT,"
            "reason TEXT,"
            "purchase_id INTEGER,"
            "drawer_after_insertion REAL NOT NULL CHECK(drawer_after_insertion = ROUND(drawer_after_insertion, 2)),"
            "FOREIGN KEY(purchase_id) REFERENCES purchases(id) ON DELETE SET NULL"
            ");";
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error creating the drawer table:") + wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
    }
}


//Discount::

wxString MainFrame::GetNameSelectProductCart(size_t& idProduct) {
    try {
        std::string productName = "";
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "SELECT p.name FROM cart c INNER JOIN products p ON c.product_id = p.id WHERE c.id = ?" << idProduct >> productName;
        wxString wxProductName = wxString::FromUTF8(productName.c_str());
        return wxProductName;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error retrieving the namee:") + wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
        return "";
    }

}

double MainFrame::GetProductPrice(size_t& idProduct) {
    try {
        double productPrice = 0.0;
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "SELECT p.price FROM cart c INNER JOIN products p ON c.product_id = p.id WHERE c.id = ?" << idProduct >> productPrice;
        return productPrice;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error retrieving the namee:") + wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
        return 0.0;
    }
}

bool MainFrame::ApplyDiscountToProductInDB(size_t& idProduct, double& newPrice) {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "UPDATE cart SET price = ? WHERE id = ?" << newPrice << idProduct;
        return true;
    }
    catch (const sqlite::sqlite_exception& e) {
        wxMessageBox(_("Error apply discount:") + wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
        return false;
    }
}
