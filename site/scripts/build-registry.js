const crypto = require("node:crypto");
const fs = require("node:fs");
const path = require("node:path");

const siteRoot = path.resolve(__dirname, "..");
const modulesRoot = path.join(siteRoot, "modules");
const registryPath = path.join(siteRoot, "registry", "index.json");

fs.mkdirSync(path.dirname(registryPath), { recursive: true });
fs.mkdirSync(modulesRoot, { recursive: true });

const modules = fs.readdirSync(modulesRoot, { withFileTypes: true })
  .filter(entry => entry.isFile() && entry.name.endsWith(".esp"))
  .map(entry => {
    const name = path.basename(entry.name, ".esp");
    if (!/^[a-zA-Z0-9_-]+$/.test(name)) {
      throw new Error(`Nombre de módulo no válido: ${entry.name}`);
    }
    const content = fs.readFileSync(path.join(modulesRoot, entry.name));
    const record = {
      name,
      file: `modules/${entry.name}`,
      sha256: crypto.createHash("sha256").update(content).digest("hex")
    };
    const metadataPath = path.join(modulesRoot, `${entry.name}.json`);
    if (fs.existsSync(metadataPath)) {
      const metadata = JSON.parse(fs.readFileSync(metadataPath, "utf8"));
      if (metadata.version !== undefined) {
        if (typeof metadata.version !== "string" || !/^\d+\.\d+\.\d+$/.test(metadata.version)) {
          throw new Error(`Versión no válida en ${path.basename(metadataPath)}; usa X.Y.Z.`);
        }
        record.version = metadata.version;
      }
      if (metadata.dependencies !== undefined) {
        if (!Array.isArray(metadata.dependencies)) {
          throw new Error(`dependencies debe ser una lista en ${path.basename(metadataPath)}.`);
        }
        const names = new Set();
        record.dependencies = metadata.dependencies.map(dependency => {
          const item = typeof dependency === "string" ? { name: dependency } : dependency;
          if (!item || typeof item.name !== "string" || !/^[a-zA-Z0-9_-]+$/.test(item.name) ||
              names.has(item.name)) {
            throw new Error(`Dependencia no válida en ${path.basename(metadataPath)}.`);
          }
          names.add(item.name);
          if (item.version !== undefined &&
              (typeof item.version !== "string" || !/^\d+\.\d+\.\d+$/.test(item.version))) {
            throw new Error(`Versión no válida para ${item.name} en ${path.basename(metadataPath)}.`);
          }
          return item.version === undefined ? { name: item.name } :
            { name: item.name, version: item.version };
        });
      }
    }
    return record;
  })
  .sort((left, right) => left.name.localeCompare(right.name, "es"));

fs.writeFileSync(registryPath, `${JSON.stringify({ schemaVersion: 1, modules }, null, 2)}\n`, "utf8");
console.log(`Registro generado: ${modules.length} módulo(s).`);
