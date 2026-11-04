from pathlib import Path
import gzip
import shutil

Import("env")

project_dir = Path(env.subst("$PROJECT_DIR"))
source_dir = project_dir / "data"
staging_dir = Path(env.subst("$BUILD_DIR")) / "spiffs_data"

if staging_dir.exists():
    shutil.rmtree(staging_dir)
staging_dir.mkdir(parents=True, exist_ok=True)

for asset_name in ("light.html", "light.css", "light.js"):
    source = source_dir / "webinterface" / asset_name
    if not source.is_file():
        continue

    destination = staging_dir / "webinterface" / f"{asset_name}.gz"
    destination.parent.mkdir(parents=True, exist_ok=True)
    with source.open("rb") as source_file:
        with destination.open("wb") as destination_file:
            with gzip.GzipFile(filename="", mode="wb", fileobj=destination_file, mtime=0) as compressed:
                shutil.copyfileobj(source_file, compressed)

env.Replace(PROJECT_DATA_DIR=str(staging_dir), PROJECTDATA_DIR=str(staging_dir))
print(f"[prepare_spiffs_data] staged lightweight portal assets in {staging_dir}")
