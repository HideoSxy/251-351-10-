#include "stego.h"

#include <png.h>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

// Вспомогательные типы

struct PngImage {
    uint32_t width      = 0;
    uint32_t height     = 0;
    int      color_type = 0;
    int      bit_depth  = 0;
    std::vector<std::vector<uint8_t>> rows;

    int channels() const {
        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY:       return 1;
            case PNG_COLOR_TYPE_GRAY_ALPHA: return 2;
            case PNG_COLOR_TYPE_RGB:        return 3;
            case PNG_COLOR_TYPE_RGBA:       return 4;
            default:                        return 0;
        }
    }

    uint64_t total_channels() const {
        return static_cast<uint64_t>(width) * height * channels();
    }
};

// Утилиты для QString → std::string (для fopen / cerr / runtime_error)

static std::string qs(const QString& s) {
    return s.toStdString();
}

// Чтение / запись PNG через libpng

static PngImage png_load(const QString& path) {
    // fopen требует const char* — конвертируем через UTF-8
    FILE* fp = std::fopen(path.toUtf8().constData(), "rb");
    if (!fp)
        throw std::runtime_error("Cannot open file: " + qs(path));

    uint8_t sig[8];
    if (std::fread(sig, 1, 8, fp) != 8 || png_sig_cmp(sig, 0, 8)) {
        std::fclose(fp);
        throw std::runtime_error("Not a valid PNG: " + qs(path));
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             nullptr, nullptr, nullptr);
    if (!png) {
        std::fclose(fp);
        throw std::runtime_error("png_create_read_struct failed");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        std::fclose(fp);
        throw std::runtime_error("png_create_info_struct failed");
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        std::fclose(fp);
        throw std::runtime_error("libpng read error");
    }

    png_init_io(png, fp);
    png_set_sig_bytes(png, 8);
    png_read_info(png, info);

    PngImage img;
    img.width      = png_get_image_width(png, info);
    img.height     = png_get_image_height(png, info);
    img.color_type = png_get_color_type(png, info);
    img.bit_depth  = png_get_bit_depth(png, info);

    if (img.bit_depth == 16)
        png_set_strip_16(png);
    if (img.color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png);
    if (img.bit_depth < 8)
        png_set_expand(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    png_read_update_info(png, info);

    img.color_type = png_get_color_type(png, info);
    img.bit_depth  = png_get_bit_depth(png, info);
    img.width      = png_get_image_width(png, info);
    img.height     = png_get_image_height(png, info);

    int rowbytes = static_cast<int>(png_get_rowbytes(png, info));
    img.rows.resize(img.height, std::vector<uint8_t>(rowbytes));

    std::vector<png_bytep> row_ptrs(img.height);
    for (uint32_t y = 0; y < img.height; ++y)
        row_ptrs[y] = img.rows[y].data();

    png_read_image(png, row_ptrs.data());
    png_read_end(png, nullptr);
    png_destroy_read_struct(&png, &info, nullptr);
    std::fclose(fp);
    return img;
}

static void png_save(const QString& path, const PngImage& img) {
    FILE* fp = std::fopen(path.toUtf8().constData(), "wb");
    if (!fp)
        throw std::runtime_error("Cannot write file: " + qs(path));

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              nullptr, nullptr, nullptr);
    if (!png) {
        std::fclose(fp);
        throw std::runtime_error("png_create_write_struct failed");
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        std::fclose(fp);
        throw std::runtime_error("png_create_info_struct failed");
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        std::fclose(fp);
        throw std::runtime_error("libpng write error");
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info,
                 img.width, img.height,
                 img.bit_depth,
                 img.color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    std::vector<png_bytep> row_ptrs(img.height);
    for (uint32_t y = 0; y < img.height; ++y)
        row_ptrs[y] = const_cast<uint8_t*>(img.rows[y].data());

    png_write_image(png, row_ptrs.data());
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    std::fclose(fp);
}

// LSB-итератор

struct LsbIterator {
    PngImage& img;
    int       ch;
    int       data_ch;
    uint64_t  pos = 0;

    explicit LsbIterator(PngImage& img_) : img(img_) {
        ch = img.channels();
        data_ch = (img.color_type == PNG_COLOR_TYPE_RGBA ||
                   img.color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ? ch - 1 : ch;
    }

    uint64_t capacity_bits() const {
        return static_cast<uint64_t>(img.width) * img.height * data_ch;
    }

    uint8_t& byte_at(uint64_t bit_idx) {
        uint64_t pixel_idx   = bit_idx / data_ch;
        int      channel_idx = static_cast<int>(bit_idx % data_ch);
        uint32_t row         = static_cast<uint32_t>(pixel_idx / img.width);
        uint32_t col         = static_cast<uint32_t>(pixel_idx % img.width);
        return img.rows[row][col * ch + channel_idx];
    }

    void write_bit(bool bit) {
        uint8_t& b = byte_at(pos++);
        b = (b & 0xFE) | (bit ? 1 : 0);
    }

    bool read_bit() {
        return (byte_at(pos++) & 1) != 0;
    }

    void write_u32(uint32_t val) {
        for (int i = 31; i >= 0; --i)
            write_bit((val >> i) & 1);
    }

    uint32_t read_u32() {
        uint32_t val = 0;
        for (int i = 31; i >= 0; --i)
            val |= (static_cast<uint32_t>(read_bit()) << i);
        return val;
    }
};

// Маркер

static constexpr uint32_t MAGIC = 0x53544547; // "STEG"

// Публичный API

bool fn_embed(const QString& image_path,
              const QString& output_path,
              const QString& msg)
{
    try {
        PngImage img = png_load(image_path);
        LsbIterator it(img);

        // QString хранит UTF-16. Конвертируем в UTF-8 байты — они и будут
        // встраиваться. При извлечении выполним обратную конвертацию.
        const QByteArray payload  = msg.toUtf8();
        const uint32_t   msg_len  = static_cast<uint32_t>(payload.size());
        const uint64_t   bits_needed = 64ULL + static_cast<uint64_t>(msg_len) * 8;

        if (bits_needed > it.capacity_bits()) {
            std::cerr << "[stego] Image too small: need "
                      << bits_needed << " bits, have "
                      << it.capacity_bits() << " bits.\n";
            return false;
        }

        it.write_u32(MAGIC);
        it.write_u32(msg_len);

        // Итерируемся по байтам QByteArray — всё корректно, никаких QChar
        for (unsigned char c : payload)
            for (int i = 7; i >= 0; --i)
                it.write_bit((c >> i) & 1);

        png_save(output_path, img);
        std::cout << "[stego] Embedded " << msg_len
                  << " byte(s) into \"" << qs(output_path) << "\".\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[stego] fn_embed error: " << e.what() << "\n";
        return false;
    }
}

QString fn_extract(const QString& image_path)
{
    try {
        PngImage img = png_load(image_path);
        LsbIterator it(img);

        if (it.capacity_bits() < 64) {
            std::cerr << "[stego] Image too small to contain a header.\n";
            return {};
        }

        const uint32_t magic = it.read_u32();
        if (magic != MAGIC) {
            std::cerr << "[stego] No hidden message found (bad magic).\n";
            return {};
        }

        const uint32_t msg_len = it.read_u32();
        if (static_cast<uint64_t>(msg_len) * 8 + 64 > it.capacity_bits()) {
            std::cerr << "[stego] Corrupt header: declared length exceeds image capacity.\n";
            return {};
        }

        // Собираем сырые UTF-8 байты в QByteArray
        QByteArray bytes(static_cast<int>(msg_len), '\0');
        for (uint32_t i = 0; i < msg_len; ++i) {
            uint8_t c = 0;
            for (int b = 7; b >= 0; --b)
                c |= (static_cast<uint8_t>(it.read_bit()) << b);
            bytes[static_cast<int>(i)] = static_cast<char>(c);
        }

        std::cout << "[stego] Extracted " << msg_len
                  << " byte(s) from \"" << qs(image_path) << "\".\n";

        // Конвертируем UTF-8 байты обратно в QString
        return QString::fromUtf8(bytes);

    } catch (const std::exception& e) {
        std::cerr << "[stego] fn_extract error: " << e.what() << "\n";
        return {};
    }
}
