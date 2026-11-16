#include "esp/regex_mod.hpp"
#include "esp/runtime.hpp"
#include <map>
#include <mutex>
#include <regex>
#include <string>
#include <vector>

namespace esp {
namespace {

void registrar_fn(Entorno& e, const std::string& n, std::function<Valor(std::vector<Valor>&, Ubicacion)> f) {
    auto fn = std::make_shared<Funcion>();
    fn->nombre = n;
    fn->es_nativa = true;
    fn->nativa = std::move(f);
    Valor v;
    v.tipo = TipoValor::Funcion;
    v.funcion = fn;
    e.definir(n, std::move(v), Visibilidad::Publico, true, true);
}

const std::regex& obtener_regex_cacheado(const std::string& patron, const std::string& flags_str, Ubicacion u) {
    static std::mutex rx_mutex;
    static std::map<std::string, std::regex> cache_rx;
    std::string clave = flags_str + "\x01" + patron;
    std::lock_guard<std::mutex> lk(rx_mutex);
    auto it = cache_rx.find(clave);
    if (it != cache_rx.end()) return it->second;
    try {
        std::regex::flag_type flags = std::regex::ECMAScript;
        for (char c : flags_str) if (c == 'i' || c == 'I') flags |= std::regex::icase;
        return cache_rx.emplace(clave, std::regex(patron, flags)).first->second;
    } catch (const std::regex_error& e) {
        throw ErrorEjecucion(std::string("patron regex no valido: ") + e.what(), u);
    }
}

}  // namespace

std::shared_ptr<Modulo> crear_modulo_re(const std::string& nombre_alias) {
    auto m = std::make_shared<Modulo>();
    m->nombre = nombre_alias;
    m->version = VERSION;
    m->entorno = std::make_shared<Entorno>();

    registrar_fn(*m->entorno, "coincide", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("re.coincide(patron, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 2 ? a[2].a_texto() : "", u);
        return Valor::de_bool(std::regex_match(a[1].a_texto(), rx));
    });

    registrar_fn(*m->entorno, "coincide_inicio", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("re.coincide_inicio(patron, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 2 ? a[2].a_texto() : "", u);
        std::string texto = a[1].a_texto();
        std::smatch match;
        if (std::regex_search(texto, match, rx, std::regex_constants::match_continuous)) {
            std::vector<Valor> grupos;
            for (std::size_t i = 0; i < match.size(); ++i) grupos.push_back(Valor::de_texto(match[i].str()));
            return Valor::de_mapa({
                {"encontrado", Valor::de_bool(true)},
                {"texto", Valor::de_texto(match[0].str())},
                {"inicio", Valor::de_entero(0)},
                {"fin", Valor::de_entero(static_cast<std::int64_t>(match.length(0)))},
                {"grupos", Valor::de_lista(std::move(grupos))}
            });
        }
        return Valor::de_mapa({{"encontrado", Valor::de_bool(false)}, {"texto", Valor::de_texto("")}, {"inicio", Valor::de_entero(-1)}, {"fin", Valor::de_entero(-1)}, {"grupos", Valor::de_lista({})}});
    });

    registrar_fn(*m->entorno, "buscar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("re.buscar(patron, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 2 ? a[2].a_texto() : "", u);
        std::string texto = a[1].a_texto();
        std::smatch match;
        if (std::regex_search(texto, match, rx)) {
            std::vector<Valor> grupos;
            for (std::size_t i = 0; i < match.size(); ++i) grupos.push_back(Valor::de_texto(match[i].str()));
            std::int64_t ini = static_cast<std::int64_t>(match.position(0));
            std::int64_t len = static_cast<std::int64_t>(match.length(0));
            return Valor::de_mapa({
                {"encontrado", Valor::de_bool(true)},
                {"texto", Valor::de_texto(match[0].str())},
                {"inicio", Valor::de_entero(ini)},
                {"fin", Valor::de_entero(ini + len)},
                {"grupos", Valor::de_lista(std::move(grupos))}
            });
        }
        return Valor::de_mapa({{"encontrado", Valor::de_bool(false)}, {"texto", Valor::de_texto("")}, {"inicio", Valor::de_entero(-1)}, {"fin", Valor::de_entero(-1)}, {"grupos", Valor::de_lista({})}});
    });

    registrar_fn(*m->entorno, "encontrar_todos", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("re.encontrar_todos(patron, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 2 ? a[2].a_texto() : "", u);
        std::string texto = a[1].a_texto();
        std::vector<Valor> resultados;
        for (std::sregex_iterator it(texto.begin(), texto.end(), rx), end; it != end; ++it) {
            resultados.push_back(Valor::de_texto((*it)[0].str()));
        }
        return Valor::de_lista(std::move(resultados));
    });

    registrar_fn(*m->entorno, "reemplazar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 3) throw ErrorEjecucion("re.reemplazar(patron, reemplazo, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 3 ? a[3].a_texto() : "", u);
        return Valor::de_texto(std::regex_replace(a[2].a_texto(), rx, a[1].a_texto()));
    });

    registrar_fn(*m->entorno, "dividir", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.size() < 2) throw ErrorEjecucion("re.dividir(patron, texto, banderas?)", u);
        const auto& rx = obtener_regex_cacheado(a[0].a_texto(), a.size() > 2 ? a[2].a_texto() : "", u);
        std::string texto = a[1].a_texto();
        std::vector<Valor> partes;
        for (std::sregex_token_iterator it(texto.begin(), texto.end(), rx, -1), end; it != end; ++it) {
            partes.push_back(Valor::de_texto(it->str()));
        }
        return Valor::de_lista(std::move(partes));
    });

    registrar_fn(*m->entorno, "escapar", [](std::vector<Valor>& a, Ubicacion u) {
        if (a.empty()) throw ErrorEjecucion("re.escapar(texto)", u);
        std::string src = a[0].a_texto();
        const std::string especiales = "\\^$.|?*+()[]{}";
        std::string out;
        out.reserve(src.size() * 2);
        for (char c : src) {
            if (especiales.find(c) != std::string::npos) out.push_back('\\');
            out.push_back(c);
        }
        return Valor::de_texto(std::move(out));
    });

    return m;
}

}  // namespace esp
