"""Generate the compressed captive portal assets embedded in the firmware."""

from pathlib import Path
import gzip

try:
    Import("env")
except NameError:
    env = None

project_dir = Path(env.subst("$PROJECT_DIR")) if env is not None else Path.cwd()
source_dir = project_dir / "data" / "webinterface"
output = project_dir / "src" / "Core" / "Generated" / "ProvisioningPortalAssets_Generated.h"

assets = (
    ("light.html", "kLightHtml"),
    ("light.css", "kLightCss"),
    ("light.js", "kLightJs"),
)

lines = [
    "#pragma once",
    "",
    "#include <stddef.h>",
    "#include <stdint.h>",
    "#include <string.h>",
    "",
    "namespace ProvisioningPortalAssets {",
    "struct Asset {",
    "    const char* path;",
    "    const char* contentType;",
    "    const uint8_t* data;",
    "    size_t size;",
    "};",
    "",
]

for filename, symbol in assets:
    source = source_dir / filename
    compressed = gzip.compress(source.read_bytes(), mtime=0)
    lines.append(f"static const uint8_t {symbol}[] = {{")
    for offset in range(0, len(compressed), 16):
        values = ", ".join(f"0x{value:02X}U" for value in compressed[offset : offset + 16])
        lines.append(f"    {values},")
    lines.append("};")

lines.extend(
    [
        "",
        "inline const Asset* find(const char* path)",
        "{",
        "    if (!path) return nullptr;",
        '    if (strcmp(path, "/webinterface/light.html") == 0) {',
        '        static const Asset asset{"/webinterface/light.html", "text/html; charset=utf-8", kLightHtml, sizeof(kLightHtml)};',
        "        return &asset;",
        "    }",
        '    if (strcmp(path, "/webinterface/light.css") == 0) {',
        '        static const Asset asset{"/webinterface/light.css", "text/css; charset=utf-8", kLightCss, sizeof(kLightCss)};',
        "        return &asset;",
        "    }",
        '    if (strcmp(path, "/webinterface/light.js") == 0) {',
        '        static const Asset asset{"/webinterface/light.js", "application/javascript; charset=utf-8", kLightJs, sizeof(kLightJs)};',
        "        return &asset;",
        "    }",
        "    return nullptr;",
        "}",
        "}  // namespace ProvisioningPortalAssets",
        "",
    ]
)

output.parent.mkdir(parents=True, exist_ok=True)
output.write_text("\n".join(lines), encoding="utf-8")
print(f"[generate_portal_assets] embedded compressed captive portal assets in {output}")
