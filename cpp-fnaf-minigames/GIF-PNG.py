import os
from PIL import Image, ImageSequence

def gif_to_spritesheet_interactive():
    gif_path = input("Podaj ścieżkę do pliku GIF: ").strip().strip('"')

    if not os.path.isfile(gif_path):
        print("Błąd: Plik nie istnieje.")
        return

    if not gif_path.lower().endswith(".gif"):
        print("Błąd: Podany plik nie jest plikiem .gif.")
        return

    try:
        gif = Image.open(gif_path)
        frames = [frame.copy().convert("RGBA") for frame in ImageSequence.Iterator(gif)]

        width, height = frames[0].size
        sheet_width = width * len(frames)
        sheet_height = height

        spritesheet = Image.new("RGBA", (sheet_width, sheet_height))

        for i, frame in enumerate(frames):
            spritesheet.paste(frame, (i * width, 0))

        gif_dir = os.path.dirname(gif_path)
        gif_name = os.path.splitext(os.path.basename(gif_path))[0]
        output_path = os.path.join(gif_dir, f"{gif_name}_spritesheet.png")

        spritesheet.save(output_path)
        print(f"Spritesheet zapisany jako: {output_path}")
        print(f"Klatki: {len(frames)}, Rozmiar pojedynczej klatki: {width}x{height}")

    except Exception as e:
        print(f"Wystąpił błąd: {e}")

# --- Run ---
gif_to_spritesheet_interactive()
