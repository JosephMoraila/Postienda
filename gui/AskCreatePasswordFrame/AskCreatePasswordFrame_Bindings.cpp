#include "gui/AskCreatePasswordFrame/AskCreatePasswordFrame.hpp"
#include "constants/HASH/HASH.hpp"
#include <wx/wx.h>
#include <openssl/evp.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include "constants/PASSWORD/PASSWORD_FILE.hpp"

void AskCreatePasswordFrame::OnPasswordEnter(wxCommandEvent& event) {
    wxString password = textInputPassword->GetValue();
    wxString confirmPassword = textInputConfirmPassword->GetValue();

    if (password.IsEmpty()) {
        messageOnEnter->SetLabel(_("Password is empty"));
        return;
    }

    if (confirmPassword.IsEmpty()) {
        messageOnEnter->SetLabel(_("Confirm password is empty"));
        return;
    }

    if (password != confirmPassword) {
        messageOnEnter->SetLabel(_("Passwords are not the same"));
        return;
    }

    messageOnEnter->SetLabel("");

    // Convertir a UTF-8
    std::string passwordUtf8(password.utf8_str());

    try {
        HASH::PasswordHash hashed = HASH::hashPassword(passwordUtf8);

        std::string saltHex = HASH::toHex(hashed.salt);
        std::string hashHex = HASH::toHex(hashed.hash);

        saveUserPassword(saltHex, hashHex);

        // Limpieza de memoria sensible
        OPENSSL_cleanse(passwordUtf8.data(), passwordUtf8.size());

        messageOnEnter->SetLabel(_("Password stored securely"));
        EndModal(wxID_OK); // success

    }
    catch (const std::exception& e) {
        messageOnEnter->SetLabel(_("Error while processing password"));
    }
}


void AskCreatePasswordFrame::saveUserPassword(const std::string& saltHex, const std::string& hashHex) {
    std::ofstream file(GetPasswordPath(), std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open password file");
    }

    file << "salt=" << saltHex << "\n";
    file << "hash=" << hashHex << "\n";

    file.close();
}