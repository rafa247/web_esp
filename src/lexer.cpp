#include "esp/lexer.hpp"
#include <cctype>
#include <unordered_map>

namespace esp {
namespace {

bool es_inicio_identificador(unsigned char c) {
    return std::isalpha(c) || c == '_' || c >= 128;
}

bool es_char_identificador(unsigned char c) {
    return std::isalnum(c) || c == '_' || c >= 128;
}

TipoToken palabra_clave(const std::string& lexema) {
    static const std::unordered_map<std::string, TipoToken> mapa{
        {"funcion", TipoToken::KwFuncion},
        {"function", TipoToken::KwFuncion},
        {"fn", TipoToken::KwFuncion},
        {"devolver", TipoToken::KwDevolver},
        {"return", TipoToken::KwDevolver},
        {"variable", TipoToken::KwVariable},
        {"var", TipoToken::KwVariable},
        {"constante", TipoToken::KwConstante},
        {"const", TipoToken::KwConstante},
        {"si", TipoToken::KwSi},
        {"if", TipoToken::KwSi},
        {"sino", TipoToken::KwSino},
        {"si_no", TipoToken::KwSino},
        {"else", TipoToken::KwSino},
        {"si_no_si", TipoToken::KwSiNoSi},
        {"elseif", TipoToken::KwSiNoSi},
        {"mientras", TipoToken::KwMientras},
        {"while", TipoToken::KwMientras},
        {"para", TipoToken::KwPara},
        {"for", TipoToken::KwPara},
        {"hasta", TipoToken::KwHasta},
        {"to", TipoToken::KwHasta},
        {"paso", TipoToken::KwPaso},
        {"step", TipoToken::KwPaso},
        {"en", TipoToken::KwEn},
        {"in", TipoToken::KwEn},
        {"fin", TipoToken::KwFin},
        {"end", TipoToken::KwFin},
        {"clase", TipoToken::KwClase},
        {"class", TipoToken::KwClase},
        {"hereda", TipoToken::KwHereda},
        {"extends", TipoToken::KwHereda},
        {"este", TipoToken::KwEste},
        {"this", TipoToken::KwEste},
        {"super", TipoToken::KwSuper},
        {"publico", TipoToken::KwPublico},
        {"publica", TipoToken::KwPublico},
        {"public", TipoToken::KwPublico},
        {"privado", TipoToken::KwPrivado},
        {"privada", TipoToken::KwPrivado},
        {"private", TipoToken::KwPrivado},
        {"conseguir", TipoToken::KwConseguir},
        {"importar", TipoToken::KwConseguir},
        {"como", TipoToken::KwComo},
        {"as", TipoToken::KwComo},
        {"exportar", TipoToken::KwExportar},
        {"export", TipoToken::KwExportar},
        {"intenta", TipoToken::KwIntenta},
        {"try", TipoToken::KwIntenta},
        {"si_no_puede", TipoToken::KwSiNoPuede},
        {"catch", TipoToken::KwSiNoPuede},
        {"romper", TipoToken::KwRomper},
        {"break", TipoToken::KwRomper},
        {"continuar", TipoToken::KwContinuar},
        {"continue", TipoToken::KwContinuar},
        {"datos", TipoToken::KwDatos},
        {"lista", TipoToken::KwLista},
        {"y", TipoToken::KwY},
        {"and", TipoToken::KwY},
        {"o", TipoToken::KwO},
        {"or", TipoToken::KwO},
        {"no", TipoToken::KwNo},
        {"not", TipoToken::KwNo},
        {"verdadero", TipoToken::Verdadero},
        {"true", TipoToken::Verdadero},
        {"falso", TipoToken::Falso},
        {"false", TipoToken::Falso},
        {"nulo", TipoToken::Nulo},
        {"null", TipoToken::Nulo},

        // Sintaxis Web Nativa en ESP
        {"pagina", TipoToken::KwPagina},
        {"estilo", TipoToken::KwEstilo},
        {"script", TipoToken::KwScript},
        {"encabezado", TipoToken::KwEncabezado},
        {"subtitulo", TipoToken::KwSubtitulo},
        {"parrafo", TipoToken::KwParrafo},
        {"boton", TipoToken::KwBoton},
        {"entrada", TipoToken::KwEntrada},
        {"enlace", TipoToken::KwEnlace},
        {"imagen", TipoToken::KwImagen},
        {"contenedor", TipoToken::KwContenedor},
        {"al_clic", TipoToken::KwAlClic},

        // Sintaxis SQL Nativa en ESP
        {"base_datos", TipoToken::KwBaseDatos},
        {"crear_tabla", TipoToken::KwCrearTabla},
        {"insertar_en", TipoToken::KwInsertarEn},
        {"valores", TipoToken::KwValores},
        {"seleccionar", TipoToken::KwSeleccionar},
        {"de", TipoToken::KwDe},
        {"donde", TipoToken::KwDonde},
        {"ordenar_por", TipoToken::KwOrdenarPor},
        {"limite", TipoToken::KwLimite},
        {"actualizar", TipoToken::KwActualizar},
        {"poner", TipoToken::KwPoner},
        {"eliminar_de", TipoToken::KwEliminarDe}
    };
    auto it = mapa.find(lexema);
    return it != mapa.end() ? it->second : TipoToken::Identificador;
}

}  // namespace

Lexer::Lexer(std::string fuente, std::string archivo)
    : fuente_(std::move(fuente)), archivo_(std::move(archivo)) {}

char Lexer::actual() const {
    return pos_ < fuente_.size() ? fuente_[pos_] : '\0';
}

char Lexer::siguiente() const {
    return pos_ + 1 < fuente_.size() ? fuente_[pos_ + 1] : '\0';
}

char Lexer::avanzar() {
    char c = actual();
    ++pos_;
    if (c == '\n') {
        ++linea_;
        columna_ = 1;
    } else {
        ++columna_;
    }
    return c;
}

void Lexer::saltar_espacios_y_comentarios() {
    while (pos_ < fuente_.size()) {
        char c = actual();
        if (c == ' ' || c == '\t' || c == '\r') {
            avanzar();
        } else if (c == '#') {
            while (pos_ < fuente_.size() && actual() != '\n') avanzar();
        } else if (c == '/' && siguiente() == '/') {
            while (pos_ < fuente_.size() && actual() != '\n') avanzar();
        } else if (c == '/' && siguiente() == '*') {
            Ubicacion ini{archivo_, linea_, columna_};
            avanzar();
            avanzar();
            bool cerrado = false;
            while (pos_ < fuente_.size()) {
                if (actual() == '*' && siguiente() == '/') {
                    avanzar();
                    avanzar();
                    cerrado = true;
                    break;
                }
                avanzar();
            }
            if (!cerrado) throw ErrorLexico("comentario multilinea sin cerrar", ini);
        } else {
            break;
        }
    }
}

Token Lexer::leer_numero() {
    Ubicacion u{archivo_, linea_, columna_};
    std::string lex;
    bool es_decimal = false;
    while (std::isdigit(static_cast<unsigned char>(actual()))) lex.push_back(avanzar());
    if (actual() == '.' && std::isdigit(static_cast<unsigned char>(siguiente()))) {
        es_decimal = true;
        lex.push_back(avanzar());
        while (std::isdigit(static_cast<unsigned char>(actual()))) lex.push_back(avanzar());
    }
    if (actual() == 'e' || actual() == 'E') {
        es_decimal = true;
        lex.push_back(avanzar());
        if (actual() == '+' || actual() == '-') lex.push_back(avanzar());
        if (!std::isdigit(static_cast<unsigned char>(actual())))
            throw ErrorLexico("exponente numerico no valido", u);
        while (std::isdigit(static_cast<unsigned char>(actual()))) lex.push_back(avanzar());
    }
    return Token{es_decimal ? TipoToken::Decimal : TipoToken::Entero, std::move(lex), std::move(u)};
}

Token Lexer::leer_texto() {
    Ubicacion u{archivo_, linea_, columna_};
    char comilla = avanzar();
    std::string texto;
    while (pos_ < fuente_.size() && actual() != comilla) {
        char c = avanzar();
        if (c == '\\') {
            if (pos_ >= fuente_.size()) throw ErrorLexico("secuencia de escape incompleta", u);
            char e = avanzar();
            if (e == 'n') texto.push_back('\n');
            else if (e == 't') texto.push_back('\t');
            else if (e == 'r') texto.push_back('\r');
            else if (e == '\\') texto.push_back('\\');
            else if (e == '"') texto.push_back('"');
            else if (e == '\'') texto.push_back('\'');
            else texto.push_back(e);
        } else {
            texto.push_back(c);
        }
    }
    if (pos_ >= fuente_.size()) throw ErrorLexico("cadena de texto sin cerrar", u);
    avanzar();
    return Token{TipoToken::Texto, std::move(texto), std::move(u)};
}

Token Lexer::leer_identificador() {
    Ubicacion u{archivo_, linea_, columna_};
    std::string lex;
    while (pos_ < fuente_.size() && es_char_identificador(static_cast<unsigned char>(actual()))) {
        lex.push_back(avanzar());
    }
    TipoToken tipo = palabra_clave(lex);
    return Token{tipo, std::move(lex), std::move(u)};
}

std::vector<Token> Lexer::analizar() {
    std::vector<Token> tokens;
    if (fuente_.size() >= 3 &&
        static_cast<unsigned char>(fuente_[0]) == 0xEF &&
        static_cast<unsigned char>(fuente_[1]) == 0xBB &&
        static_cast<unsigned char>(fuente_[2]) == 0xBF) {
        pos_ = 3;
    }

    while (pos_ < fuente_.size()) {
        saltar_espacios_y_comentarios();
        if (pos_ >= fuente_.size()) break;

        Ubicacion u{archivo_, linea_, columna_};
        char c = actual();

        if (c == '\n') {
            avanzar();
            if (tokens.empty() || tokens.back().tipo != TipoToken::SaltoLinea) {
                tokens.push_back(Token{TipoToken::SaltoLinea, "\\n", std::move(u)});
            }
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(leer_numero());
            continue;
        }
        if (c == '"' || c == '\'') {
            tokens.push_back(leer_texto());
            continue;
        }
        if (es_inicio_identificador(static_cast<unsigned char>(c))) {
            tokens.push_back(leer_identificador());
            continue;
        }

        avanzar();
        switch (c) {
            case '+': tokens.push_back(Token{TipoToken::Mas, "+", std::move(u)}); break;
            case '-': tokens.push_back(Token{TipoToken::Menos, "-", std::move(u)}); break;
            case '*': tokens.push_back(Token{TipoToken::Estrella, "*", std::move(u)}); break;
            case '/': tokens.push_back(Token{TipoToken::Barra, "/", std::move(u)}); break;
            case '%': tokens.push_back(Token{TipoToken::Porcentaje, "%", std::move(u)}); break;
            case '(': tokens.push_back(Token{TipoToken::ParenIzq, "(", std::move(u)}); break;
            case ')': tokens.push_back(Token{TipoToken::ParenDer, ")", std::move(u)}); break;
            case '[': tokens.push_back(Token{TipoToken::CorcheteIzq, "[", std::move(u)}); break;
            case ']': tokens.push_back(Token{TipoToken::CorcheteDer, "]", std::move(u)}); break;
            case '{': tokens.push_back(Token{TipoToken::LlaveIzq, "{", std::move(u)}); break;
            case '}': tokens.push_back(Token{TipoToken::LlaveDer, "}", std::move(u)}); break;
            case ',': tokens.push_back(Token{TipoToken::Coma, ",", std::move(u)}); break;
            case '.': tokens.push_back(Token{TipoToken::Punto, ".", std::move(u)}); break;
            case ':': tokens.push_back(Token{TipoToken::DosPuntos, ":", std::move(u)}); break;
            case ';': tokens.push_back(Token{TipoToken::PuntoYComa, ";", std::move(u)}); break;
            case '=':
                if (actual() == '=') {
                    avanzar();
                    tokens.push_back(Token{TipoToken::IgualIgual, "==", std::move(u)});
                } else {
                    tokens.push_back(Token{TipoToken::Igual, "=", std::move(u)});
                }
                break;
            case '!':
                if (actual() == '=') {
                    avanzar();
                    tokens.push_back(Token{TipoToken::Diferente, "!=", std::move(u)});
                } else {
                    throw ErrorLexico("caracter '!' inesperado; use 'no' o '!='", u);
                }
                break;
            case '<':
                if (actual() == '=') {
                    avanzar();
                    tokens.push_back(Token{TipoToken::MenorIgual, "<=", std::move(u)});
                } else {
                    tokens.push_back(Token{TipoToken::Menor, "<", std::move(u)});
                }
                break;
            case '>':
                if (actual() == '=') {
                    avanzar();
                    tokens.push_back(Token{TipoToken::MayorIgual, ">=", std::move(u)});
                } else {
                    tokens.push_back(Token{TipoToken::Mayor, ">", std::move(u)});
                }
                break;
            default:
                throw ErrorLexico(std::string("caracter no reconocido: '") + c + "'", u);
        }
    }

    tokens.push_back(Token{TipoToken::FinArchivo, "", Ubicacion{archivo_, linea_, columna_}});
    return tokens;
}

}  // namespace esp