CREATE DATABASE IF NOT EXISTS school_db;
USE school_db;

CREATE TABLE IF NOT EXISTS students_final (
    nrp VARCHAR(20) PRIMARY KEY,
    nama VARCHAR(100),
    prodi VARCHAR(50),
    agama VARCHAR(20),
    asal_kota VARCHAR(50),
    alamat_surabaya TEXT,
    telp_pribadi VARCHAR(20),
    riwayat_penyakit_enc TEXT,
    kontak_darurat_enc TEXT
);
