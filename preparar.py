from pathlib import Path
from PIL import Image

for i, n in enumerate((256, 512, 1024), 1):
    pasta = Path("entradas")
    png = pasta / f"imagem{i}_{n}.png"
    with Image.open(png) as imagem:
        if imagem.mode != "L" or imagem.size != (n, n):
            raise ValueError(f"Formato inesperado em {png}")
        raw = pasta / f"imagem_{n}.raw"
        raw.write_bytes(imagem.tobytes())
        print(f"{png.name} -> {raw.name} ({raw.stat().st_size} pixels)")
