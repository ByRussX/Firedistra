import tkinter as tk
from tkinter import filedialog
from PIL import Image, ImageTk

class HitboxEditorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("2D Engine - Hitbox Generator (Con Zoom)")

        self.pil_img = None
        self.tk_img = None
        self.zoom_factor = 4  # Zoom por defecto (4x)
        self.hitboxes = []    # Coordenadas en píxeles originales (x, y, w, h)
        self.rect_ids = []

        self.start_x = None
        self.start_y = None
        self.temp_rect_id = None

        self.setup_ui()

    def setup_ui(self):
        # Panel superior de controles
        top_bar = tk.Frame(self.root, bg="#1e1e1e", pady=5)
        top_bar.pack(side=tk.TOP, fill=tk.X)

        tk.Button(top_bar, text="📁 Cargar Sprite", command=self.load_image, bg="#2d2d2d", fg="white").pack(side=tk.LEFT, padx=5)
        
        tk.Label(top_bar, text="Zoom:", bg="#1e1e1e", fg="white").pack(side=tk.LEFT, padx=(15, 2))
        for z in [1, 2, 4, 8, 16]:
            tk.Button(top_bar, text=f"{z}x", command=lambda factor=z: self.set_zoom(factor), bg="#2d2d2d", fg="white", width=3).pack(side=tk.LEFT, padx=1)

        tk.Button(top_bar, text="↩️ Deshacer", command=self.undo_hitbox, bg="#2d2d2d", fg="white").pack(side=tk.LEFT, padx=(15, 2))
        tk.Button(top_bar, text="🗑️ Limpiar", command=self.clear_hitboxes, bg="#2d2d2d", fg="white").pack(side=tk.LEFT, padx=2)

        # Contenedor con barras de desplazamiento para el lienzo
        canvas_frame = tk.Frame(self.root, bg="#121212")
        canvas_frame.pack(side=tk.TOP, expand=True, fill=tk.BOTH)

        self.canvas = tk.Canvas(canvas_frame, bg="#121212", cursor="cross")
        self.canvas.pack(side=tk.TOP, expand=True, fill=tk.BOTH)

        # Eventos del ratón
        self.canvas.bind("<ButtonPress-1>", self.on_press)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        self.canvas.bind("<ButtonRelease-1>", self.on_release)

        # Panel inferior para código C++
        code_frame = tk.LabelFrame(self.root, text=" Código C++ Generado ", bg="#1e1e1e", fg="white", padx=5, pady=5)
        code_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=5, pady=5)

        self.code_output = tk.Text(code_frame, height=6, bg="#0d0d0d", fg="#00ff66", font=("Consolas", 10))
        self.code_output.pack(fill=tk.BOTH, expand=True)

    def load_image(self):
        path = filedialog.askopenfilename(filetypes=[("Imágenes", "*.png *.jpg *.jpeg *.bmp")])
        if not path:
            return

        self.pil_img = Image.open(path).convert("RGBA")
        self.hitboxes.clear()
        self.redraw_canvas()

    def set_zoom(self, factor):
        self.zoom_factor = factor
        if self.pil_img:
            self.redraw_canvas()

    def redraw_canvas(self):
        if not self.pil_img:
            return

        # Escalar imagen con Nearest Neighbor para preservar el pixel art nítido
        w, h = self.pil_img.width * self.zoom_factor, self.pil_img.height * self.zoom_factor
        resized_img = self.pil_img.resize((w, h), Image.NEAREST)
        self.tk_img = ImageTk.PhotoImage(resized_img)

        self.canvas.config(width=w, height=h)
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, image=self.tk_img, anchor=tk.NW)

        # Re-dibujar rectángulos escalados al zoom actual
        self.rect_ids.clear()
        for x, y, bw, bh in self.hitboxes:
            zx, zy = x * self.zoom_factor, y * self.zoom_factor
            zbw, zbh = bw * self.zoom_factor, bh * self.zoom_factor
            rid = self.canvas.create_rectangle(zx, zy, zx + zbw, zy + zbh, outline="#00ff00", width=2)
            self.rect_ids.append(rid)

        self.update_code_output()

    def on_press(self, event):
        if not self.pil_img:
            return
        self.start_x = event.x
        self.start_y = event.y
        self.temp_rect_id = self.canvas.create_rectangle(
            self.start_x, self.start_y, self.start_x, self.start_y,
            outline="#00ff00", width=2
        )

    def on_drag(self, event):
        if not self.pil_img or not self.temp_rect_id:
            return
        self.canvas.coords(self.temp_rect_id, self.start_x, self.start_y, event.x, event.y)

    def on_release(self, event):
        if not self.pil_img or not self.temp_rect_id:
            return

        end_x, end_y = event.x, event.y

        # Coordenadas en píxeles del lienzo (con zoom)
        lx = min(self.start_x, end_x)
        ly = min(self.start_y, end_y)
        lw = abs(end_x - self.start_x)
        lh = abs(end_y - self.start_y)

        # Convertir a píxeles reales de la imagen sin zoom
        orig_x = round(lx / self.zoom_factor)
        orig_y = round(ly / self.zoom_factor)
        orig_w = round(lw / self.zoom_factor)
        orig_h = round(lh / self.zoom_factor)

        if orig_w < 1 or orig_h < 1:
            self.canvas.delete(self.temp_rect_id)
            return

        self.hitboxes.append((orig_x, orig_y, orig_w, orig_h))
        self.temp_rect_id = None
        self.redraw_canvas()

    def undo_hitbox(self):
        if self.hitboxes:
            self.hitboxes.pop()
            self.redraw_canvas()

    def clear_hitboxes(self):
        self.hitboxes.clear()
        self.redraw_canvas()

    def update_code_output(self):
        self.code_output.delete("1.0", tk.END)

        if not self.hitboxes:
            self.code_output.insert(tk.END, "// Selecciona un nivel de zoom y dibuja las hitboxes sobre el sprite...")
            return

        lines = ["// Hitboxes compuestas (en píxeles base del sprite)"]
        for x, y, w, h in self.hitboxes:
            lines.append(f"newEntity.addHitbox(Vector2f({x}.0f, {y}.0f), Vector2f({w}.0f, {h}.0f));")

        self.code_output.insert(tk.END, "\n".join(lines))

if __name__ == "__main__":
    root = tk.Tk()
    app = HitboxEditorApp(root)
    root.mainloop()