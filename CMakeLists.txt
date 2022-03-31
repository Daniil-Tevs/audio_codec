cmake_minimum_required(VERSION 3.20)
project(audio_codec)


set(CMAKE_CXX_STANDARD 14)
include_directories("speex-1.2.0/")
include_directories("speex-1.2.0/include/speex/")
include_directories("speex-1.2.0/libspeex/")
include_directories("speex-1.2.0/win32/")
add_library(MyLibrary STATIC speex-1.2.0/include/speex/speex.h
        speex-1.2.0/include/speex/speex_bits.h
        speex-1.2.0/include/speex/speex_stereo.h
        speex-1.2.0/include/speex/speex_types.h
        speex-1.2.0/include/speex/speex_header.h
        speex-1.2.0/win32/config.h
        speex-1.2.0/libspeex/bits.c
        speex-1.2.0/libspeex/cb_search.c
        speex-1.2.0/libspeex/exc_5_64_table.c
        speex-1.2.0/libspeex/exc_5_256_table.c
        speex-1.2.0/libspeex/exc_8_128_table.c
        speex-1.2.0/libspeex/exc_10_16_table.c
        speex-1.2.0/libspeex/exc_10_32_table.c
        speex-1.2.0/libspeex/exc_20_32_table.c
        speex-1.2.0/libspeex/filters.c
        speex-1.2.0/libspeex/gain_table.c
        speex-1.2.0/libspeex/gain_table_lbr.c
        speex-1.2.0/libspeex/hexc_10_32_table.c
        speex-1.2.0/libspeex/hexc_table.c
        speex-1.2.0/libspeex/high_lsp_tables.c
        speex-1.2.0/libspeex/kiss_fft.c
        speex-1.2.0/libspeex/kiss_fftr.c
        speex-1.2.0/libspeex/lpc.c
        speex-1.2.0/libspeex/lsp.c
        speex-1.2.0/libspeex/lsp_tables_nb.c
        speex-1.2.0/libspeex/ltp.c
        speex-1.2.0/libspeex/modes.c
        speex-1.2.0/libspeex/modes_wb.c
        speex-1.2.0/libspeex/nb_celp.c
        speex-1.2.0/libspeex/quant_lsp.c
        speex-1.2.0/libspeex/sb_celp.c
        speex-1.2.0/libspeex/smallft.c
        speex-1.2.0/libspeex/speex.c
        speex-1.2.0/libspeex/speex_callbacks.c
        speex-1.2.0/libspeex/speex_header.c
        speex-1.2.0/libspeex/stereo.c
        speex-1.2.0/libspeex/testenc.c
        speex-1.2.0/libspeex/testenc_uwb.c
        speex-1.2.0/libspeex/testenc_wb.c
        speex-1.2.0/libspeex/vbr.c
        speex-1.2.0/libspeex/vorbis_psy.c
        speex-1.2.0/libspeex/vq.c
        speex-1.2.0/libspeex/window.c)
add_executable(audio_codec main.cpp
        )
target_link_libraries(audio_codec MyLibrary)