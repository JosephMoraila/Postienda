#include "gui/UsuariosFrame/UsuariosFrame.h"
#include "gui/MainFrame/MainFrame.h"
#include "third_party/json.hpp"
#include <fstream>
#include "utils/ValidateStringInput.h"
#include "constants/JSON/Session/Session.h"
#include "utils/GetFromFile.h"
#include <sstream>
#include "constants/APPNAME/APPNAME.h"
#include "constants/MESSAGES_ADVICE/WARNING/WARNING_MESSAGES.hpp"
using namespace WARNING_MESSAGES;
using json = nlohmann::json;

void UsuariosFrame::CrearJSONSesion() {
	std::ifstream archivoIn(JSON_SESSION_PATH);
	json sesionJson;
	bool necesitaCreacion = true;

	if (archivoIn.is_open()) {
		try {
			archivoIn >> sesionJson; // Lee todo el contenido del archivo y lo parsea como JSON
			if (sesionJson.contains("session") && sesionJson["session"].is_string()) {
				necesitaCreacion = false;
			}
		}
		catch (...) {
			necesitaCreacion = true;
		}
		archivoIn.close();
	}

	if (necesitaCreacion) {
		sesionJson = { {"session", ""} };
	}

	std::ofstream archivoOut(JSON_SESSION_PATH);
	if (archivoOut.is_open()) {
		archivoOut << std::setw(4) << sesionJson << std::endl;
		archivoOut.close();
	}
}

void UsuariosFrame::getSessionFromJSON() {
	wxString usuario = getUserFromJSON<wxString>();
	if(usuario == "Ninguno" || usuario == "") {
		nombreUsuarioSeleccionado = _("None");
		sesionLabel->SetLabel(_("SESSION: ") + nombreUsuarioSeleccionado);
	}
	else {
		nombreUsuarioSeleccionado = usuario;
		sesionLabel->SetLabel(_("SESSION: ") + nombreUsuarioSeleccionado);
	}
}

void UsuariosFrame::OnIniciarSesion(wxCommandEvent& event) {
	int filaSeleccionada = grid->GetGridCursorRow();

	// Validar que haya una fila seleccionada
	if (filaSeleccionada == wxNOT_FOUND || grid->GetNumberRows() == 0) {
		wxMessageBox(_("No user selected to log in"), WARNING, wxOK | wxICON_WARNING, this);
		return;
	}

	// Obtener el nombre del usuario seleccionado
	wxString userName = grid->GetCellValue(filaSeleccionada, 1);

	// Convertir wxString a std::string para JSON
	std::string usuarioStr = userName.utf8_string();

	// Leer JSON existente (o crear nuevo si no existe o estÃÂ¡ mal)
	json sesionJson;
	std::ifstream archivoIn(JSON_SESSION_PATH);
	if (archivoIn.is_open()) {
		try {
			archivoIn >> sesionJson;
		}
		catch (...) {
			nombreUsuarioSeleccionado = _("None");
			sesionLabel->SetLabel(_("None: ") + nombreUsuarioSeleccionado);
			archivoIn.close();
			CrearJSONSesion(); // Si hay error, crear el JSON de sesiÃÂ³n
		}
		archivoIn.close();
	}

	// Reemplazar el valor de "session"
	sesionJson["session"] = usuarioStr;

	// Guardar el nuevo JSON
	std::ofstream archivoOut(JSON_SESSION_PATH);
	if (archivoOut.is_open()) {
		archivoOut << std::setw(4) << sesionJson << std::endl;
		archivoOut.close();
	}
	nombreUsuarioSeleccionado = userName;
	mainFramePun->SetTitle(APPNAME + " <"+nombreUsuarioSeleccionado+">");
	sesionLabel->SetLabel(_("SESSION: ") + nombreUsuarioSeleccionado);
	// ConfirmaciÃÂ³n (opcional)
	wxMessageBox(_("Session started as: ") + userName, _("Session"), wxOK | wxICON_INFORMATION, this);
}

void UsuariosFrame::OnLogOut(wxCommandEvent& event) {
	// Leer JSON existente (o crear nuevo si no existe o estÃÂ¡ mal)
	json sesionJson;
	std::ifstream archivoIn(JSON_SESSION_PATH);
	if (archivoIn.is_open()) {
		try {
			archivoIn >> sesionJson;
		}
		catch (...) {
			nombreUsuarioSeleccionado = _("None");
			mainFramePun->SetTitle(APPNAME);
			sesionLabel->SetLabel(_("SESSION: ") + nombreUsuarioSeleccionado);
			archivoIn.close();
			CrearJSONSesion(); // Si hay error, crear el JSON de sesiÃÂ³n
		}
		archivoIn.close();
	}

	// Reemplazar el valor de "session"
	sesionJson["session"] = "";

	// Guardar el nuevo JSON
	std::ofstream archivoOut(JSON_SESSION_PATH);
	if (archivoOut.is_open()) {
		archivoOut << std::setw(4) << sesionJson << std::endl;
		archivoOut.close();
	}
	nombreUsuarioSeleccionado = _("None");
	mainFramePun->SetTitle(APPNAME);
	sesionLabel->SetLabel(_("SESSIO: ") + nombreUsuarioSeleccionado);
	// ConfirmaciÃÂ³n (opcional)
	wxMessageBox(_("You have logged out, so purchases will not have an assigned user."), "Info", wxOK | wxICON_INFORMATION, this);
}
