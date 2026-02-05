#include "gui/AskEnterPasswordFrame/AskEnterPasswordFrame.hpp"
#include <fstream>
#include <stdexcept>
#include "constants/PASSWORD/PASSWORD_FILE.hpp"
#include "constants/HASH/HASH.hpp"



void AskEnterPasswordFrame::OnPasswordEnter(wxCommandEvent& event) {
	wxString password = textInputPassword->GetValue();
    std::string passwordUtf8(password.utf8_str());
    HASH::PasswordHash stored = loadUserPassword();
    bool ok = HASH::verifyPassword(passwordUtf8, stored);
    if(ok) EndModal(wxID_OK);
    else messageOnEnter->SetLabel(_("Incorrect password"));
}


HASH::PasswordHash AskEnterPasswordFrame::loadUserPassword() {
    std::ifstream file(GetPasswordPath());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open password file");
    }

    HASH::PasswordHash result;
    std::string line;


    while (std::getline(file, line)) {
        if (line.rfind("salt=", 0) == 0) {
            result.salt = HASH::fromHex(line.substr(5));
        }
        else if (line.rfind("hash=", 0) == 0) {
            result.hash = HASH::fromHex(line.substr(5));
        }
    }

    if (result.salt.empty() || result.hash.empty()) {
        throw std::runtime_error("Invalid password file format");
    }

    return result;
}