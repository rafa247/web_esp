const languageEntries = [
  { group: "Estructura y declaraciones", items: [
    ["variable", "Declara una variable que puede cambiar de valor.", "declaración"],
    ["constante", "Declara un valor que no se puede reasignar.", "declaración"],
    ["datos", "Alias de declaración de variable.", "alias"],
    ["lista", "Alias de declaración de variable, útil para colecciones.", "alias"],
    ["funcion", "Declara una función con parámetros entre paréntesis.", "declaración"],
    ["devolver", "Termina una función y devuelve un valor.", "funciones"],
    ["clase", "Declara una clase cuyos miembros pueden ser campos o métodos.", "declaración"],
    ["hereda", "Indica la clase base: `clase Hija hereda Base`.", "clases"],
    ["de", "Alias para indicar una clase base.", "alias"],
    ["publica / publico / pública / público / public", "Declara un miembro accesible desde fuera del módulo o clase.", "visibilidad"],
    ["privada / privado / private", "Declara un miembro accesible solo desde el ámbito autorizado.", "visibilidad"]
  ]},
  { group: "Control de flujo", items: [
    ["si / if", "Ejecuta un bloque cuando la condición es verdadera.", "condicional"],
    ["sino / si_no / else", "Define el bloque alternativo de una condición.", "condicional"],
    ["elseif / else_if / si_no_si", "Añade una condición alternativa a una cadena `si`.", "condicional"],
    ["si no", "Forma separada que también introduce una rama alternativa.", "condicional"],
    ["mientras / while", "Repite el bloque mientras la condición sea verdadera.", "bucle"],
    ["para", "Repite un bloque sobre un rango o una colección.", "bucle"],
    ["en", "Usado con `para` para recorrer los elementos de una colección.", "bucle"],
    ["hasta / a", "Define el límite final de un bucle numérico `para`.", "bucle"],
    ["paso", "Define el incremento o decremento del bucle numérico.", "bucle"],
    ["romper", "Sale inmediatamente del bucle actual.", "bucle"],
    ["continuar", "Salta al siguiente ciclo del bucle actual.", "bucle"],
    ["fin / end", "Cierra una función, clase, bucle o bloque condicional.", "bloque"],
    ["intenta", "Ejecuta un bloque y permite manejar errores en la rama alternativa.", "errores"],
    ["si_no_puede", "Introduce el bloque alternativo de `intenta`.", "errores"]
  ]},
  { group: "Módulos y expresiones", items: [
    ["conseguir", "Carga un módulo estándar, local o instalado; consulta el registro solo con permiso.", "módulos"],
    ["exportar", "Escribe un archivo, compila un programa o propone un módulo con `como modulo`.", "módulos"],
    ["__herramientas__", "Módulo estándar para analizar y compilar código ESP.", "módulos"],
    ["como", "Indica el formato de exportación, por ejemplo `como modulo`.", "módulos"],
    ["y", "Operador lógico AND.", "operador"],
    ["o", "Operador lógico OR.", "operador"],
    ["no", "Operador lógico NOT.", "operador"],
    ["verdadero / falso", "Valores booleanos incorporados.", "valor"],
    ["nulo", "Valor que representa la ausencia de dato.", "valor"],
    ["este / this", "Referencia a la instancia actual dentro de un método.", "clases"],
    ["VERSION", "Constante global con la versión del runtime ESP.", "constante"],
    ["longitud / agregar", "Propiedades y método de listas: longitud devuelve el tamaño y agregar(valor) añade un elemento.", "colecciones"]
  ]},
  { group: "Operadores y colecciones", items: [
    ["=", "Asigna un valor a una variable, campo o posición indexada.", "asignación"],
    ["+  -  *  /  %  **", "Suma, resta, multiplicación, división, módulo y potencia.", "aritmética"],
    ["==  !=  >  <  >=  <=", "Igualdad, diferencia y comparaciones numéricas.", "comparación"],
    ["[]  {}", "Sintaxis literal para listas y mapas, respectivamente.", "colecciones"],
    ["[índice]  .miembro", "Acceso por índice a colecciones o por nombre a campos y módulos.", "acceso"]
  ]},
  { group: "Funciones globales", items: [
    ["mostrar / imprimir / mostrar_en_terminal", "Imprime valores en la terminal, separados por espacios.", "función"],
    ["tipo", "Devuelve el nombre del tipo de un valor.", "función"],
    ["texto", "Convierte un valor a texto.", "función"],
    ["entero", "Convierte un valor numérico a entero.", "función"],
    ["longitud", "Devuelve la longitud de un texto, lista o mapa.", "función"],
    ["elementos", "Imprime los elementos de una lista.", "función"],
    ["agregar_a_lista", "Añade un elemento a una lista.", "función"],
    ["existe_en_lista", "Comprueba si una lista contiene un valor.", "función"],
    ["afirmar", "Lanza un error si la condición es falsa.", "función"],
    ["hash_sha256", "Calcula el resumen SHA-256 de un texto.", "función"]
  ]}
];

const standardModules = [
  { name: "archivos", aliases: [], description: "Lectura, escritura y comprobación de archivos.",
    members: [["leer(ruta)", "Devuelve el contenido del archivo como texto."],
      ["escribir(ruta, texto)", "Escribe el texto en la ruta y devuelve verdadero si termina."],
      ["existe(ruta)", "Indica si el archivo se puede abrir."]] },
  { name: "sistema", aliases: ["sistema.os"], description: "Información del sistema operativo y sus variables.",
    members: [["nombre()", "Devuelve windows, linux o macos según la plataforma."],
      ["variable(nombre)", "Devuelve el valor de una variable de entorno o nulo si no existe."]] },
  { name: "sistema.procesos", aliases: ["procesos"], description: "Ejecuta procesos y devuelve código, salida y error.",
    members: [["ejecutar(comando, tiempo_límite?)", "Devuelve un mapa con codigo, salida y error. El tiempo límite opcional está expresado en milisegundos."]] },
  { name: "matematicas", aliases: [], description: "Operaciones matemáticas habituales y constante PI.",
    members: [["raiz(x)", "Calcula la raíz cuadrada de x."], ["abs(x)", "Devuelve el valor absoluto."],
      ["min(a, b)", "Devuelve el menor de los dos números."], ["max(a, b)", "Devuelve el mayor de los dos números."],
      ["PI", "Constante aproximada de pi."]] },
  { name: "estadistica", aliases: [], description: "Estadística básica para colecciones numéricas.",
    members: [["media(lista)", "Calcula la media aritmética de los números de una lista no vacía."]] },
  { name: "datos.sql", aliases: ["sql"], description: "Base de datos SQL integrada y comprobación de clientes externos.",
    members: [["abrir(ruta?)", "Abre o crea una base embebida; sin ruta, usa :memoria:. Devuelve un mapa con ruta y motor."],
      ["consultar(ruta, sql, parametros?)", "Ejecuta SQL en una base abierta. Los parámetros opcionales son una lista para los marcadores ?."],
      ["conectar_externo(motor)", "Comprueba si el cliente nativo del motor está disponible; no crea una sesión SQL remota."]] },
  { name: "redes.ip", aliases: ["ip"], description: "Consulta el nombre del equipo local.",
    members: [["local()", "Devuelve el hostname; en plataformas no Windows usa localhost."]] },
  { name: "redes.http", aliases: ["http", "web.http"], description: "Realiza solicitudes HTTP GET.",
    members: [["obtener(url)", "Devuelve el cuerpo de la respuesta como texto. La implementación actual está disponible en Windows."]] },
  { name: "servidor", aliases: [], description: "Inicia un servidor HTTP sencillo para atender una solicitud.",
    members: [["http(puerto?, cuerpo?)", "Escucha en el puerto (8080 por defecto), responde a un cliente y devuelve verdadero. La implementación actual está disponible en Windows."]] },
  { name: "redes.sockets", aliases: ["sockets"], description: "Envío de datagramas UDP.",
    members: [["udp_enviar(host, puerto, texto)", "Envía un datagrama y devuelve la cantidad de bytes enviados. La implementación actual está disponible en Windows."]] },
  { name: "seguridad", aliases: [], description: "Funciones criptográficas disponibles en el runtime.",
    members: [["sha256(texto)", "Devuelve el resumen SHA-256 en hexadecimal."]] },
  { name: "json", aliases: [], description: "Analiza y serializa valores JSON.",
    members: [["parsear(texto)", "Convierte JSON a valores ESP."], ["analizar(texto)", "Alias de parsear."],
      ["deserializar(texto)", "Alias de parsear."], ["texto(valor)", "Serializa un valor ESP a JSON."],
      ["serializar(valor)", "Alias de texto."], ["codificar(valor)", "Alias de texto."]] },
  { name: "pruebas", aliases: [], description: "Aserciones para programas y suites de pruebas.",
    members: [["afirmar(condición, mensaje?)", "Devuelve verdadero si se cumple; de lo contrario, lanza un error con el mensaje opcional."]] },
  { name: "compilacion", aliases: [], description: "Información del compilador C++ detectado.",
    members: [["compilador()", "Devuelve la ruta del compilador C++ disponible."]] },
  { name: "__herramientas__", aliases: [], description: "Herramientas reales del parser y del compilador ESP.",
    members: [["analizar(codigo, archivo?)", "Comprueba la sintaxis y devuelve valido, sentencias y el diagnóstico con línea y columna."],
      ["diagnosticar(codigo, archivo?)", "Alias de analizar."],
      ["compilar(codigo, salida, formato?, optimizacion?)", "Compila código ESP en un ejecutable, C++, LLVM IR o biblioteca nativa; falla con un error explícito si el compilador no puede generar el artefacto."]] }
];

const keywordGroups = document.querySelector("#keyword-groups");
const standardModuleGrid = document.querySelector("#standard-modules");
const communityModuleGrid = document.querySelector("#community-modules");
const registryStatus = document.querySelector("#registry-status");
const communityEmpty = document.querySelector("#community-empty");

function makeKeywordCard([name, description, tag]) {
  const article = document.createElement("article");
  article.className = "keyword-card";
  const title = document.createElement("h4");
  const code = document.createElement("code");
  code.textContent = name;
  const badge = document.createElement("span");
  badge.className = "keyword-tag";
  badge.textContent = tag;
  title.append(code, badge);
  const text = document.createElement("p");
  text.textContent = description;
  article.append(title, text);
  return article;
}

function renderKeywords(query = "") {
  const term = query.trim().toLocaleLowerCase("es");
  keywordGroups.replaceChildren();
  for (const group of languageEntries) {
    const items = group.items.filter(item => `${item[0]} ${item[1]} ${item[2]}`.toLocaleLowerCase("es").includes(term));
    if (!items.length) continue;
    const section = document.createElement("section");
    section.className = "keyword-group";
    const heading = document.createElement("h3");
    heading.textContent = group.group;
    const grid = document.createElement("div");
    grid.className = "keyword-grid";
    for (const item of items) grid.append(makeKeywordCard(item));
    section.append(heading, grid);
    keywordGroups.append(section);
  }
  if (!keywordGroups.childElementCount) keywordGroups.append(makeEmpty("No se encontraron palabras para esa búsqueda."));
}

function makeEmpty(message) {
  const empty = document.createElement("p");
  empty.className = "empty-state";
  empty.textContent = message;
  return empty;
}

function makeModuleCard(module, community = false) {
  const article = document.createElement("article");
  article.className = "module-card";
  const top = document.createElement("div");
  top.className = "module-card-top";
  const title = document.createElement("h3");
  title.textContent = module.name;
  const badge = document.createElement("span");
  badge.className = "module-badge";
  badge.textContent = community ? "comunidad" : "estándar";
  top.append(title, badge);
  const description = document.createElement("p");
  description.textContent = module.description || (community ? "Módulo ESP publicado en el registro comunitario." : "");
  const members = document.createElement("div");
  members.className = "module-members";
  if (Array.isArray(module.aliases) && module.aliases.length) {
    const aliases = document.createElement("p");
    aliases.textContent = `Alias: ${module.aliases.join(", ")}`;
    members.append(aliases);
  }
  if (Array.isArray(module.members)) {
    for (const [signature, details] of module.members) {
      const item = document.createElement("p");
      const code = document.createElement("code");
      code.textContent = signature;
      item.append(code, document.createTextNode(` — ${details}`));
      members.append(item);
    }
  } else {
    members.textContent = module.aliases?.length
      ? `Alias: ${module.aliases.join(", ")} · Fuente: ${module.file || `${module.name}.esp`}`
      : `Fuente: ${module.file || `${module.name}.esp`}`;
  }
  article.append(top, description, members);
  if (community) {
    const download = document.createElement("a");
    download.className = "module-download";
    download.href = `modules/${encodeURIComponent(module.name)}.esp`;
    download.download = `${module.name}.esp`;
    download.textContent = "Descargar fuente .esp";
    article.append(download);
  }
  return article;
}

function renderStandardModules(query = "") {
  const term = query.trim().toLocaleLowerCase("es");
  standardModuleGrid.replaceChildren();
  const matches = standardModules.filter(module =>
    `${module.name} ${module.description} ${module.members}`.toLocaleLowerCase("es").includes(term));
  for (const module of matches) standardModuleGrid.append(makeModuleCard(module));
  if (!matches.length) standardModuleGrid.append(makeEmpty("No se encontraron módulos estándar."));
}

function renderCommunityModules(modules, query = "") {
  const term = query.trim().toLocaleLowerCase("es");
  communityModuleGrid.replaceChildren();
  const matches = modules.filter(module =>
    `${module.name} ${module.description || ""} ${module.file || ""}`.toLocaleLowerCase("es").includes(term));
  for (const module of matches) communityModuleGrid.append(makeModuleCard(module, true));
  communityEmpty.hidden = matches.length > 0;
}

let communityModules = [];
renderKeywords();
renderStandardModules();

document.querySelector("#language-search").addEventListener("input", event => renderKeywords(event.target.value));
document.querySelector("#module-search").addEventListener("input", event => {
  renderStandardModules(event.target.value);
  renderCommunityModules(communityModules, event.target.value);
});

document.querySelector(".menu-toggle").addEventListener("click", event => {
  const button = event.currentTarget;
  const nav = document.querySelector("#site-nav");
  const open = button.getAttribute("aria-expanded") !== "true";
  button.setAttribute("aria-expanded", String(open));
  nav.classList.toggle("is-open", open);
});

document.querySelectorAll("#site-nav a").forEach(link => {
  link.addEventListener("click", () => {
    document.querySelector(".menu-toggle").setAttribute("aria-expanded", "false");
    document.querySelector("#site-nav").classList.remove("is-open");
  });
});

document.addEventListener("keydown", event => {
  if ((event.metaKey || event.ctrlKey) && event.key.toLowerCase() === "k") {
    event.preventDefault();
    document.querySelector("#language-search").focus();
  }
});

async function loadRegistry() {
  try {
    const response = await fetch(new URL("registry/index.json", document.baseURI), { cache: "no-store" });
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    const registry = await response.json();
    if (!registry || !Array.isArray(registry.modules)) throw new Error("el índice no tiene una lista de módulos válida");
    communityModules = registry.modules.filter(module =>
      module && typeof module.name === "string" && /^[a-zA-Z0-9_-]+$/.test(module.name));
    renderCommunityModules(communityModules, document.querySelector("#module-search").value);
    registryStatus.textContent = `${communityModules.length} módulo(s) disponibles`;
    communityEmpty.hidden = communityModules.length > 0;
  } catch (error) {
    registryStatus.textContent = "Registro no disponible";
    communityModuleGrid.replaceChildren(makeEmpty("No se pudo cargar el registro. Comprueba la conexión o inténtalo más tarde."));
    communityEmpty.hidden = true;
    console.error("No se pudo cargar el registro de módulos:", error);
  }
}

loadRegistry();
