/**
 * @file ProductosFrame.h
 * @brief DeclaraciÃÂÃÂÃÂÃÂ³n de la clase ProductosFrame que representa la ventana de gestiÃÂÃÂÃÂÃÂ³n de productos y categorÃÂÃÂÃÂÃÂ­as.
 * @version 1.0
 * @details
 * La clase se encarga de:
 * - Crear y mostrar la ventana de productos.
 * - Manejar la estructura de categorÃÂÃÂÃÂÃÂ­as y productos en memoria.
 * - Interactuar con archivos JSON para persistencia.
 * - Interactuar con la base de datos temporal SQLite.
 * - Aplicar temas (claro/oscuro) y efectos de hover en botones.
 */

#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>
#include <map>
#include <vector>
#include <string>
#include "third_party/json.hpp"
#include <set>
#include "include/models.hpp"
#include "sqlite_modern_cpp.h"
#include <variant>
#include "utils/MathUtils.hpp"

using json = nlohmann::json;

/**
 * @brief Clase que representa la ventana principal de productos y categorÃÂÃÂÃÂÃÂ­as para su creaciÃÂÃÂÃÂÃÂ³n, ediciÃÂÃÂÃÂÃÂ³n y eliminaciÃÂÃÂÃÂÃÂ³n.
 * @details Hereda de wxFrame, permite gestionar visualmente el ÃÂÃÂÃÂÃÂ¡rbol de categorÃÂÃÂÃÂÃÂ­as y productos,
 * realizar bÃÂÃÂÃÂÃÂºsquedas, agregar/eliminar elementos y guardar cambios en archivos JSON o base de datos.
 */
class ProductosFrame : public wxFrame
{
public:
    /**
     * @brief Constructor de la clase ProductosFrame.
     * @param parent Ventana padre que contendrÃÂÃÂÃÂÃÂ¡ este frame.
     */
    ProductosFrame(wxWindow* parent);

    /**
     * @brief Aplica el tema claro u oscuro a la ventana y todos sus widgets hijos.
     * @param oscuro true para tema oscuro, false para tema claro.
     */
    void AplicarTema(bool oscuro);

private:

    /** @name Widgets principales */
    /// @{
    wxPanel* mainPanel = nullptr;       ///< Panel principal contenedor de widgets.
    wxBoxSizer* vSizer = nullptr;       ///< Contenedor vertical para organizar widgets.
    wxTreeCtrl* arbolCategorias = nullptr; ///< ÃÂÃÂÃÂÃÂrbol visual de categorÃÂÃÂÃÂÃÂ­as y productos.
    wxButton* btnAgregarCategoria = nullptr; ///< BotÃÂÃÂÃÂÃÂ³n para agregar categorÃÂÃÂÃÂÃÂ­a.
    wxButton* btnAgregarProducto = nullptr; ///< BotÃÂÃÂÃÂÃÂ³n para agregar producto.
    wxButton* btnEliminar = nullptr;    ///< BotÃÂÃÂÃÂÃÂ³n para eliminar elementos seleccionados.
	wxButton* btnModify = nullptr;    ///< BotÃÂÃÂÃÂÃÂ³n para modificar elementos seleccionados de la lista.
	wxButton* btnStock = nullptr;    ///< BotÃÂÃÂÃÂÃÂ³n para gestionar stock de productos.
    wxPanel* panelBusqueda; ///< Panel contenedor de los controles de bÃÂÃÂÃÂÃÂºsqueda.
    wxTextCtrl* textoBusqueda; ///< Campo de texto para ingresar el tÃÂÃÂÃÂÃÂ©rmino de bÃÂÃÂÃÂÃÂºsqueda.
    wxButton* btnLimpiarBusqueda; ///< BotÃÂÃÂÃÂÃÂ³n para limpiar el tÃÂÃÂÃÂÃÂ©rmino de bÃÂÃÂÃÂÃÂºsqueda.
    wxStaticText* labelResultados; ///< Etiqueta para mostrar el nÃÂÃÂÃÂÃÂºmero de resultados encontrados.
    wxButton* btnBuscar; ///< BotÃÂÃÂÃÂÃÂ³n para iniciar la bÃÂÃÂÃÂÃÂºsqueda.
    bool enModoBusqueda; ///< Indica si actualmente se estÃÂÃÂÃÂÃÂ¡ en modo de bÃÂÃÂÃÂÃÂºsqueda (mostrando solo resultados).
    /// @}

    /** @name Funciones de creaciÃÂÃÂÃÂÃÂ³n de widgets y eventos */
    /// @{
	void Widgets(); ///< Crea y organiza los widgets en la ventana.
    void OnButtonEnter(wxMouseEvent& event); ///< Evento hover sobre botÃÂÃÂÃÂÃÂ³n.
    void OnButtonLeave(wxMouseEvent& event); ///< Evento salida de hover.
	void OnSize(wxSizeEvent& event);         ///< Evento cambio de tamaÃÂÃÂÃÂÃÂ±o que cambia tamaÃÂÃÂÃÂÃÂ±o de fuente al redimensionar ventana.
    //void OnSeleccionCambiada(wxTreeEvent& event); ///< Evento selecciÃÂÃÂÃÂÃÂ³n cambiada en el ÃÂÃÂÃÂÃÂ¡rbol.
    void CrearPanelBusqueda();               ///< Crea los controles para bÃÂÃÂÃÂÃÂºsqueda.
	void ContextualMenu();///< MenÃÂÃÂÃÂÃÂº contextual al hacer clic en un item. Determina aparitr de la busqueda en @ref mapaItems si es categorÃÂÃÂÃÂÃÂ­a o producto.
	void AppearButtonsModify(); ///< Muestra u oculta el botÃÂÃÂÃÂÃÂ³n de modificar segÃÂÃÂÃÂÃÂºn la selecciÃÂÃÂÃÂÃÂ³n.
    bool temaOscuro = false;             ///< Indica si el tema oscuro estÃÂÃÂÃÂÃÂ¡ activo.

    /// @}


    /** @DB functionss */
    /// @{

    /**
		@brief Creates the database and the necessary tables and triggers if they do not exist.
    **/
    void CrearConexionBD();
     /**
		 @brief Loads categories from the database into memory and the tree control.
     **/
	void LoadCategoriesFromDB();
     /**
		 @brief Loads products from the database into memory and the tree control.
     **/
	void LoadProductsFromDB();

	std::map<wxTreeItemId, std::shared_ptr<Categoria>> treeItemId_Category_Map; //!< Map to link tree items to categories
	std::map<wxTreeItemId, std::shared_ptr<Producto>> treeItemId_Product_Map;  //!< Map to link tree items to products
    /**
		@brief Finds the tree item ID corresponding to a given category.
		@param  cat - Shared pointer to the category to find.
		@retval - The wxTreeItemId corresponding to the category, or an invalid ID if not found.
    **/
    wxTreeItemId FindTreeItemByCategoria(const std::shared_ptr<Categoria>& cat);


	//Utility functions to check for existing names and codebars and others
     /**
		 @brief Checks if a category name already exists at the same hierarchical level.
		 @param  newCategoryName - Name of the new category to check.
         @param  categoryId      - Category Id to check
		 @retval - True if the category name exists at the same level, false otherwise.
     **/
	bool ExistsCategoryNameInSameLevel(wxString& newCategoryName, size_t& categoryId);
     /**
		 @brief Checks if a product name already exists within the same category.
		 @param  newProductName - Name of the new product to check.
		 @param  categoryId - Category Id to check
		 @retval - True if the product name exists in the same category, false otherwise.
     **/
	bool ExistsProductNameInSameCategory(wxString& newProductName, size_t& categoryId);
     /**
		 @brief Checks if a barcode already exists globally across all products.
		 @param  codebar - Barcode to check
		 @retval - True if the barcode exists globally, false otherwise.
     **/
	bool ExistsCodebarGlobal(const std::string& codebar);
     /**
		 @brief Adjusts stock values when a product's type changes from weighted to unit-based or vice. Example: 2.55 kg to 2.0 units.
		 @param productId - ID of the product whose type has changed.
		 @param wasByWeight - Indicates if the product was previously sold by weight.
		 @param nowByWeight - Indicates if the product is now sold by weight.
     **/
	void AdjustStockForProductTypeChange(size_t productId, bool wasByWeight, bool nowByWeight);


	//Insert/delete/update functions
     /**
		 @brief Inserts a new category into the database.
		 @param  nombre   - Name of the category to insert.
		 @param  parentId - ID of the parent category.
		 @retval - The ID of the newly inserted category.
     **/
	size_t InsertCategoryToDB(const std::string& nombre, size_t& parentId);
     /**
		 @brief Inserts a new product into the database.
		 @param product - Product object to insert.
		 @param categoryId - ID of the parent category.
     **/
	void InsertProductToDB(Producto& product, size_t& categoryId);

     /**
		 @brief Deletes a category from the database.
		 @param categoria - Shared pointer to the category to delete taking the category id from it.
     **/
	void DeleteCategoryFromDB(const std::shared_ptr<Categoria>& categoria);
     /**
		 @brief Deletes a product from the database.
		 @param producto - Product object to delete.
		 @param idCategoriaPadre - ID of the parent category.
     **/
	void DeleteProductFromDB(const Producto& producto, const size_t idCategoriaPadre);

     /**
		 @brief Updates the name of a category in the database.
		 @param newName     - New name for the category
		 @param idCategoria - ID of the category to update
		 @param oldName     - Old name of the category
     **/
	void UpdateCategoryNameInDB(std::string newName, size_t idCategoria, std::string oldName);
     /**
		 @brief Updates a product's information in the database.
		 @param oldProductInfo - Old product information
		 @param newProductInfo   - Shared pointer to the new product information
		 @param idCategoriaPadre - ID of the parent category
     **/
	void UpdateProductInDB(const Producto& oldProductInfo, const std::shared_ptr<Producto>& newProductInfo, size_t idCategoriaPadre);

     /**
		 @brief Inserts a stock record for a product into the database.
		 @param productId - ID of the product to insert stock for.
     **/
	void InsertStockProductToDB(size_t productId);
    /**
		@brief Retrieves the stock of a product from the database.
		@param  idProducto - ID of the product to retrieve stock for.
		@param  byWeight   - Output parameter indicating if the product is sold by weight.
		@retval - The stock value, either as unsigned long long (units) or double (weight).
    **/
    std::variant<unsigned long long, double> GetStockFromDB(size_t idProducto, bool& byWeight);
    /**
		@brief Updates the stock of a product in the database.
		@tparam T - Type of the stock value (unsigned long long for units, double for weight).
		@param  idProducto - ID of the product to update stock for.
		@param  stock - New stock value.
    **/
    template<typename T>
    void UpdateStockInDB(size_t idProducto, T stock);

    /// @}


    /** @Binds functionss */
    /// @{

     /**
		 @brief Asks user to name the new category and verify its uniqueness
		 @param event - Event triggered by clicking the "Add Category" button.
     **/
	void OnAgregarCategoria(wxCommandEvent& event); 
     /**
		 @brief A dialog is shown to the user to input product details and add it to the selected category, then verifies uniqueness.
		 @param event - Event triggered by clicking the "Add Product" button.
     **/
	void OnAgregarProducto(wxCommandEvent& event); 
    /**
         @brief Deletes the selected category or product after user confirmation.
         @param event - Event triggered by clicking the "Delete" button.
	 **/
	void OnEliminar(wxCommandEvent& event); 

     /**
		 @brief Searches for products or categories based on the input search term and updates the tree view with results.
		 @param event - Event triggered by clicking the "Search" button.
     **/
	void OnBuscar(wxCommandEvent& event); 
    /**
         @brief Clears the search term and restores the full tree view of categories and products.
         @param event - Event triggered by clicking the "Clear Search" button.
	 **/
	void OnLimpiarBusqueda(wxCommandEvent& event); 

    /// @}


    /** @Category related functionss */
    /// @{

    /**
		@brief Adds a new category under the specified parent category in the tree control.
		@param parentId - ID of the parent category where the new category will be added.
		@param nombre - Name of the new category to add.
    **/
    void Addcategory(const wxTreeItemId& parentId, const wxString& nombre);
     /**
		 @brief Deletes a category from the tree control and memory, including all its subcategories and products.
		 @param categoryId - ID of the category to delete.
     **/
	void DeleteCategory(const wxTreeItemId& categoryId);
     /**
		 @brief Removes references to a category from its parent before deletion.
		 @param categoryId - ID of the category whose references will be deleted.
     **/
	void DeleteReferencesCategory(const wxTreeItemId& categoryId);
    /**
		@brief Updates the information of a category in the tree control and memory.
		@param item     - Tree item ID of the category to update.
		@param parentId - Tree item ID of the parent category to which the category belongs to verify name uniqueness.
    **/
    void ChangeCategoryInfo(const wxTreeItemId& item, const wxTreeItemId& parentId);

    /// @}


	/** @Product related functionss */
	/// @{

    /**
		@brief Adds a new product under the specified parent category in the tree control.
		@param product  - Product object to add.
		@param parentId - ID of the parent category where the product will be added.
    **/
    void AddProduct(Producto& product, const wxTreeItemId& parentId);
     /**
		 @brief Deletes a product from the tree control and memory.
		 @param productId - ID of the product to delete.
     **/
	void DeleteProduct(const wxTreeItemId& productId);
     /**
		 @brief Updates the information of a product in the tree control and memory.
		 @param fatherCategory - Shared pointer to the parent category of the product.
		 @param item - Tree item ID of the product to update.
     **/
	void ChangeProductInfo(const std::shared_ptr<Categoria>& fatherCategory, const wxTreeItemId& item);

    /// @}


	/** @Change related functionss */
	/// @{

    /**
		@brief Updates the information of a category or product based on the item type.
		@param item - Tree item ID of the category or product to update.
		@param esCategoria - Indicates if the item is a category (true) or a product (false).
    **/
    void ChangeInfo(const wxTreeItemId& item, const bool& esCategoria);

    /// @}


	/** @Stock related functionss */
	/// @{

     /**
		 @brief Manages the stock of a selected product, allowing the user to view and update stock levels.
		 @param item - Tree item ID of the product whose stock will be managed.
     **/
	void StockProduct(const wxTreeItemId& item);

    /// @}


    /** @Search related functionss */
    /// @{

    /**
		@brief Searches for categories matching the search term and stores their tree item IDs.
        @param termino - Word to search for
        @param itemCategorias - wxTreeItemId vector where tree items id are loaded
    **/
    void SearchCategories(wxString termino, std::vector<wxTreeItemId>& itemCategorias);
    /**
		@brief Searches for products matching the search term and stores their tree item IDs.
		@param termino       - Word to search for
		@param itemProductos - wxTreeItemId vector where tree items id are loaded
    **/
    void SearchProducts(wxString termino, std::vector<wxTreeItemId>& itemProductos);
    /**
		@brief Loads the filtered tree from the database based on the search term.
		@param searchTerm - Term to search for
    **/
    void LoadFilteredTreeFromDB(const wxString& searchTerm);
    /**
		@brief Adds a product to the filtered tree under the specified category.
		@param product - Product object to add
		@param category_id - ID of the category under which to add the product
    **/
    void AddProductToFilteredTree(const std::shared_ptr<Producto>& product, size_t category_id);

    /// @}

    enum {
        ID_TEXTO_BUSQUEDA = wxID_HIGHEST + 100,
        ID_BUSCAR,
        ID_LIMPIAR_BUSQUEDA,
        ID_AGREGAR_CATEGORIA = 1,
        ID_AGREGAR_PRODUCTO,
        ID_ELIMINAR,
        ID_MODIFICAR,
        ID_GESTIONAR_STOCK,
        ID_ARBOL,
        ID_GUARDAR,
        ID_CERRAR_DESCARTAR
    };

};
