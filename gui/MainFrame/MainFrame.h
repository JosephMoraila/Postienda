/**
 * @file MainFrame.h
 * @brief DeclaraciÃÂÃÂ³n de la clase MainFrame que representa la ventana principal de la aplicaciÃÂÃÂ³n.
 * @version 1.0
 */

#pragma once
#include <optional>
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/listctrl.h>
#include "gui/ProductosFrame/ProductosFrame.h"
#include "gui/TicketFrame/TicketFrame.hpp"
#include "gui/PreviousPurchasesFrame/PreviousPurchasesFrame.hpp"
#include "gui//InfoProductsFrame/InfoProductsFrame.hpp"
#include "gui/MainFrame/logic/MainFrame_models.hpp"
#include "gui/LanguageFrame/LanguageFrame.hpp"
#include "utils/MathUtils.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

 // Forward declaration para evitar inclusiÃÂÃÂ³n circular con UsuariosFrame
class UsuariosFrame;

/**
 * @class MainFrame
 * @brief Ventana principal de la aplicaciÃÂÃÂ³n.
 *
 * Hereda de wxFrame y maneja la interfaz principal,
 * los menÃÂÃÂºs, los botones y la interacciÃÂÃÂ³n con otras ventanas como ProductosFrame y UsuariosFrame.
 */
class MainFrame : public wxFrame
{
public:
    /**
     * @brief Constructor de MainFrame.
     * @param title: TÃÂÃÂ­tulo de la ventana principal que se mostrarÃÂÃÂ¡ en la barra de tÃÂÃÂ­tulo.
     *
     * Inicializa la ventana principal y prepara los controles y eventos.
     */
    MainFrame(const wxString& title);

    /**
     * @brief Obtiene el usuario actual desde un archivo JSON.
     */
    void GetActualUserFromJSON();

protected:
#ifdef _WIN32
    /**
     * @brief Override de wxFrame para capturar mensajes de Windows especÃÂÃÂ­ficos.
     *
     * Este mÃÂÃÂ©todo permite interceptar mensajes del sistema operativo antes de que
     * wxWidgets los procese. Se utiliza principalmente para detectar cambios en
     * la configuraciÃÂÃÂ³n del sistema, como el cambio automÃÂÃÂ¡tico entre modo claro y oscuro.
     *
     * @param nMsg Identificador del mensaje enviado por Windows (por ejemplo, WM_SETTINGCHANGE).
     * @param wParam ParÃÂÃÂ¡metro adicional del mensaje (depende del tipo de mensaje).
     * @param lParam ParÃÂÃÂ¡metro adicional del mensaje (depende del tipo de mensaje, a menudo un puntero o handle).
     * @return Retorna el resultado del procesamiento del mensaje, generalmente pasado a wxFrame::MSWWindowProc.
     *
     * @note Este mÃÂÃÂ©todo solo se compila en Windows (_WIN32) y requiere que la clase sea derivada de wxFrame.
     *
     * @details
     * Este override verifica si el mensaje recibido es WM_SETTINGCHANGE, que se envÃÂÃÂ­a cuando
     * el usuario cambia configuraciones del sistema (como el tema de color). Si el mensaje
     * corresponde a la secciÃÂÃÂ³n "ImmersiveColorSet", y la aplicaciÃÂÃÂ³n estÃÂÃÂ¡ configurada para
     * seguir el modo del sistema, entonces se llama a IsDarkModeSystem() para obtener el
     * estado actual (oscuro o claro) y se aplica con AplicarTema().
     */
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) override;
#endif

private:
    /// @name Funciones relacionadas con la interfaz principal
    ///@{

    /**
     * @brief Crea la barra de menÃÂÃÂº de la ventana.
     */
    void MenuBar();

    /**
     * @brief Evento al cerrar la ventana principal.
     * @param event Evento de cierre de wxWidgets.
	 */
	void OnClose(wxCloseEvent& event);

    /**
		@brief Event that is triggered when a key E is pressed while the list control has focus on a row to delete it
		@param event - wxKeyEvent reference
    **/
    void OnListaKeyDown(wxKeyEvent& event);

    /**
		@brief Updates the visibility and enabled state of the "Realizar Compra" button based on whether there are items in the list control.
    **/
    void UpdateButtonRealizarCompra();

    /**
	 @brief Returns a vector with the indexes of the selected items in the list control
	 @retval - Vector of size_t with the indexes of the selected items, if no item is selected returns an empty vector
    **/
	std::vector<size_t> ReturnSelectedItemsControlList();

    /**
		@brief Shows a bullet point 'ÃÂÃÂ' in the appearance menu showing the current theme
    **/
    void ActualizarMenuApariencia();

    /**
     * @brief Crea los widgets principales (paneles y botones).
     */
    void Widgets();

    /**
     * @brief Evento al hacer clic en el botÃÂÃÂ³n de productos.
     * @param event Evento de comando de wxWidgets.
     */
    void OnProductosClicked(wxCommandEvent& event);

    /**
     * * @brief Abre la ventana de contraseña para crearla o ingresarla.
     * \return True si fue exitoso el ingreso o la creacion, false si hubo un error o fue contraseña mala
     */
    bool PasswordWindow();

    /**
     * @brief Evento al hacer clic en el botÃÂÃÂ³n de usuarios.
     * @param event Evento de comando de wxWidgets.
     */
    void OnUsersClicked(wxCommandEvent& event);

    /**
	 @brief Evento al hacer clic en el botÃÂÃÂ³n de tickets, abre la ventana de tickets
	 @param event - Evento de comando de wxWidgets
    **/
	void OnTicketClicked(wxCommandEvent& event);

     /**
		 @brief Opens the language selection dialog
		 @param event - Event command
     **/
	void OnLanguageClicked(wxCommandEvent& event);

    /**
        @brief Opens the previous purchases frame
        @param event - Event command
    **/
    void OnPreviousPurchasesClicked(wxCommandEvent& event);

    void onInfoProductsClicked(wxCommandEvent& event);

    /**
     * @brief Evento al seleccionar tema claro.
     * @param event Evento de comando.
     */
    void OnTemaClaro(wxCommandEvent& event);

    /**
     * @brief Evento al seleccionar tema oscuro.
     * @param event Evento de comando.
     */
    void OnTemaOscuro(wxCommandEvent& event);

    /**
     * @brief Calls IsDarkModeSystemWindows or IsDarkModeSystemLinux depending on SO
     * 
     * @return true if system is using dark mode.
     * @return false if not
     */
    bool IsDarkModeSystem();

    #ifdef _WIN32
    /**
     * @brief Comprueba si el sistema operativo Windows estÃÂÃÂ¡ usando modo oscuro para aplicaciones.
     *
     * Esta funciÃÂÃÂ³n solo se ejecuta en Windows (_WIN32). Consulta el registro de Windows
     * en la clave:
     *  HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
     * y lee el valor "AppsUseLightTheme" para determinar si las aplicaciones usan
     * tema claro u oscuro.
     *
     * @return true  Si el sistema estÃÂÃÂ¡ en modo oscuro (AppsUseLightTheme = 0)
     * @return false Si el sistema estÃÂÃÂ¡ en modo claro o si ocurre un error
     */
    bool IsDarkModeSystemWindows();
    #elif __linux__
    /**
     * @brief Calls by command the differente desktop enviorenment if dark mode is on
     * 
     * @return true if dark mode is on.
     * @return false if is not.
     */
    bool IsDarkModeSystemLinux();
    #endif
    /**
        @brief Lee el JSON de tema y en base a los parametros aplicarÃÂÃÂ¡ el tema
        @details Se obtiene si usar tema modo sistema y si se estÃÂÃÂ¡ usando del sistema se llama a @ref IsDarkModeSystem que devuelve si el sistema estÃÂÃÂ¡ en modo oscuro o no, si no se obtiene
        si se estÃÂÃÂ¡ en modo oscuro sin el sistema, al final se llama a @ref AplicarTema para aplicar tema ya sea claro o negro.
    **/
    void AplicarTemaActual();        
    /**
     * @brief Evento al seleccionar tema del sistema.
     * @param event Evento de comando.
     */
    void ApplySystemMode(wxCommandEvent& event);

    /**
     * @brief Aplica el tema oscuro o claro a la ventana y sus controles.
     * @param oscuro true si el tema debe ser oscuro, false si debe ser claro.
     */
    void AplicarTema(bool oscuro);

    /**
        @brief Adjust the width of the columns of the control list when the windows changes its size
    **/
    void AjustarColumnasListCtrl();

    /**
     * @brief Evento al cambiar el tamaÃÂÃÂ±o de la ventana.
     * @param event Evento de tamaÃÂÃÂ±o.
     */
    void OnSize(wxSizeEvent& event);

    /**
     * @brief Crea o actualiza el JSON para almacenar la configuraciÃÂÃÂ³n del tema oscuro.
     */
    void CreateJSONTemaOscuro();

    /**
     * @brief Guarda la configuraciÃÂÃÂ³n actual del tema.
     */
    void GuardarConfiguracion();

    /**
        @brief Creats table purchases if not exists, ask user payment method and calls PrintTicket 
        @param event - Button command
    **/
    void OnRealizarCompra(wxCommandEvent& event);

    ///@}


    /// @name Punteros a otras ventanas y controles
    ///@{

    /// Ventana de productos (nullptr si no estÃÂÃÂ¡ abierta)
    ProductosFrame* productosVentana = nullptr;

    /// Ventana de usuarios (nullptr si no estÃÂÃÂ¡ abierta)
    UsuariosFrame* usuariosVentana = nullptr;

	TicketFrame* ticketVentana = nullptr; ///> Ventana de tickets (nullptr si no estÃÂÃÂ¡ abierta)

    PreviousPurchaseFrame* previousPurchasesVentana = nullptr; ///Previous purchases window (nullptr if is not opened)

    InfoProductsFrame* infoProductsVentana = nullptr; ///Info products window

	LanguageFrame* LanguageWindow = nullptr; ///> Window for language selection (nullptr if is not opened)

    /// Panel principal de la ventana
    wxPanel* mainPanel = nullptr;

	wxListCtrl* listaProductos = nullptr; /// Control de lista para mostrar productos en el carrito

    /// Campo de entrada de texto
    wxTextCtrl* input = nullptr;

    wxButton* button = nullptr; /// BotÃÂÃÂ³n comodÃÂÃÂ­n

	/// BotÃÂÃÂ³n que aparece cuando se selecciona un producto
    wxButton* buttonDeleteProducts = nullptr;

	wxButton* buttonRealizarCompra = nullptr; /// BotÃÂÃÂ³n para realizar compra

	wxStaticText* labelTotal = nullptr; ///Label that shows total price
	double totalUI = 0.0; ///> Total price shown in labelTotal

    /// Estado del tema: false = claro, true = oscuro
    bool temaOscuro = false;

    /// Estado para saber si se estÃÂÃÂ¡ usando tema oscuro o claro del sistema y en base a eso aplicar tema correspondiente en ventanas
    bool usarModoSistema = false;   
    ///@}


    /**
    * @name LOGIC: DB Functions
    * @{
    */

    std::optional<ProductInfo> m_lastSelectedProduct; ///> Almacena el ÃÂÃÂºltimo producto seleccionado

    std::unordered_map<size_t, double> productsIdsStock; ///Sirve para almacenar el id del producto y su stock para agregar uno y si ya existe actualizar y si baja de 0 no dejar que registre ese producto a comprar porque ya no hay o sumar si se devuelve. Es un stock temporal.

    /**
		@brief Asks the user for the weight of the last selected product and calculates its price based on it. Then updates the GUI list control.
    **/
    void CalculatePriceWeight();

    /**
		@brief Verifies if there are products in the cart table DB when opening the app, if there are products it asks the user if they want to keep them or delete them.
    **/
    void CartHasProducts();

    //GET PRODUCT BY BARCODE:

    /**
		@brief Gets a product from the database by its barcode.
		@param  barcodeToFind - Barcode of the product to search for in the database.
		@retval - Returns a ProductInfo struct with the product details if found, otherwise returns std::nullopt.
    **/
    std::optional<ProductInfo> GetProductFromDBbyBarcode(const std::string& barcodeToFind);

	//GET PRODUCT BY BARCODE FINISHES
    //GET PRODUCT BY NAME:

    /**
		@brief Gets the input text from the user and searches for products in the database calling other functions.
        @details This function is triggered by a button click event. It retrieves the text entered by the user in the input field,
		then calls SeleccionarProductoPorNombre to check if a product with that name exists in the database and return id, then calls
		GetProductFromDBbyId to get the full product details by id, and finally adds the product to the list control if found.
		@param event - Event object from wxWidgets representing the button click event.
    **/
    void CheckInputLogic(wxCommandEvent& event);

    /**
		@brief  Gets product(s) from the database by name, returning a vector of ProductInfo structs if there are more than one match.
		@param  productName - Product name to search for in the database.
		@retval - Vector of ProductInfo structs containing details of matching products.
    **/
    std::vector<ProductInfo> GetProductFromDBbyName(const std::string& productName);

    /**
		@brief Calls GetProductFromDBbyName to check if there's a product(s) with that name, if there are multiple matches asks the user to select one to return its id.
		@param  productName - Product name to search for in the database, it's passed to GetProductFromDBbyName.
		@retval - Returns the product id selected or found, otherwise returns 0.
    **/
    size_t SeleccionarProductoPorNombre(const std::string& productName);

    /**
		@brief Gets a product from the database by its id.
		@param  productId - Id of the product to search for in the database.
		@retval Returns a ProductInfo struct with the product details if found, otherwise returns an empty struct.
    **/
    ProductInfo GetProductFromDBbyId(size_t productId);

     /**
	     @brief Adds a product to the list control in the GUI.
	     @param nombreCompleto - Full name of the product to display in the list control.
	     @param precioFinal - Final price of the product to display in the list control.
     **/
	void AddProductToListCtrl(wxString nombreCompleto, double precioFinal);

    //GET PRODUCT BY NAME FINISHES
	//ADD/DELETE PRODUCT TO CART:

    /**
		@brief Creates the list control table for displaying products in the cart.
    **/
    void CreateCartTable();

    /**
		@brief Adds a product to the cart list control in the GUI.
		@param productId - Id of the product to add to the cart.
		@param qty - Quantity of the product to add to the cart. If the product is already in the cart, this value is added to the existing quantity.
    **/
    void AddToCart(size_t& productId, double& qty);
    /**
		@brief Asks the user for confirmation and deletes all products from the cart list control in the GUI.
		@param  mensaje - Message to display in the confirmation dialog.
		@retval - Returns true if the user confirmed the deletion, false by exceptions or if user didn't accept. Also returns true if the cart was already empty or the table does not exist.
    **/
    bool DeleteCartProductsAsk(wxString& mensaje);
    /**
        @brief Just deletes the info from cart table
    **/
    void DeleteCartProducts();
     /**
	     @brief Deletes selected products from the cart list control in the GUI based on their indices.
	     @param v - Vector of indices of the products to delete from the cart.
     **/
	void DeleteSelectedProductFromCart(const std::vector<size_t>& v);
    /**
     * @brief retorna el la cantidad al stock al unordered map productsIdsStock ya que se quito de el producto del carrito
     * \param idsProductsCart Ids de cart/carrito para buscar su cantidad y product_id
     */
    void ReturnStockWhenDeletItemInCart(const std::vector<size_t>& idsProductsCart);
    /**
		@brief Gets the price of an item in the cart list control by its index.
		@param  searchId - Index of the item in the cart list control to get the price for.
		@retval - Returns the price of the item if found, otherwise returns -1.0.
    **/
    double GetItemPriceById(const size_t searchId);

    //COMPRAS:

    /**
	 @brief Creates "purchases" and "purchase_items" tables in the database if they do not exist.
    **/
	void CreateComprasTable();
    /**
	 @brief Adds the current cart products to the "purchases" and "purchase_items" tables in the database,updates the stock, then clears the cart.
	 @retval - Returns the total price of the purchase if successful, -1.0 on error.
    **/
	double AddCompraToDB();

    /** @} */ // cierra Funciones de productos

        /**
    * @name PrintTicket Functions
    * @{
    */

    /**
        @brief Evaluates if the printer settings are enabled to print and which mode
        @param pagoCliente - How much the client paid
        @param cashPayment - Payment method
    **/
    void PrintTicket(double pagoCliente, bool cashPayment);
#ifdef _WIN32
    /**
        @brief Print the ticket on canvas mode
        @param pagoCliente - How much the client paid
        @param cashPayment - Payment method
    **/
    void PrintCanvasPurchaseWindows(double& pagoCliente, bool& cashPayment);
#endif
    /**
        @brief Print the ticket on EscPos mode
        @param pagoCliente - How much the client paid
        @param cashPayment - Payment method
    **/
    void PrintEscposPurchase(double& pagoCliente, bool& cashPayment);
    /**
        @brief Get the last purchase info from DB to append the paid items 
        @param  pagoCliente - Finally used here to show on the ticket the change
        @param  cashPayment - Finally used here to know if the change is 0 (paid with card)
        @retval - Purchase info text
    **/
    wxString GetInfoPurchaseToPrint(double& pagoCliente, bool& cashPayment);

    /** @} */

    enum IDs {
        ID_MENU_PRODUCTOS = 1001,
        ID_MENU_USERS,
        ID_MENU_TICKET,
		ID_MENU_LANGUAGE,
        ID_MENU_PREVIOUS_PURCHASES,
        ID_MENU_INFO_PRODUCTS,
        ID_MENU_CLARO,
        ID_MENU_OSCURO,
        ID_SYSTEM_MODE,
        ID_INPUT,
        ID_BUTTON_DELETE_PRODUCTS
    };


};
