#include <locale>
#include <iomanip>
#include <sstream>
#include "wx/wx.h"

/**
    @namespace DateTimeUtils
	@brief Contains utility functions for date and time manipulation.
**/
namespace DateTimeUtils {

     /**
		 @brief Formats a date-time string into localized date and time components.
		 @param  dateTimeStr - The input date-time string in ISO format (e.g., "2024-06-15 14:30:00")
		 @retval - A pair containing the formatted date and time as wxString in that order.
     **/
	inline std::pair<wxString, wxString> FormatDateTimeLocalized(const std::string& dateTimeStr) {
        // Establecer el locale del sistema
        std::setlocale(LC_TIME, "");
        // Separar fecha y hora
        wxString fechaHora = wxString::FromUTF8(dateTimeStr);
        wxArrayString partes = wxSplit(fechaHora, ' ');

        wxString fecha = partes.Count() > 0 ? partes[0] : wxString("");
        wxString hora = partes.Count() > 1 ? partes[1] : wxString("");

        // Parsear fecha
        wxDateTime dt;
        if (!dt.ParseISOCombined(fechaHora)) dt.ParseDate(fecha);

        wxString fechaFormateada;
        if (dt.IsValid()) {
            // Usa formato localizado del sistema
            wxString diaSemana = dt.Format("%A", wxDateTime::CET);
            wxString fechaLocal = dt.Format("%d/%B/%Y", wxDateTime::CET);

            fechaFormateada = diaSemana + ", " + fechaLocal;
        }
        else fechaFormateada = fecha;

		return std::make_pair(fechaFormateada, hora);
	}
}