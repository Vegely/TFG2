import numpy as np
import matplotlib.pyplot as plt
import sys

# --- CONFIGURATION ---
FILE_PATH = "psoc_trng_1gb.bin"   # Su archivo de datos
# IMPORTANTE: Crear datos de prueba si el archivo no existe para demostración
try:
    with open(FILE_PATH, 'rb') as f:
        pass
except FileNotFoundError:
    print(f"Aviso: '{FILE_PATH}' no encontrado. Creando datos aleatorios de prueba.")
    # Generar 1MB de datos aleatorios para probar el script
    dummy_data = np.random.randint(0, 256, 1000000, dtype=np.uint8)
    with open(FILE_PATH, 'wb') as f:
        dummy_data.tofile(f)

SAMPLES_TO_READ = 10000000      # Leer 10 MB (suficiente para visualización)
OUTPUT_FORMAT = "pdf"           # Formato solicitado
OUTPUT_FILENAME = f"analisis_combinado.{OUTPUT_FORMAT}"

def generate_spanish_plots():
    try:
        # 1. Cargar Datos
        print(f"Leyendo datos de {FILE_PATH}...")
        data = np.fromfile(FILE_PATH, dtype=np.uint8, count=SAMPLES_TO_READ)
        
        if data.size == 0:
            print("Error: ¡El archivo está vacío!")
            return

        # Aplicar estilo gráfico profesional
        try:
            plt.style.use('seaborn-v0_8-whitegrid')
        except:
            plt.style.use('bmh')

        # ==========================================
        # CONFIGURACIÓN DE LA FIGURA PRINCIPAL Y SUBGRÁFICOS
        # ==========================================
        print("Generando imagen combinada...")
        # Crear 1 fila, 3 columnas.
        # figsize=(Ancho, Alto) - debe ser ancho para acomodar los 3 gráficos
        fig, axes = plt.subplots(nrows=1, ncols=3, figsize=(24, 7))
        
        # 'axes' es ahora una lista de 3 objetos de gráfico: [ax0, ax1, ax2]

        # ==========================================
        # SUBGRÁFICO 1: HISTOGRAMA (usa axes[0])
        # ==========================================
        # Título basado en su descripción: "Histograma de Frecuencias"
        ax0 = axes[0]
        ax0.hist(data, bins=256, range=(0, 255), 
                  color='#4C72B0', edgecolor='none', alpha=0.9, density=True)
        
        ax0.set_xlim(0, 255)
        # Usando el título proporcionado en la descripción
        ax0.set_title("Histograma de Frecuencias", fontsize=14, fontweight='bold', pad=15)
        # Etiquetas traducidas al español
        ax0.set_xlabel("Valor del Byte (0–255)", fontsize=12)
        ax0.set_ylabel("Densidad de Probabilidad", fontsize=12)
        ax0.grid(axis='x', alpha=0)

        # ==========================================
        # SUBGRÁFICO 2: BITMAP (usa axes[1])
        # ==========================================
        # Título basado en su descripción: "Bitmap (Mapa de Bits)"
        ax1 = axes[1]
        side = int(np.sqrt(data.size))
        if side > 1000: side = 1000 # Limitar tamaño para rendimiento
        img_data = data[:side*side].reshape((side, side))
        
        ax1.imshow(img_data, cmap='gray', vmin=0, vmax=255, interpolation='nearest')
        
        ax1.axis('off')
        # Usando el título proporcionado en la descripción
        ax1.set_title(f"Bitmap (Mapa de Bits) - {side}x{side}", fontsize=14, fontweight='bold', pad=15)
        # Asegurar que se mantenga cuadrado
        ax1.set_aspect('equal')

        # ==========================================
        # SUBGRÁFICO 3: LAG PLOT (usa axes[2])
        # ==========================================
        # Título basado en su descripción: "Lag Plot (Gráfico de Retardo)"
        ax2 = axes[2]
        x_lag = data[:-1]
        y_lag = data[1:]
        
        # Graficar solo los primeros puntos para mantener el PDF ligero y limpio
        ax2.scatter(x_lag[:8000], y_lag[:8000], s=3, alpha=0.4, color='#2F4F4F', edgecolors='none')
        
        ax2.set_xlim(0, 255)
        ax2.set_ylim(0, 255)
        # Crucial: aspecto cuadrado para interpretar correctamente la correlación
        ax2.set_aspect('equal', adjustable='box')
        
        # Usando el título proporcionado en la descripción
        ax2.set_title("Lag Plot (Gráfico de Retardo)", fontsize=14, fontweight='bold', pad=15)
        # Etiquetas traducidas al español
        ax2.set_xlabel("Byte Actual ($x_i$)", fontsize=12)
        ax2.set_ylabel("Byte Siguiente ($x_{i+1}$)", fontsize=12)
        
        ticks = [0, 64, 128, 192, 255]
        ax2.set_xticks(ticks)
        ax2.set_yticks(ticks)

        # ==========================================
        # GUARDADO FINAL
        # ==========================================
        # Ajusta automáticamente el espaciado para que las etiquetas no se superpongan
        plt.tight_layout()
        
        print(f"Guardando imagen combinada en {OUTPUT_FILENAME}...")
        # Se guarda como PDF según lo solicitado
        plt.savefig(OUTPUT_FILENAME, format=OUTPUT_FORMAT)
        plt.close()

        print("\n¡Éxito! El archivo PDF ha sido generado.")

    except FileNotFoundError:
        print(f"Error: No se pudo encontrar '{FILE_PATH}'. Verifique el nombre del archivo.")
    except Exception as e:
        print(f"Ocurrió un error: {e}", file=sys.stderr)

if __name__ == "__main__":
    generate_spanish_plots()