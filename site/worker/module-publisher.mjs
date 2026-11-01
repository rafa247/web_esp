const MAX_SOURCE_BYTES = 1024 * 1024;
const API_ROOT = "https://api.github.com";
const TURNSTILE_URL = "https://challenges.cloudflare.com/turnstile/v0/siteverify";

class SubmissionError extends Error {
  constructor(status, message) {
    super(message);
    this.status = status;
  }
}

function jsonResponse(body, status, origin) {
  const headers = { "Content-Type": "application/json; charset=utf-8", "Cache-Control": "no-store" };
  if (origin) {
    headers["Access-Control-Allow-Origin"] = origin;
    headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
    headers["Access-Control-Allow-Headers"] = "Content-Type";
    headers.Vary = "Origin";
  }
  return new Response(status === 204 ? null : JSON.stringify(body), { status, headers });
}

function configuredOrigin(env) {
  try {
    const url = new URL(env.ALLOWED_ORIGIN);
    if (url.protocol !== "https:" || url.origin !== env.ALLOWED_ORIGIN) return "";
    return url.origin;
  } catch {
    return "";
  }
}

async function githubRequest(env, path, options = {}) {
  const suffix = path ? `/${path}` : "";
  const response = await fetch(`${API_ROOT}/repos/${env.REGISTRY_OWNER}/${env.REGISTRY_REPO}${suffix}`, {
    ...options,
    headers: {
      Accept: "application/vnd.github+json",
      Authorization: `Bearer ${env.GITHUB_TOKEN}`,
      "User-Agent": "ESP-Module-Publisher",
      "X-GitHub-Api-Version": "2022-11-28",
      ...(options.body ? { "Content-Type": "application/json" } : {}),
      ...options.headers
    }
  });
  if (!response.ok) {
    const detail = await response.text();
    throw new SubmissionError(
      response.status === 404 ? 404 : response.status === 409 || response.status === 422 ? 409 : 502,
      response.status === 404 ? "No se encontró el archivo o la rama del registro." :
        response.status === 409 || response.status === 422
          ? "El registro cambió o rechazó el envío. Inténtalo otra vez."
          : `GitHub rechazó la publicación (HTTP ${response.status}): ${detail.slice(0, 300)}`
    );
  }
  return response.status === 204 ? null : response.json();
}

async function comprobarTurnstile(token, request, env, origin) {
  if (!env.TURNSTILE_SECRET) throw new SubmissionError(503, "La verificación antispam aún no está configurada.");
  const datos = new URLSearchParams({ secret: env.TURNSTILE_SECRET, response: token });
  const ip = request.headers.get("CF-Connecting-IP");
  if (ip) datos.set("remoteip", ip);
  const response = await fetch(TURNSTILE_URL, { method: "POST", body: datos });
  if (!response.ok) throw new SubmissionError(502, "No se pudo verificar el control antispam.");
  const resultado = await response.json();
  if (!resultado.success || resultado.hostname !== new URL(origin).hostname)
    throw new SubmissionError(400, "Completa de nuevo la verificación antispam.");
}

async function leerCuerpoLimitado(request, limite) {
  const lector = request.body?.getReader();
  if (!lector) return "";
  const fragmentos = [];
  let total = 0;
  try {
    while (true) {
      const { done, value } = await lector.read();
      if (done) break;
      total += value.byteLength;
      if (total > limite) {
        await lector.cancel();
        throw new SubmissionError(413, "El envío supera el tamaño máximo permitido.");
      }
      fragmentos.push(value);
    }
  } finally {
    lector.releaseLock();
  }
  const contenido = new Uint8Array(total);
  let offset = 0;
  for (const fragmento of fragmentos) {
    contenido.set(fragmento, offset);
    offset += fragmento.byteLength;
  }
  try {
    return new TextDecoder("utf-8", { fatal: true }).decode(contenido);
  } catch {
    throw new SubmissionError(400, "El envío debe usar texto UTF-8 válido.");
  }
}

async function enviarModulo(request, env, origin) {
  if (!env.GITHUB_TOKEN || !env.REGISTRY_OWNER || !env.REGISTRY_REPO)
    throw new SubmissionError(503, "El publicador todavía no está configurado.");
  if (!/^[-A-Za-z0-9_.]+$/.test(env.REGISTRY_OWNER) || !/^[-A-Za-z0-9_.]+$/.test(env.REGISTRY_REPO) ||
      env.REGISTRY_OWNER === "." || env.REGISTRY_OWNER === ".." ||
      env.REGISTRY_REPO === "." || env.REGISTRY_REPO === "..")
    throw new SubmissionError(503, "La configuración del repositorio no es válida.");

  const contentLength = Number(request.headers.get("Content-Length") || 0);
  if (contentLength > MAX_SOURCE_BYTES + 16 * 1024)
    throw new SubmissionError(413, "El envío supera el tamaño máximo permitido.");
  const bodyText = await leerCuerpoLimitado(request, MAX_SOURCE_BYTES + 16 * 1024);

  let payload;
  try {
    payload = JSON.parse(bodyText);
  } catch {
    throw new SubmissionError(400, "El envío debe ser JSON válido.");
  }
  if (!payload || typeof payload !== "object" || Array.isArray(payload))
    throw new SubmissionError(400, "El envío no tiene el formato esperado.");

  const { name, description, source, turnstileToken } = payload;
  if (typeof name !== "string" || !/^[a-zA-Z0-9_-]{1,64}$/.test(name))
    throw new SubmissionError(400, "El nombre admite letras, números, guion y guion bajo (máximo 64).");
  if (typeof description !== "string" || description.trim().length < 8 || description.length > 280)
    throw new SubmissionError(400, "La descripción debe tener entre 8 y 280 caracteres.");
  if (typeof source !== "string" || !source.trim())
    throw new SubmissionError(400, "El código ESP no puede estar vacío.");
  if (new TextEncoder().encode(source).byteLength > MAX_SOURCE_BYTES)
    throw new SubmissionError(413, "El código ESP supera el límite de 1 MiB.");
  if (typeof turnstileToken !== "string" || !turnstileToken || turnstileToken.length > 2048)
    throw new SubmissionError(400, "Completa la verificación antispam.");

  await comprobarTurnstile(turnstileToken, request, env, origin);

  const branch = (await githubRequest(env, "", { method: "GET" })).default_branch;
  if (typeof branch !== "string" || !branch)
    throw new SubmissionError(502, "No se pudo determinar la rama del registro.");

  const existingSource = await fetch(
    `${API_ROOT}/repos/${env.REGISTRY_OWNER}/${env.REGISTRY_REPO}/contents/site/modules/${encodeURIComponent(name)}.esp?ref=${encodeURIComponent(branch)}`,
    {
      headers: {
        Accept: "application/vnd.github+json",
        Authorization: `Bearer ${env.GITHUB_TOKEN}`,
        "User-Agent": "ESP-Module-Publisher",
        "X-GitHub-Api-Version": "2022-11-28"
      }
    }
  );
  if (existingSource.ok) throw new SubmissionError(409, "Ya existe un módulo con ese nombre.");
  if (existingSource.status !== 404)
    throw new SubmissionError(502, `No se pudo comprobar el nombre del módulo (HTTP ${existingSource.status}).`);

  const metadataPath = `site/modules/${encodeURIComponent(name)}.esp.json`;
  const existingMetadata = await fetch(`${API_ROOT}/repos/${env.REGISTRY_OWNER}/${env.REGISTRY_REPO}/contents/${metadataPath}?ref=${encodeURIComponent(branch)}`, {
    headers: {
      Accept: "application/vnd.github+json",
      Authorization: `Bearer ${env.GITHUB_TOKEN}`,
      "User-Agent": "ESP-Module-Publisher",
      "X-GitHub-Api-Version": "2022-11-28"
    }
  });
  if (existingMetadata.ok) throw new SubmissionError(409, "Ya existe un módulo con ese nombre.");
  if (existingMetadata.status !== 404)
    throw new SubmissionError(502, `No se pudo comprobar el nombre del módulo (HTTP ${existingMetadata.status}).`);

  const head = await githubRequest(env, `branches/${encodeURIComponent(branch)}`, { method: "GET" });
  const baseCommit = head.commit.sha;
  const baseTree = (await githubRequest(env, `git/commits/${baseCommit}`, { method: "GET" })).tree.sha;
  const metadata = `${JSON.stringify({ description: description.trim() }, null, 2)}\n`;
  const blobs = await Promise.all([
    githubRequest(env, "git/blobs", {
      method: "POST",
      body: JSON.stringify({ content: source, encoding: "utf-8" })
    }),
    githubRequest(env, "git/blobs", {
      method: "POST",
      body: JSON.stringify({ content: metadata, encoding: "utf-8" })
    })
  ]);
  const tree = await githubRequest(env, "git/trees", {
    method: "POST",
    body: JSON.stringify({
      base_tree: baseTree,
      tree: [
        { path: `site/modules/${name}.esp`, mode: "100644", type: "blob", sha: blobs[0].sha },
        { path: `site/modules/${name}.esp.json`, mode: "100644", type: "blob", sha: blobs[1].sha }
      ]
    })
  });
  const commit = await githubRequest(env, "git/commits", {
    method: "POST",
    body: JSON.stringify({
      message: `Publicar módulo ESP: ${name}`,
      tree: tree.sha,
      parents: [baseCommit]
    })
  });
  await githubRequest(env, `git/refs/heads/${encodeURIComponent(branch)}`, {
    method: "PATCH",
    body: JSON.stringify({ sha: commit.sha, force: false })
  });

  return { name, url: `https://${env.REGISTRY_OWNER.toLowerCase()}.github.io/${env.REGISTRY_REPO}/modules/${name}.esp` };
}

export default {
  async fetch(request, env) {
    const origin = request.headers.get("Origin");
    const allowedOrigin = configuredOrigin(env);
    if (!allowedOrigin || origin !== allowedOrigin)
      return jsonResponse({ error: "Origen no autorizado." }, 403);
    if (new URL(request.url).pathname !== "/submit")
      return jsonResponse({ error: "Ruta no encontrada." }, 404, allowedOrigin);
    if (request.method === "OPTIONS")
      return jsonResponse({}, 204, allowedOrigin);
    if (request.method !== "POST")
      return jsonResponse({ error: "Método no permitido." }, 405, allowedOrigin);
    if (!request.headers.get("Content-Type")?.toLowerCase().startsWith("application/json"))
      return jsonResponse({ error: "Se requiere Content-Type application/json." }, 415, allowedOrigin);

    try {
      const resultado = await enviarModulo(request, env, allowedOrigin);
      return jsonResponse({
        message: "Módulo publicado. El catálogo estará disponible cuando termine el despliegue de la web.",
        ...resultado
      }, 201, allowedOrigin);
    } catch (error) {
      if (error instanceof SubmissionError)
        return jsonResponse({ error: error.message }, error.status, allowedOrigin);
      console.error("Falló la publicación de un módulo ESP:", error);
      return jsonResponse({ error: "No se pudo completar la publicación. Inténtalo más tarde." }, 502, allowedOrigin);
    }
  }
};
