#include "gui/UsuariosFrame/UsuariosFrame.h"
#include "utils/ValidateStringInput.h"
#include "sqlite_modern_cpp.h"
#include "constants/DB/DB.h"
#include <wx/wx.h>
#include <iostream>
#include "utils/GetFromFile.h"
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"
using namespace WARNING_MESSAGES;

void UsuariosFrame::CreateOrConnectDB() {
    try {
        // Ruta explÃÂ­cita relativa al ejecutable
        sqlite::database db(GetDBPath());

        // Configurar SQLite para usar UTF-8
        db << "PRAGMA encoding = 'UTF-8';";

        // Crear tabla si no existe (opcional)
        db << "CREATE TABLE IF NOT EXISTS usuarios ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "nombre TEXT NOT NULL UNIQUE);";

    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error connecting to the database: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}

void UsuariosFrame::GetUsuariosFromDB() {
    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";

        // Limpiar la tabla antes de cargar nuevos datos
        grid->ClearGrid();
        if (grid->GetNumberRows() > 0) {
            grid->DeleteRows(0, grid->GetNumberRows());
        }

        // Cargar usuarios con ID desde la base de datos
        db << "SELECT id, nombre FROM usuarios ORDER BY id;"
            >> [&](int id, std::string nombre) {
            int row = grid->GetNumberRows();
			//wxMessageBox("Numero de row: " + wxString::Format("%d", row), "Debug", wxOK | wxICON_INFORMATION, this);
            grid->AppendRows(1);
            // Columna 0: ID (oculta)
            grid->SetCellValue(row, 0, wxString::Format("%d", id));
            grid->SetReadOnly(row, 0, true);
            // Columna 1: Nombre (visible)
            grid->SetCellValue(row, 1, wxString::FromUTF8(nombre));
            grid->SetReadOnly(row, 1, true); // Solo lectura para el nombre tambiÃÂ©n
            };
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error loading users: %s", e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}

void UsuariosFrame::OnAgregarUsuario(wxCommandEvent& event) {
    wxString nombreWx = nombreInput->GetValue().Trim();
    if (nombreWx.IsEmpty()) {
        wxMessageBox(_("Name can not be empty"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // Convertir wxString a UTF-8 correctamente
    std::string nombre = nombreWx.ToUTF8().data();

    // Aplicar validaciÃÂ³n despuÃÂ©s de la conversiÃÂ³n
    nombre = LimpiarYValidarNombre(nombre);

    try {
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "SELECT nombre FROM usuarios WHERE nombre = ?;" << nombre
            >> [&](std::string existingNombre) {
                if (nombre == existingNombre) {
                    throw std::runtime_error(_("User name already exists, choose another one.."));
				}
			};

    }
    catch(const std::runtime_error& e) {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);
        return;
	}
    catch(const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error validating name: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
        return;
	}

    try {
        sqlite::database db(GetDBPath());

        // Asegurar que SQLite usa UTF-8
        db << "PRAGMA encoding = 'UTF-8';";

        db << "INSERT INTO usuarios (nombre) VALUES (?);" << nombre;

        // Actualizar la tabla
        GetUsuariosFromDB();
        nombreInput->Clear(); // Limpiar el campo de entrada
        wxMessageBox(_("User added successfully"), _("Success"), wxOK | wxICON_INFORMATION, this);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error adding user: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }

    
}

void UsuariosFrame::OnEliminarusuario(wxCommandEvent& event) {
	int filaSeleccionada = grid->GetGridCursorRow(); //Toma el indice de la fila seleccionada en el grid

    // Validar que haya una fila seleccionada
    if (filaSeleccionada == wxNOT_FOUND || grid->GetNumberRows() == 0) {
        wxMessageBox(_("No selected user to delete."), WARNING, wxOK | wxICON_WARNING, this);
        return;
    }

    wxString usuarioEliminar = grid->GetCellValue(filaSeleccionada, 1);

	int respuesta = wxMessageBox(_("Are you sure you want to delete the user?\"") + usuarioEliminar + "\"?", _("Confirm Deletion"), wxYES_NO | wxICON_QUESTION, this);
    if(respuesta != wxYES) return; // Cancelar la eliminaciÃÂ³n si el usuario no confirma
	

    // Obtener ID de la fila seleccionada (columna 0)
    wxString idStr = grid->GetCellValue(filaSeleccionada, 0);

    long id;
    if (!idStr.ToLong(&id)) {
        wxMessageBox(_("Invalid user ID selected"), "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    std::string actualUser = getUserFromJSON<std::string>();
    wxString actualUserWx = wxString::FromUTF8(actualUser);
	// Prevenir eliminar el usuario en sesiÃÂ³n
    if (actualUserWx != "Ninguno" && usuarioEliminar == actualUserWx) {
        wxMessageBox(_("You cannot delete a user who is currently logged in."), "Error", wxOK | wxICON_ERROR, this);
        return;
    }
    
    try {
        // Conectar a la base de datos y eliminar
        sqlite::database db(GetDBPath());
        db << "PRAGMA encoding = 'UTF-8';";
        db << "DELETE FROM usuarios WHERE id = ?;" << id;

        // Eliminar del grid
        grid->DeleteRows(filaSeleccionada);

        wxMessageBox(_("User successfully removed."), _("Success"), wxOK | wxICON_INFORMATION, this);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format(_("Error deleting user: %s"), e.what()), "Error", wxOK | wxICON_ERROR, this);
    }
}