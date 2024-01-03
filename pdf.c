#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 16384
int compress_file(const char *source, gzFile dest) {
    FILE *source_file = fopen(source, "rb");
    if (!source_file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", source);
        return 1;
    }

    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    // Inicializar a estrutura de compressão zlib
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
        fprintf(stderr, "Erro ao inicializar a compressão zlib\n");
        fclose(source_file);
        return 1;
    }

    // Definir o arquivo de origem como entrada
    strm.next_in = Z_NULL;
    strm.avail_in = 0;

    // Loop para comprimir e escrever no arquivo de destino
    do {
        strm.avail_in = fread(in, 1, CHUNK, source_file);
        if (ferror(source_file)) {
            fprintf(stderr, "Erro ao ler o arquivo %s\n", source);
            deflateEnd(&strm);
            fclose(source_file);
            return 1;
        }

        strm.next_in = in;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            // Comprimir
            ret = deflate(&strm, Z_FINISH);
            if (ret == Z_STREAM_ERROR) {
                fprintf(stderr, "Erro ao comprimir o arquivo %s\n", source);
                deflateEnd(&strm);
                fclose(source_file);
                return 1;
            }

            have = CHUNK - strm.avail_out;

            // Escrever no arquivo de destino
            if (gzwrite(dest, out, have) != have) {
                fprintf(stderr, "Erro ao escrever no arquivo comprimido\n");
                deflateEnd(&strm);
                fclose(source_file);
                return 1;
            }
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    // Finalizar a compressão
    deflateEnd(&strm);

    // Fechar o arquivo de origem
    fclose(source_file);

    return 0;
}


void generate_meta_xml(const char *author, const char *title) {
    FILE *meta_file = fopen("meta.xml", "w");

    fprintf(meta_file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(meta_file, "<office:document-meta xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" office:version=\"1.2\">\n");
    fprintf(meta_file, "  <office:meta>\n");
    fprintf(meta_file, "    <meta:initial-creator>%s</meta:initial-creator>\n", author);
    fprintf(meta_file, "    <dc:title>%s</dc:title>\n", title);
    fprintf(meta_file, "    <!-- Adicione outras propriedades conforme necessário -->\n");
    fprintf(meta_file, "  </office:meta>\n");
    fprintf(meta_file, "</office:document-meta>\n");

    fclose(meta_file);
}

int main() {
    char arquivo[100];
    char nome[100];
    char extensao[15];

    printf("Escreva o nome do arquivo a ser convertido: \n");
    scanf("%s", arquivo);

    strcpy(nome, arquivo);
    strcat(nome, ".odt");

    printf("Escreva a extensao do arquivo (.txt, .html): \n");
    scanf("%s", extensao);

    strcat(arquivo, extensao);

    FILE *fptr;
    fptr = fopen(arquivo, "rt");

    if (!fptr) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Criar e abrir o arquivo content.xml
    FILE *novoarquivo = fopen("content.xml", "w, ccs=ISO-8859-1");

     // Cabeçalho do arquivo ODT
    fprintf(novoarquivo, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    fprintf(novoarquivo, "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:ooo=\"http://openoffice.org/2004/office\" xmlns:ooow=\"http://openoffice.org/2004/writer\" xmlns:oooc=\"http://openoffice.org/2004/calc\" xmlns:dom=\"http://www.w3.org/2001/xml-events\" xmlns:rpt=\"http://openoffice.org/2005/report\" xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" xmlns:xhtml=\"http://www.w3.org/1999/xhtml\" xmlns:grddl=\"http://www.w3.org/2003/g/data-view#\" xmlns:tableooo=\"http://openoffice.org/2009/table\" xmlns:drawooo=\"http://openoffice.org/2010/draw\" xmlns:calcext=\"urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0\" xmlns:loext=\"urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0\" xmlns:field=\"urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0\" xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" xmlns:css3t=\"http://www.w3.org/TR/css3-text/\" office:version=\"1.2\" office:mimetype=\"application/vnd.oasis.opendocument.text\">\n");
    fprintf(novoarquivo, "<office:styles>\n");
    fprintf(novoarquivo, "<style:default-style style:family=\"paragraph\" style:name=\"Standard\">\n");
    fprintf(novoarquivo, "<style:paragraph-properties fo:hyphenation-ladder-count=\"no-limit\" style:tab-stop-distance=\"0.5in\" style:writing-mode=\"lr-tb\"/>\n");
    fprintf(novoarquivo, "<style:text-properties style:use-window-font-color=\"true\" fo:font-size=\"12pt\"/>\n");
    fprintf(novoarquivo, "</style:default-style>\n");
    fprintf(novoarquivo, "</office:styles>\n");
    fprintf(novoarquivo, "<office:automatic-styles>\n");
    fprintf(novoarquivo, "<style:page-layout style:name=\"pm1\">\n");
    fprintf(novoarquivo, "<style:page-layout-properties fo:page-width=\"8.5in\" fo:page-height=\"11in\" style:num-format=\"1\" style:writing-mode=\"lr-tb\" style:footnote-max-height=\"0.5in\" style:footnote-sep-min-height=\"0.25in\" style:script-type=\"western\"/>\n");
    fprintf(novoarquivo, "</style:page-layout>\n");
    fprintf(novoarquivo, "</office:automatic-styles>\n");
    fprintf(novoarquivo, "<office:body>\n");
    fprintf(novoarquivo, "<office:text>\n");
    fprintf(novoarquivo, "<text:p>\n");

    int c;
    while ((c = fgetc(fptr)) != EOF) {
        fputc(c, novoarquivo);
    }

    fprintf(novoarquivo, "\n</text:p>\n");
    fprintf(novoarquivo, "</office:text>\n");
    fprintf(novoarquivo, "</office:body>\n");
    fprintf(novoarquivo, "</office:document-content>\n");
    // Adicionar content.xml, meta.xml, settings.xml, styles.xml ao arquivo ZIP (.odt)
    gzFile zip_file = gzopen(nome, "wb");
    if (!zip_file) {
        fprintf(stderr, "Erro ao criar o arquivo ZIP %s\n", nome);
        fclose(novoarquivo);
        return 1;
    }

    printf("Tamanho de content.xml antes da compressao: %ld bytes\n", ftell(novoarquivo));

    const char *xml_files[] = {"content.xml", "meta.xml", "settings.xml", "styles.xml"};

    for (int i = 0; i < sizeof(xml_files) / sizeof(xml_files[0]); i++) {
        const char *xml_filename = xml_files[i];
        if (strcmp(xml_filename, "meta.xml") == 0) {
            // Gere o conteúdo do meta.xml
            generate_meta_xml("Seu Nome", nome);
            // Comprima outros arquivos XML
            if (compress_file(xml_filename, zip_file) != 0) {
                fprintf(stderr, "Erro ao comprimir o arquivo %s\n", xml_filename);
                gzclose(zip_file);
                fclose(novoarquivo);
                return 1;
            }
        }
    }

    printf("Tamanho de content.xml apos a compressao: %ld bytes\n", gzseek(zip_file, 0, SEEK_CUR));

    // Fechar os arquivos
    gzclose(zip_file);
    fclose(novoarquivo);

    printf("Arquivo ODT criado com sucesso: %s\n", nome);

    return 0;
}