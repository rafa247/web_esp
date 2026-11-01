# Sitio de ESP

Sitio estático escrito en HTML, CSS y JavaScript sin dependencias de compilación. Incluye la referencia del lenguaje, la biblioteca estándar y el catálogo comunitario.

## Dependencias

El sitio no usa paquetes npm, frameworks, fuentes externas ni CDN. La web se ejecuta en un navegador moderno con soporte para HTML, CSS, JavaScript y `fetch`.

| Componente | Versión | Licencia | Uso |
|---|---|---|---|
| Node.js | 18 o posterior; probado con 24.18.0 | MIT ([licencia](https://github.com/nodejs/node/blob/main/LICENSE)) | Generar el índice y los SHA-256 de los módulos con APIs integradas; no instala paquetes. |
| `curl` | Debe admitir `--proto`, `--proto-redir` y `--max-filesize`; probado con 8.13.0 | MIT ([licencia](https://curl.se/docs/copyright.html)) | Descargar el índice y los módulos por HTTPS desde el intérprete. |
| Git | 2.x; probado con 2.55.0 | GPL-2.0 ([licencia](https://github.com/git/git/blob/master/COPYING)) | Preparar la rama que propone un módulo. |
| GitHub CLI (`gh`) | Versión estable actual; no se fija una versión | MIT ([licencia](https://github.com/cli/cli/blob/trunk/LICENSE)) | Crear el Pull Request de un módulo; solo se necesita para publicar. |

El workflow de Pages usa `actions/checkout@v4`, `actions/configure-pages@v5`, `actions/upload-pages-artifact@v3` y `actions/deploy-pages@v4`, todos con licencia MIT. Esas etiquetas fijan la versión mayor de cada acción.

## Registro

Los módulos publicados se guardan como archivos `.esp` en `modules/`. Antes de desplegar, genera el índice:

```powershell
node scripts\build-registry.js
```

El índice contiene el nombre, la ruta y el SHA-256 de cada módulo. El workflow de GitHub Pages lo regenera antes de publicar. Para declarar versión y dependencias, añade junto a `nombre.esp` el archivo `nombre.esp.json`:

```json
{
  "version": "1.2.3",
  "dependencies": [
    { "name": "base", "version": "2.0.0" },
    "utilidades"
  ]
}
```

Las versiones admitidas son exactas (`X.Y.Z`); el intérprete instala primero las dependencias, y rechaza ciclos. Los campos de versión y dependencias son opcionales para conservar compatibilidad con los módulos existentes. Un `conseguir nombre "1.2.3"` solicita una versión exacta.

## Compilación desde ESP

`exportar destino como exe fuente` compila el texto ESP a un ejecutable del sistema actual. Los formatos disponibles incluyen `exe`, `cpp`, `ll`, `estatica` y la biblioteca dinámica nativa (`dll` en Windows, `so` en Linux o `dylib` en macOS). Las bibliotecas exportan `esp_run(void)` mediante C ABI y generan una cabecera `.h` adyacente; devuelve `0` si el programa ESP termina correctamente y `1` si falla. No transfiere memoria a través de la ABI.

`conseguir __herramientas__` carga `analizar(codigo, archivo?)`, `diagnosticar(codigo, archivo?)` y `compilar(codigo, salida, formato?, optimizacion?)`. El analizador devuelve un mapa con `valido`, `sentencias`, `diagnostico`, `linea` y `columna`; el compilador devuelve `archivo`, `formato`, `mensaje`, `cache` y `llvm`, y lanza un error si no logra generar el artefacto. El formato `cpp` solo genera la fuente; los formatos ejecutables y las bibliotecas requieren un compilador C++ nativo, y `ll` requiere Clang.

## GitHub Pages

El sitio está en `site/` dentro del proyecto ESP. Publica el proyecto conservando tanto esa carpeta como `.github/workflows/pages.yml` en la raíz; habilita Pages con la fuente **GitHub Actions** en la configuración del repositorio. El workflow publica al hacer push a `main`.

La integración del intérprete usa `ESP_MODULE_REGISTRY_REPOSITORY=rafa247/web_esp`; también puedes indicar la URL HTTPS del sitio con `ESP_MODULE_REGISTRY_URL`.

En PowerShell configura el repositorio para la sesión actual así:

```powershell
$env:ESP_MODULE_REGISTRY_REPOSITORY = "rafa247/web_esp"
```

Para buscar e instalar módulos, `curl` debe estar disponible en `PATH`. La publicación crea un Pull Request y requiere Git, GitHub CLI (`gh`) autenticado y permiso para subir ramas. Inicia sesión con `gh auth login`, configura la autenticación de Git con `gh auth setup-git` y verifica la sesión con `gh auth status`; no guardes tokens en los fuentes.

## Contribuir módulos

Un Pull Request puede añadir `modules/nombre.esp` y, de forma opcional, `modules/nombre.esp.json` con su versión exacta y dependencias. Tras revisar y fusionar el cambio, el workflow actualiza el índice y publica el módulo junto con el sitio.
