#include <wx/wx.h>
#include <wx/grid.h>

class MainFrame; // Forward declaration to avoid circular dependency

/**
 * @class UsuariosFrame
 * @brief Secondary window for user management in the system.
 * @details Provides functionality to create, delete, and display users
 * from a database. It also manages the active session using JSON
 * and applies light/dark themes.
 *
 * Inherits from wxFrame.
 */
class UsuariosFrame : public wxFrame
{
public:
    /**
     * @brief Constructor for the users window.
     * @param parent Parent window.
     * @param mainFramePtr Pointer to the main window for communication.
     */
    UsuariosFrame(wxWindow* parent, MainFrame* mainFramePtr);

    /**
     * @brief Applies light or dark theme to the window and its child widgets.
     * @param oscuro true to enable dark theme, false for light theme.
     */
    void AplicarTema(bool oscuro);

private:
    MainFrame* mainFramePun; ///< Pointer to the main window to access its methods and attributes.

    /** @name Widgets principales */
    /// @{
    wxPanel* mainPanel = nullptr; ///< Main panel containing all widgets.
    wxGrid* grid = nullptr; ///< Grid to display the users loaded from the database.
    wxTextCtrl* nombreInput = nullptr; ///< Text input field for adding a new user.
    wxStaticText* sesionLabel; ///< Label that graphically indicates the user currently logged in.
    wxString nombreUsuarioSeleccionado = _("None"); ///< Name of the selected user for the session (default: "None").
    bool temaOscuro = false; ///< Current theme state (true = dark, false = light).

    /**
     * @brief Creates and arranges the main widgets in the window.
     */
    void Widgets();

    /**
     * @brief Event handler: triggered when the mouse enters a button.
     * @param event Standard wxWidgets mouse event.
     */
    void OnButtonEnter(wxMouseEvent& event);

    /**
     * @brief Event handler: triggered when the mouse leaves a button.
     * @param event Standard wxWidgets mouse event.
     */
    void OnButtonLeave(wxMouseEvent& event);
    /// @}


    /** @name Database/table */
    /// @{

    /**
     * @brief Creates the database or connects if it already exists.
     */
    void CreateOrConnectDB();

    /**
     * @brief Event handler: adds a user to the table database using the entered text.
     * @param event Standard wxWidgets command event.
     */
    void OnAgregarUsuario(wxCommandEvent& event);

    /**
     * @brief Loads all users from the table database and displays them in the grid.
     */
    void GetUsuariosFromDB();

    /**
     * @brief Event handler: removes the selected user from the table database.
     * @param event Standard wxWidgets command event.
     */
    void OnEliminarusuario(wxCommandEvent& event);

    /// @}
 

    /** @name Session JSON  */
    /// @{

    /**
	 * @brief Verifies if the JSON file exits or if it is malformed, and creates it if necessary with blank user value.
     */
    void CrearJSONSesion();

    /**
	 * @brief Loads the session from the JSON file, setting the active user to be displayed in the label.
     */
    void getSessionFromJSON();

    /**
	 * @brief Event handler: starts a session with the currently selected user from the grid.
     * @param event Standard wxWidgets command event.
     */
    void OnIniciarSesion(wxCommandEvent& event);

    /**
	 * @brief Event handler: logs out the current session and resets the state. And the value in the JSON to blank.
     * @param event Standard wxWidgets command event.
     */
    void OnLogOut(wxCommandEvent& event);

    /// @}
};
