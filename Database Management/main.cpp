#include <iostream>
#include <windows.h>
#include <mysql.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

class DatabaseManager {
private:
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;

    string encrypt(string text) {
        char key = 'K'; 
        for (size_t i = 0; i < text.size(); ++i) {
            text[i] ^= key;
        }
        
        stringstream ss;
        for(unsigned char c : text) {
            ss << hex << setw(2) << setfill('0') << (int)c;
        }
        return ss.str();
    }

    string decrypt(string hexText) {
        string text = "";
        for (size_t i = 0; i < hexText.length(); i += 2) {
            string byteString = hexText.substr(i, 2);
            char byte = (char)strtol(byteString.c_str(), nullptr, 16);
            text += byte;
        }
        
        char key = 'K';
        for (size_t i = 0; i < text.size(); ++i) {
            text[i] ^= key;
        }
        return text;
    }

    string escapeString(string input) {
        if (!conn) return input;
        char* escaped = new char[input.length() * 2 + 1];
        mysql_real_escape_string(conn, escaped, input.c_str(), input.length());
        string result = escaped;
        delete[] escaped;
        return result;
    }

public:
    DatabaseManager() : conn(nullptr), row(nullptr), res(nullptr) {
        conn = mysql_init(nullptr);
        conn = mysql_real_connect(conn, "localhost", "root", "", "school_db", 3306, nullptr, 0);
        
        if (conn) {
            cout << "[Success] Connected to school_db!" << endl;
        } else {
            cout << "[Error] DB Connection Failed: " << mysql_error(conn) << endl;
        }
    }

    void searchStudent(string nrpInput, bool isAdmin) {
        if (!conn) return;

        string safeNRP = escapeString(nrpInput);
        string query = "SELECT * FROM students_final WHERE nrp = '" + safeNRP + "'";

        if (mysql_query(conn, query.c_str()) == 0) {
            res = mysql_store_result(conn);
            
            if (res && (row = mysql_fetch_row(res))) {
                cout << "\n=== STUDENT PROFILE ===" << endl;
                cout << "NRP:   " << row[0] << endl;
                cout << "Name:  " << row[1] << endl;
                cout << "Prodi: " << row[2] << endl;
                cout << "Agama: " << row[3] << endl;
                cout << "Kota:  " << row[4] << endl;
                
                if (isAdmin) {

                    cout << "[SECURE] Medical History :   " << decrypt(row[7]) << " (Decrypted)" << endl; 
                    cout << "[SECURE] Emergency Contact: " << decrypt(row[8]) << " (Decrypted)" << endl;
                    cout << "Address: " << row[5] << endl;
                    cout << "Phone:   " << row[6] << endl;
                } else {
                    cout << "Medical:   [REDACTED]" << endl;
                    cout << "Emergency: [REDACTED]" << endl;
                }
            } else {
                cout << "Student not found." << endl;
            }
            if(res) mysql_free_result(res);
        } else {
            cout << "Query failed: " << mysql_error(conn) << endl;
        }
    }

    void deleteAllData() {
        if (!conn) return;
        
        string query = "TRUNCATE TABLE students_final";
        
        if (mysql_query(conn, query.c_str()) == 0) {
            cout << "[Success] All data deleted from students_final." << endl;
        } else {
            cout << "[Error] Failed to delete data: " << mysql_error(conn) << endl;
        }
    }

    void importCSV(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "[Error] Could not find " << filename << endl;
            return;
        }

        string line;
        int count = 0;
        int totalLines = 0;
        cout << "Reading " << filename << "..." << endl;

        while (getline(file, line)) {
            totalLines++;
            vector<string> data;
            string currentField = "";
            bool inQuotes = false;
            
            for (char c : line) {
                if (c == '\"') {
                    inQuotes = !inQuotes;
                } else if (c == ',' && !inQuotes) {
                    data.push_back(currentField);
                    currentField = "";
                } else {
                    currentField += c;
                }
            }
            data.push_back(currentField); 

            if (data.size() >= 9) {
                string encDisease = encrypt(data[7]);
                string encContact = encrypt(data[8]);

                string nrp = escapeString(data[0]);
                string nama = escapeString(data[1]);
                string prodi = escapeString(data[2]);
                string agama = escapeString(data[3]);
                string kota = escapeString(data[4]);
                string alamat = escapeString(data[5]);
                string telp = escapeString(data[6]);

                string query = "INSERT INTO students_final (nrp, nama, prodi, agama, asal_kota, alamat_surabaya, telp_pribadi, riwayat_penyakit_enc, kontak_darurat_enc) VALUES ('" 
                               + nrp + "', '" + nama + "', '" + prodi + "', '" + agama + "', '" + kota + "', '" + alamat + "', '" + telp + "', '" + encDisease + "', '" + encContact + "')";

                if (mysql_query(conn, query.c_str()) == 0) {
                    count++;
                } else {
                    cout << "[Warning] Failed to import NRP " << data[0] << ": " << mysql_error(conn) << endl;
                }
            }
        }
        cout << "[Success] Processed " << totalLines << " lines. Imported " << count << " new students." << endl;
    }

    ~DatabaseManager() {
        if (res) mysql_free_result(res);
        if (conn) mysql_close(conn);
    }
};

int main() {
    DatabaseManager db;
    int choice;
    string nrp;

    while (true) {
        cout << "\n--- STUDENT DATABASE SYSTEM ---" << endl;
        cout << "1. Import CSV to DB" << endl;
        cout << "2. Search Student (Public)" << endl;
        cout << "3. Search Student (Admin)" << endl;
        cout << "4. Delete ALL Data" << endl;
        cout << "5. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: db.importCSV("students_sample.csv"); break;
            case 2: 
                cout << "Enter NRP: "; cin >> nrp;
                db.searchStudent(nrp, false); 
                break;
            case 3: 
                cout << "Enter NRP: "; cin >> nrp;
                db.searchStudent(nrp, true); 
                break;
            case 4: db.deleteAllData(); break;
            case 5: return 0;
        }
    }
}