# Comparativa ScanTailor vs ScanTailor Advanced — Guía para integrar funciones

Este documento compara las funciones del proyecto actual (Scan Tailor oficial/derivado) con [ScanTailor Advanced](https://github.com/4lex4/scantailor-advanced) y describe cómo añadir las funciones que faltan. El repositorio Advanced está clonado en `scantailor-advanced/` dentro del proyecto.

---

## 1. Resumen ejecutivo

| Área | Nuestro proyecto | ScanTailor Advanced |
|------|------------------|---------------------|
| **Estructura** | `filters/` en raíz, `ui/`, módulos en raíz | Todo en `src/` (core, app, imageproc, etc.) |
| **Filtros** | fix_orientation, page_split, deskew, select_content, page_layout, output | Los mismos + lógica extendida en cada uno |
| **Binarización** | Otsu, Mokji, Sauvola, Wolf en `imageproc` | Idem + **widgets UI** (Otsu/Sauvola/Wolf) en output |
| **Procesamiento por lotes** | Un hilo (`WorkerThread`) | **Varios hilos** (`WorkerThreadPool` + `QThreadPool`) |
| **UI** | Sin temas, sin barra de estado dedicada | **Temas claro/oscuro**, **StatusBarPanel**, **SystemLoadWidget** |
| **Output** | Un solo flujo de salida | **Split output** (foreground/background), **original background**, **RenderParams** |
| **Margins / Page** | Margins clásicos | **Guías**, **page box** interactivo, **auto/original alignment** mejorado |
| **Configuración** | Settings básicos | **DefaultParams** + perfiles, **Units** (px/mm/cm/in), **ApplicationSettings** |
| **Thumbnails** | ThumbnailSequence | + **OrderByDeviationProvider**, opciones de calidad/tamaño, orden por desviación |

---

## 2. Funciones de ScanTailor Advanced por categoría

### 2.1 Procesamiento y rendimiento

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Multi-threading en batch** | No. Un solo `WorkerThread`. | Sí. `WorkerThreadPool` + `QThreadPool`, número de hilos configurable. | Copiar `src/core/WorkerThreadPool.cpp/.h`; en `FilterTaskExecutor` (o equivalente) usar el pool en lugar de un único `WorkerThread`; añadir `SystemLoadWidget` para elegir nº de hilos. |
| **SystemLoadWidget** | No. | Sí. Widget para ver carga y elegir hilos durante el procesamiento. | Copiar `src/app/SystemLoadWidget.cpp/.h` y enlazar desde la ventana de batch. |

**Archivos clave en Advanced:**  
`src/core/WorkerThreadPool.cpp`, `WorkerThreadPool.h`, `src/app/SystemLoadWidget.cpp`, `SystemLoadWidget.h`, `MainWindow.cpp` (integración del widget y del pool).

---

### 2.2 Binarización adaptativa (output)

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Sauvola / Wolf en UI** | Algoritmos en `imageproc/Binarize.*`; **no** expuestos en el filtro Output. | Sí. Widgets `SauvolaBinarizationOptionsWidget`, `WolfBinarizationOptionsWidget`, `OtsuBinarizationOptionsWidget` en Output. | Añadir en `filters/output/`: los 3 widgets (clases + .ui), `BinarizationOptionsWidget.h` base; en `OptionsWidget` incluir el selector de método (Otsu/Sauvola/Wolf) y los parámetros; en `OutputGenerator` y `Params` usar el tipo y parámetros elegidos (window size, k, etc.). |
| **Parámetro k en Sauvola** | `binarizeSauvola(src, window_size)` sin k. | `binarizeSauvola(src, windowSize, k = 0.34)`. | Extender `imageproc/Binarize.cpp` para aceptar `k` y usar la misma fórmula que en Advanced. |
| **Parámetro k en Wolf** | Sin k. | `binarizeWolf(..., k = 0.3)`. | Igual: extender `Binarize.cpp` y la firma en `Binarize.h`. |

**Archivos clave en Advanced:**  
`src/core/filters/output/SauvolaBinarizationOptionsWidget.*`, `WolfBinarizationOptionsWidget.*`, `OtsuBinarizationOptionsWidget.*`, `BinarizationOptionsWidget.h`, `BlackWhiteOptions.cpp/.h`, `OptionsWidget.cpp` (sección binarización), `OutputGenerator.cpp` (llamadas a binarización), `Params.h/.cpp`, `RenderParams.*`.  
`src/imageproc/Binarize.cpp/.h` (firmas con `k`).

---

### 2.3 Salida (output) avanzada

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Splitting output** | No. | Sí. Salida separada: capa texto (foreground) y capa imágenes (background); opciones B&W o color en foreground. | Introducir `SplittingOptions`, `ForegroundType`; en `OutputGenerator` generar además máscara de foreground y, si aplica, imagen de background; escribir archivos extra (p. ej. sufijo `_fg`, `_bg`) según opciones. Reutilizar lógica de `OutputImageWithForegroundMask`, `OutputImageWithOriginalBackgroundMask`. |
| **Original background** | No. | Sí. Guardar fondo original en carpeta `original_background` con colores reservados (#010101, #fefefe). | Añadir modo en Params; en el pipeline de salida generar mapa de “original background” y guardar en subcarpeta con las convenciones de color de Advanced. |
| **Control total en output** | Parcial. | Control de relleno de márgenes, normalización de iluminación (antes binarización y en color), suavizado Savitzky-Golay y morfológico en la etapa de salida. | Revisar `OutputProcessingParams`, `RenderParams` y las opciones en `OptionsWidget` de Advanced; mapear cada opción a nuestro `Params`/`OutputImageParams` y al `OutputGenerator`. |
| **Relleno de zonas externas** | No documentado. | Opciones: Background (estimar color de fondo) o White. | Añadir opción en Params de output; en el renderizado, rellenar píxeles fuera del contenido con el color elegido (estimación de fondo o blanco). |
| **Compresión TIFF** | Por defecto del sistema. | Opciones: B&W (None, LZW, Deflate, CCITT G4) y color (None, LZW, Deflate, JPEG). | Añadir en Settings/OutputFileParams opciones de compresión; en el código que escribe TIFF (p. ej. `TiffWriter` si existe, o Qt) pasar los parámetros de compresión. |

**Archivos clave en Advanced:**  
`OutputImageBuilder.cpp/.h`, `OutputImagePlain.*`, `OutputImageWithForeground*.h/cpp`, `OutputImageWithOriginalBackground*.h/cpp`, `RenderParams.*`, `SplittingOptions.*`, `OutputProcessingParams.*`, `OutputGenerator.cpp` (flujos de salida y escritura).

---

### 2.4 Interfaz de usuario

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Temas claro/oscuro** | No. | Sí. `resources/light_scheme/`, `resources/dark_scheme/` (stylesheet.qss). | Añadir recursos de estilos; en Settings (o diálogo de preferencias) opción “Tema: Claro / Oscuro”; al cambiar, cargar el QSS correspondiente en la aplicación. |
| **StatusBarPanel** | No. | Sí. Muestra modo de zona, posición del ratón, tamaño físico, página actual, nombre y tipo [L]/[R]. | Copiar `src/app/StatusBarPanel.cpp/.h/.ui`; en `MainWindow` añadir un panel inferior (QStatusBar o widget fijo) y rellenarlo desde el filtro/vista actual (ImageViewBase, ZoneEditorBase, etc.). |
| **Unidades (px, mm, cm, in)** | Probablemente fijas (p. ej. mm). | `Units`, `UnitsProvider`, `UnitsConverter`, `UnitsListener`; afectan márgenes, DPI, etc. | Copiar módulo Units de `src/core/`; registrar `UnitsProvider`; donde se muestren/editen medidas, usar el convertidor y el proveedor; añadir en menú/ajustes la elección de unidad. |
| **Opciones de filtro colapsables** | No. | Sí. Los group boxes de opciones se pueden colapsar; estado persistido. | Usar QGroupBox con `setCheckable` o botón de colapsar; guardar estado (expanded/collapsed) en ApplicationSettings o por filtro. |
| **Zoom y foco al cambiar pestañas de output** | No. | Sí. Se guardan por pestaña. | En el widget de pestañas de output (Original, B&W, etc.), asociar a cada pestaña el zoom y el centro de la vista; al cambiar de pestaña, restaurar; al cambiar de página, actualizar según la página. |
| **Atajos Ctrl+1..5 para pestañas** | No. | Sí. | En la ventana principal o en el widget de output, `addAction` con QShortcut para cada pestaña. |

**Archivos clave en Advanced:**  
`src/app/StatusBarPanel.*`, `MainWindow.cpp` (status bar, temas, unidades); `src/core/Units*.cpp/.h`, `ImageViewBase.cpp` (zoom/focus); `src/resources/light_scheme/`, `dark_scheme/`.

---

### 2.5 Margins, página y contenido

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Guías (guides)** | No. | Sí. Líneas horizontales/verticales en la etapa de márgenes; arrastre con Ctrl+Alt+LMB; doble clic en contenido para acoplar a la guía más cercana. | Añadir en `filters/page_layout/`: `Guide.h/.cpp`, modelo de lista de guías; en la vista (ImageView), dibujar guías y manejar eventos (crear, mover, eliminar); guardar guías en Settings del filtro. |
| **Page box interactivo** | No (o fijo). | Sí. Caja de página editable como la de contenido; arrastre con Shift+LMB. | En select_content (y donde se use), tratar la “page box” como un rectángulo editable similar al content box; persistir en Settings. |
| **Modos de alineación Auto / Original** | Parcial. | Auto, Manual, Original; alineación vertical/horizontal por separado; considera page box del paso 4. | Revisar `page_layout` en Advanced: `Settings`, `CacheDrivenTask` y la lógica de alineación; portar opciones y cálculos a nuestro page_layout/margins. |
| **Desviación (deviation)** | No. | Sí. Destacar páginas con skew alto, contenido de tamaño distinto o márgenes que no coinciden; ordenar por desviación; asteriscos rojos en thumbnails. | Implementar `DeviationProvider` (caché, recálculo bajo demanda) y `OrderByDeviationProvider`; en thumbnails usar el proveedor para ordenar y para pintar el indicador; opción en ajustes para mostrar/ocultar asteriscos. |
| **Auto-ajuste del área de contenido** | No. | Sí. Doble clic en contenido: expande o ajusta el borde más cercano (con Shift/Ctrl para restringir ejes). | En la vista de select_content, en el manejador de doble clic: calcular el borde más próximo al clic y ajustar el content box (expandir si el contenido se sale, o mover el borde al borde del contenido). |

**Archivos clave en Advanced:**  
`src/core/filters/page_layout/Guide.*`, `ImageView.cpp`, `Settings.*`, `Filter.cpp`; `src/core/filters/select_content/` (page box, content box); `src/core/DeviationProvider.*`, `OrderByDeviationProvider.*`, `ThumbnailBase.cpp`; `ApplicationSettings` (opción de asteriscos).

---

### 2.6 Zonas (picture zones, fill zones)

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Modos de creación Z/X/C** | No. | Z = poligonal, X = lasso, C = rectangular. | En `ZoneEditorBase` (o equivalente) definir estados por tecla; Z/X/C cambian el modo de creación; al dibujar, usar el polígono, la selección libre o el rectángulo según el modo. |
| **Shift+LMB arrastrar zona** | No. | Sí. | En la vista de zonas, detectar Shift+LMB sobre una zona y entrar en modo “arrastrar zona”; actualizar geometría al soltar. |
| **Ctrl+Shift+LMB copiar zona al arrastrar** | No. | Sí. | Igual que arriba, pero duplicar la zona en la posición final. |
| **Ctrl+Alt+Click copiar última zona** | No. | Sí. | Guardar referencia a la última zona creada; en Ctrl+Alt+Click en un punto, crear una copia centrada en ese punto. |
| **Suprimir zona con Del** | No. | Sí. | Con foco en la vista de zonas, keyPress Del elimina la zona bajo el cursor. |
| **Tecla D en vértice para borrar vértice** | No. | Sí. | En modo edición de polígono, si el cursor está sobre un vértice, D elimina ese vértice. |
| **Ctrl al arrastrar vértice = ángulo recto** | No. | Sí. | Al arrastrar un vértice con Ctrl pulsado, restringir a 90° respecto al siguiente/anterior. |
| **Forma de imagen rectangular** | Parcial. | Modo “Rectangular” con sensibilidad; fusión de Quadro y Rectangular. | En output, añadir opción “Rectangular” y parámetro de sensibilidad; en la detección de formas de imagen, generar rectángulos con la lógica de Advanced (PictureShapeOptions). |

**Archivos clave en Advanced:**  
`src/core/ZoneEditorBase.cpp` (modos, atajos, arrastre); `src/core/filters/output/PictureShapeOptions.*`, detección de formas en output.

---

### 2.7 Dewarping

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Dewarping marginal** | No. | Sí. Modo automático para bordes superior/inferior curvos sobre fondo negro. | Implementar detección de bordes curvos y ajuste de la malla de dewarping a esos bordes; añadir modo “Marginal” en opciones de dewarping. |
| **Arrastre vertical de línea con Ctrl** | No. | Sí. En la línea superior/inferior de la malla, Ctrl+LMB en el punto izquierdo/derecho mueve la línea entera. | En `DewarpingView`, en el manejo de ratón, detectar Ctrl + agarre en extremos de la línea horizontal; mover todos los puntos de esa línea en vertical. |
| **Eliminación de 3 puntos rojos centrales** | No. | Sí. Menos puntos en la malla para simplificar. | Ajustar el modelo de malla y la vista para no dibujar/editar esos puntos. |
| **Cambio automático a modo Manual al mover la malla** | No. | Sí. | Al detectar movimiento de un punto de la malla por el usuario, poner el modo en Manual. |

**Archivos clave en Advanced:**  
`src/core/filters/output/DewarpingView.cpp`, `DewarpingOptions.*`, lógica de dewarping en `OutputGenerator` o módulo dewarping.

---

### 2.8 Thumbnails y navegación

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Vista multicolumna de thumbnails** | Parcial. | Mejorada; thumbnails repartidos de forma uniforme. | Revisar `ThumbnailSequence` y el layout; igualar distribución y número de columnas. |
| **Calidad y tamaño de thumbnails configurables** | No. | Sí. Calidad y tamaño en Settings; caché por calidad. | Añadir opciones en Settings; en `ThumbnailPixmapCache` (o equivalente) usar tamaño/calidad y generar cachés distintos si hace falta. |
| **Ordenar por desviación** | No. | Sí. | Con `OrderByDeviationProvider`, ofrecer en el combo de ordenación “Por desviación (desc)”. |
| **Asteriscos rojos en páginas con alta desviación** | No. | Sí. Opción en Settings. | Al pintar el thumbnail, si `DeviationProvider` marca la página como desviada, dibujar un indicador (asterisco); opción para activar/desactivar. |
| **Selección múltiple sin teclado** | No. | Sí. | Modo “selección múltiple” (p. ej. botón o estado); clic añade/quita de la selección. |
| **Navegación entre páginas seleccionadas** | No. | Shift+PgUp/PgDown (o Q/W). | Con una lista de páginas seleccionadas, Shift+PgUp/PgDown cambia a la anterior/siguiente en esa lista. |
| **Ir a página por número (Ctrl+G)** | No. | Sí. | Diálogo o campo “Ir a página”; al aceptar, cambiar la página actual al número indicado. |
| **Persistir selección al cambiar de filtro** | No. | Sí. | Guardar índices de páginas seleccionadas al cambiar de paso; al volver, restaurar la selección. |

**Archivos clave en Advanced:**  
`src/app/ThumbnailSequence.cpp`, `MainWindow.cpp` (navegación, atajos); `src/core/ThumbnailPixmapCache.*`, `OrderByDeviationProvider.*`, `ApplicationSettings.*`.

---

### 2.9 Configuración y persistencia

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Default params (perfiles)** | No. | Sí. Perfiles “Default” y “Source”; perfiles de usuario en `config/profiles`; se aplican al crear proyecto nuevo. | Implementar `DefaultParams`, `DefaultParamsProvider`, `DefaultParamsProfileManager`; diálogo `DefaultParamsDialog` para editar perfiles; al crear proyecto, cargar el perfil seleccionado como valores por defecto de cada filtro. |
| **ApplicationSettings** | Parcial (QSettings). | Centralizado: temas, unidades, thumbnails, desviación, auto-guardado, compresión TIFF, etc. | Ampliar nuestro uso de QSettings (o un singleton ApplicationSettings) con las mismas claves que Advanced y usarlas en MainWindow, filtros y diálogos. |
| **Auto-guardado de proyecto** | No. | Sí. Opción en Settings; también durante batch. | Timer o señal periódica; si el proyecto está guardado y hay cambios, llamar a guardar; durante batch, guardar al terminar cada página o cada lote. |
| **Portabilidad** | Depende del build. | Config/datos en la carpeta del ejecutable si es posible; si no, en rutas del sistema. | Al iniciar, comprobar si el directorio del ejecutable es escribible; si sí, usar `app_dir/config`, `app_dir/data`; si no, usar QStandardPaths. |
| **Black on white detection** | No. | Sí. Detección automática de contenido claro sobre fondo oscuro; corrección de algoritmos automáticos; opción por página en output. | Añadir `BlackOnWhiteEstimator`; en select_content y output, usar la estimación para invertir o ajustar umbrales; opción en Output por página. |

**Archivos clave en Advanced:**  
`src/core/DefaultParams.*`, `DefaultParamsProvider.*`, `DefaultParamsProfileManager.*`, `ApplicationSettings.*`; `src/app/DefaultParamsDialog.*`, `SettingsDialog.*`; `src/core/BlackOnWhiteEstimator.*`.

---

### 2.10 Page split y detección de página

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Rellenar recorte (fill offcut)** | No. | Sí. Las opciones de page split influyen en el relleno de zonas fuera de página. | En la generación de salida y en la vista de page split, tener en cuenta el “offcut” y rellenarlo según la opción. |
| **Cutters que no se cruzan** | No. | Sí. Validación en UI para que los cortes no se solapen. | En el editor de cortes, al mover un cutter, comprobar intersección con otros y limitar el movimiento o rechazar. |
| **Aplicar corte a páginas de distinto tamaño** | Rechazo o reset. | Adaptar cutters en lugar de rechazar. | Al aplicar un corte de una página a otras de dimensiones distintas, escalar o desplazar los cutters para que queden dentro de la página destino. |
| **Page detect en márgenes negros** | Parcial. | Opción para detectar página en márgenes negros o desactivar detección. | En select_content o page_split, añadir opción “Page detect” y usar la detección de borde de página en imágenes con bordes negros. |

**Archivos clave en Advanced:**  
`src/core/filters/page_split/Task.cpp`, `Filter.cpp`, `OptionsWidget` (cutters, apply); `src/core/filters/select_content/` (page detect).

---

### 2.11 Segmentación y color

| Función | En nuestro proyecto | En Advanced | Cómo añadirla |
|--------|----------------------|-------------|----------------|
| **Color segmenter** | No. | Sí. Segmentar imagen en regiones de color; colorizar máscara B&W. | Añadir módulo de segmentación (p. ej. por k-means o por componentes conectadas en espacio de color); en output, opción para generar máscara coloreada. |
| **Posterización** | No. | Sí. Reducir número de colores (cuantización); útil para DjVu y compresión. | Implementar cuantización de color (paleta fija o adaptativa); opción en output para posterizar antes de guardar. |

**Archivos clave en Advanced:**  
Lógica en `OutputGenerator` y posiblemente en `imageproc` (buscar “posteriz”, “segment” en el código).

---

## 3. Orden sugerido para integrar

1. **Bajo esfuerzo y alto impacto**  
   - StatusBarPanel (información de contexto).  
   - Unidades (px/mm/cm/in) para márgenes y DPI.  
   - Atajos Ctrl+1..5 y Ctrl+G.

2. **Rendimiento**  
   - WorkerThreadPool + SystemLoadWidget (multi-threading en batch).

3. **Output**  
   - Widgets de binarización (Otsu/Sauvola/Wolf) en UI y parámetro k en `imageproc`.  
   - Opciones de compresión TIFF.  
   - Relleno de zonas externas (fondo/blanco).

4. **UX de zonas y márgenes**  
   - Guías en page_layout.  
   - Modos de zona Z/X/C y atajos (Shift+LMB, Del, D, Ctrl en vértice).  
   - Auto-ajuste del área de contenido (doble clic).

5. **Funciones más grandes**  
   - Splitting output y original background.  
   - Deviation (DeviationProvider, orden y asteriscos).  
   - Default params y perfiles.  
   - Temas claro/oscuro.

---

## 4. Diferencias de estructura de código

- **Rutas:** En nuestro proyecto los filtros están en `filters/<nombre>/` en la raíz; en Advanced están en `src/core/filters/<nombre>/`. Al portar, adaptar `#include` y CMake.
- **Output:** Advanced separa `OutputImage`, `OutputImageBuilder`, `OutputImagePlain`, `OutputImageWithForeground*`, `OutputImageWithOriginalBackground*`, `RenderParams`, `OutputProcessingParams`. Nosotros tenemos un `OutputGenerator` más monolítico; conviene extraer pasos a clases similares para poder añadir split y original background sin duplicar demasiado.
- **Core vs app:** En Advanced, la lógica de filtros y modelos está en `core`; la UI (MainWindow, diálogos, StatusBarPanel) en `app`. Nosotros tenemos `ui/` y lógica repartida; para StatusBarPanel y SystemLoadWidget puede crearse un equivalente bajo `ui/` o donde se gestionen las ventanas.

---

## 5. Referencia rápida de archivos en scantailor-advanced

```
scantailor-advanced/
├── src/
│   ├── app/
│   │   ├── MainWindow.cpp, .h, .ui
│   │   ├── StatusBarPanel.cpp, .h, .ui
│   │   ├── SystemLoadWidget.cpp, .h
│   │   ├── SettingsDialog.cpp, .h, .ui
│   │   ├── DefaultParamsDialog.cpp, .h, .ui
│   │   └── ThumbnailSequence.cpp, .h
│   ├── core/
│   │   ├── WorkerThreadPool.cpp, .h
│   │   ├── DeviationProvider.cpp, .h
│   │   ├── OrderByDeviationProvider.cpp, .h
│   │   ├── ApplicationSettings.cpp, .h
│   │   ├── DefaultParams*.cpp, .h
│   │   ├── Units*.cpp, .h
│   │   ├── ZoneEditorBase.cpp (modos zona)
│   │   ├── ImageViewBase.cpp (zoom/focus, status)
│   │   └── filters/
│   │       ├── page_layout/Guide.cpp, .h, Settings, ImageView
│   │       ├── output/
│   │       │   ├── SauvolaBinarizationOptionsWidget.*
│   │       │   ├── WolfBinarizationOptionsWidget.*
│   │       │   ├── OtsuBinarizationOptionsWidget.*
│   │       │   ├── SplittingOptions.*
│   │       │   ├── RenderParams.*
│   │       │   ├── OutputImage*.cpp, .h
│   │       │   └── OutputGenerator.cpp
│   │       └── ...
│   ├── imageproc/Binarize.cpp, .h (firmas con k)
│   └── resources/
│       ├── light_scheme/stylesheet/stylesheet.qss
│       └── dark_scheme/stylesheet/stylesheet.qss
```

---

## 6. Licencia

ScanTailor Advanced está bajo GPL-3.0. Cualquier código que se copie o adapte de ese repositorio debe mantener la licencia GPL-3.0 y los avisos de copyright correspondientes.

---

---

## 7. Estado de integración (implementado)

**Fase 0 — Completada**
- `ApplicationSettings` (singleton con QSettings): tema, unidades, auto-guardado, compresión TIFF, desviación, thumbnails, perfiles, hilos de batch, etc.
- Sistema de **Unidades**: `Units`, `UnitsProvider`, `UnitsConverter`, `UnitsListener` (px, mm, cm, in).

**Fase 1 — Completada**
- **StatusBarPanel**: panel en la barra de estado con página actual (p. N / total) y nombre/tipo [L]/[R].
- **Temas**: estilos mínimo claro/oscuro en `resources/stylesheets/`; carga en `main.cpp` según `ApplicationSettings::getColorScheme()`.
- **Atajos**: Ctrl+G (ir a página por número), Ctrl+1..5 (cambiar pestaña de salida cuando el filtro activo es Output).
- **Hilos de batch**: al iniciar procesamiento por lotes se usa `ApplicationSettings::getBatchThreadCount()` para configurar `QThreadPool::globalInstance()->setMaxThreadCount()` (0 = automático).
- **SettingsDialog**: pestañas General, Thumbnails y Output/TIFF con: esquema de color, auto-guardado, unidades, hilos de batch, detección Black on white (y “aplicar en output”), calidad/tamaño de miniatura, columna única, resaltar desviación, pregunta al cancelar selección, compresión TIFF B&W y Color. Todo persistido en `ApplicationSettings`.

**Fase 2 (parcial) — Completada**
- **TiffWriter**: usa `ApplicationSettings::getTiffBwCompression()` y `getTiffColorCompression()` para imágenes bitonales/indexadas y RGB/ARGB.
- **Thumbnails**: `Utils::createThumbnailCache()` usa `ApplicationSettings::getThumbnailQuality()`; `MainWindow` usa `getMaxLogicalThumbnailSize()` para la secuencia de miniaturas.
- **DeviationProvider** (template) y **OrderByDeviationProvider**: añadidos en core. **Deskew**: `Settings` mantiene un `DeviationProvider<PageId>` actualizado con el ángulo de deskew por página; el **Filter** ofrece la opción “Order by decreasing deviation” en el orden de páginas. Especializaciones `std::hash<ImageId>` y `std::hash<PageId>` para uso en `DeviationProvider`. Pendiente: asteriscos rojos en thumbnails (requiere pasar flag “deviant” desde CacheDrivenTask a ThumbnailBase/Thumbnail/IncompleteThumbnail).

**Fases 3–9**: pendientes; seguir el plan por fases y la referencia de archivos en `scantailor-advanced/`.

---

*Documento generado a partir de la comparación entre el proyecto en este repositorio y el clon de [4lex4/scantailor-advanced](https://github.com/4lex4/scantailor-advanced) en `scantailor-advanced/`.*
