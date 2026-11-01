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
    return {
      name,
      file: `modules/${entry.name}`,
      sha256: crypto.createHash("sha256").update(content).digest("hex")
    };
  })
  .sort((left, right) => left.name.localeCompare(right.name, "es"));

fs.writeFileSync(registryPath, `${JSON.stringify({ schemaVersion: 1, modules }, null, 2)}\n`, "utf8");
console.log(`Registro generado: ${modules.length} módulo(s).`);
