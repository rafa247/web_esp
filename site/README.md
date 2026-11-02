# Sitio de ESP

Sitio estático escrito en HTML, CSS y JavaScript sin dependencias de compilación. Incluye la referencia del lenguaje, la biblioteca estándar y el catálogo comunitario.

## Dependencias

El sitio no usa paquetes npm ni frameworks. La web usa APIs nativas del navegador y carga Cloudflare Turnstile solo cuando el formulario de publicación está configurado.

| Componente | Versión | Licencia | Uso |
|---|---|---|---|
| Node.js | 18 o posterior; probado con 24.18.0 | MIT ([licencia](https://github.com/nodejs/node/blob/main/LICENSE)) | Generar el índice y los SHA-256 de los módulos con APIs integradas; no instala paquetes. |
| `curl` | Debe admitir `--proto`, `--proto-redir` y `--max-filesize`; probado con 8.13.0 | MIT ([licencia](https://curl.se/docs/copyright.html)) | Descargar el índice y los módulos por HTTPS desde el intérprete. |
| Cloudflare Workers y Turnstile | Plan gratuito disponible; consulta los límites vigentes | Propietario | Recibir envíos anónimos del formulario web y bloquear spam automatizado. |
| `ESP_MODULE_PUBLISHER_URL` | Opcional | — | Cambiar el endpoint HTTPS usado por `exportar ... como modulo`; por defecto usa el publicador de este registro. |

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

Las versiones admitidas son exactas (`X.Y.Z`); el intérprete instala primero las dependencias, y rechaza ciclos. Los campos de versión y dependencias son opcionales para conservar compatibilidad con los módulos existentes. Un `conseguir nombre "1.2.3"` solicita una versión exacta. Cada módulo se usa como `nombre_del_modulo.miembro`: por ejemplo, `conseguir mi_modulo` seguido de `mi_modulo.funcion_exportada()`.

El módulo comunitario [`ip.esp`](./modules/ip.esp) implementa utilidades de IPv4 e IPv6 en ESP: validación, conversión y normalización de IPv6, máscaras, subredes CIDR, rangos y clasificación habitual. Sus cálculos son locales: no resuelve DNS ni escanea equipos. Los conversores y cálculos devuelven `nulo` ante entradas no válidas; las validaciones y clasificadores devuelven `falso`.

```esp
conseguir ip
variable v4 = ip.rango("192.168.1.10/24")
variable v6 = ip.rango("2001:db8:abcd:12::1/48")
mostrar(v4.red, v4.broadcast, v4.primera, v4.ultima)
mostrar(ip.canonizar_ipv6("2001:0db8:0000:0000:0000:0000:0000:0001"))
mostrar(v6.red, v6.ultima)
```

## Publicar módulos desde la web

Cualquier persona puede usar el formulario de la página **Publicar un módulo**: escribe el nombre, la descripción y el fuente ESP, completa Turnstile y pulsa **Publicar módulo**. No necesita Git, GitHub CLI, terminal ni cuenta. Los envíos se añaden directamente al registro y se publican automáticamente cuando termina GitHub Pages; no pasan por revisión. Los módulos contienen código ejecutable: los usuarios deben inspeccionar el fuente antes de instalarlo.

El sitio es estático y no puede guardar envíos por sí solo. Para habilitar el formulario, el mantenedor configura una vez un Cloudflare Worker desde el panel web; no hace falta Git ni instalar herramientas:

1. Crea una cuenta de Cloudflare y un widget de Turnstile para el dominio `rafa247.github.io`.
2. En **Workers & Pages**, crea un Worker llamado `esp-module-publisher` y publica el contenido de [`worker/module-publisher.mjs`](./worker/module-publisher.mjs).
3. En **Settings → Variables and Secrets**, configura estas variables:
   - `REGISTRY_OWNER`: `rafa247`
   - `REGISTRY_REPO`: `web_esp`
   - `ALLOWED_ORIGIN`: `https://rafa247.github.io`
   - `GITHUB_TOKEN` como secreto: un fine-grained personal access token limitado al repositorio `web_esp`, con permiso **Contents: Read and write**.
   - `TURNSTILE_SECRET` como secreto: la clave secreta del widget de Turnstile.
4. En [`app.js`](./app.js), asigna `modulePublisherEndpoint` a la URL HTTPS del Worker terminada en `/submit`, y `turnstileSiteKey` a la clave pública del widget. Publica el sitio.

El token de GitHub solo vive como secreto del Worker; nunca lo pongas en JavaScript del sitio. El token requiere permiso de escritura en contenido y la rama principal debe permitir esa escritura directa. El Worker valida el origen, el desafío antispam, el nombre, la descripción y el límite de 1 MiB; evita nombres duplicados y escribe los dos archivos del módulo en una sola actualización de la rama principal. El cambio dispara el workflow de Pages, que regenera el índice. La cuenta y el token del mantenedor son necesarios para configurar el servicio; quienes envían módulos no necesitan cuenta.

El formulario está desactivado hasta configurar la URL del Worker y la clave pública de Turnstile. El módulo comunitario [`ip.esp`](./modules/ip.esp) está incluido en el registro; el módulo nativo antiguo de hostname sigue disponible como `conseguir redes.ip`.

### Publicar desde un programa ESP

`exportar nombre como modulo fuente, manifiesto?` envía el módulo directamente al Worker, sin Git, GitHub CLI, cuenta ni desafío Turnstile. El manifiesto JSON es opcional; puede incluir `description`, `version` y `dependencies`:

```esp
exportar hola como modulo "publica funcion hola()\n    mostrar(\"Hola\")\nfin\n", "{\"description\":\"Imprime Hola en pantalla.\"}"
```

El endpoint predeterminado es el publicador de este registro. Para usar otro, configura `ESP_MODULE_PUBLISHER_URL` con una URL HTTPS terminada en `/submit-esp`. Se requiere `curl` en `PATH`.

La ruta directa de `exportar` es anónima y no usa Turnstile: cualquiera que conozca el endpoint puede enviar módulos únicos y provocar ejecuciones del workflow de Pages. El Worker valida el nombre, tamaño, descripción y metadatos, y no permite sobrescribir módulos existentes, pero eso no evita el spam. Para envíos desde navegador, usa el formulario, que sí exige Turnstile.

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

Para buscar, instalar y publicar módulos desde ESP, `curl` debe estar disponible en `PATH`. Configura `ESP_MODULE_PUBLISHER_URL` solo si necesitas reemplazar el endpoint predeterminado del Worker.

## Contribuir módulos

Además del formulario, los mantenedores pueden añadir `modules/nombre.esp` y, de forma opcional, `modules/nombre.esp.json` mediante un Pull Request. El workflow actualiza el índice y publica el módulo junto con el sitio.
