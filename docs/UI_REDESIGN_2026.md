# Rediseño UI ScanTailor — Estándares 2026

Documento de arquitectura y guías para una interfaz moderna, orientada a productividad en procesamiento masivo de páginas escaneadas.

---

## 1. Arquitectura de ventana principal

### 1.1 Layout objetivo

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│  TOP TOOLBAR                                                                     │
│  [Logo] [Proyecto]  │  [Pipeline: Split→Deskew→Margins→Dewarp→Binarize→Output]   │
│  [Nuevo][Abrir][Guardar]  [Cmd Palette ⌘K]  [Zoom] [Vista] [Batch] [Export]     │
├──────────────┬────────────────────────────────────────────────┬─────────────────┤
│              │                                                 │                 │
│  LEFT        │              CENTER                             │  RIGHT          │
│  SIDEBAR     │              WORKSPACE                          │  SIDEBAR        │
│  (Thumbs)    │              (Page preview)                     │  (Params)       │
│              │                                                 │                 │
│  ┌────────┐  │  ┌─────────────────────────────────────────┐  │  ┌────────────┐ │
│  │  ⋮     │  │  │                                         │  │  │ Parámetros │ │
│  │ thumb  │  │  │         QGraphicsView / ImageView        │  │  │ del paso   │ │
│  │ thumb  │  │  │         Zoom, grid, raw/processed       │  │  │ actual     │ │
│  │ thumb  │  │  │                                         │  │  │            │ │
│  │  ⋮     │  │  └─────────────────────────────────────────┘  │  │ Presets    │ │
│  │ [Sort] │  │                                                 │  │ Aplicar a  │ │
│  └────────┘  │                                                 │  │ selección  │ │
│              │                                                 │  └────────────┘ │
├──────────────┴────────────────────────────────────────────────┴─────────────────┤
│  BOTTOM PANEL — Batch / Export                                                  │
│  [Procesar selección] [Procesar todo] [Exportar]  │  Progreso  │  [Detener]     │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 Widget hierarchy (Qt)

```
QMainWindow (MainWindow)
├── QWidget centralWidget
│   ├── QVBoxLayout (root)
│   │   ├── TopToolbar (QWidget / QToolBar)
│   │   ├── QSplitter (horizontal, main content)
│   │   │   ├── QDockWidget | QSplitter — Left: Thumbnails
│   │   │   │   └── ThumbnailDockContent
│   │   │   │       ├── QToolButton (focus/keep in view)
│   │   │   │       ├── QGraphicsView (thumbView) → ThumbnailSequence
│   │   │   │       └── QComboBox (sortOptions)
│   │   │   ├── QFrame (center workspace)
│   │   │   │   └── QStackedLayout
│   │   │   │       ├── ImageView (filter-specific: ImageViewBase subclasses)
│   │   │   │       ├── NewOpenProjectPanel
│   │   │   │       ├── ProcessingIndicationWidget
│   │   │   │       └── BatchProcessingWidget
│   │   │   └── QDockWidget | QSplitter — Right: Parameters
│   │   │       └── QScrollArea
│   │   │           └── FilterOptionsWidget (stacked by stage)
│   │   └── BottomPanel (QWidget)
│   │       └── Batch/Export controls + progress
│   ├── QMenuBar
│   └── QStatusBar
```

- **Thumbnails**: izquierda (sidebar), no derecha, para flujo lectura L→R y más espacio al preview.
- **Pipeline**: en barra superior como tira de etapas clicables (o en un toolbar dedicado).
- **Parámetros**: panel derecho colapsable/redimensionable.
- **Batch/Export**: barra inferior fija o colapsable.

---

## 2. Wireframe de la ventana principal

### 2.1 Zonas y proporciones

| Zona            | Ancho sugerido      | Redimensionable | Contenido principal                    |
|-----------------|---------------------|-----------------|----------------------------------------|
| Left sidebar    | 220–280 px          | Sí (QSplitter)  | Lista de miniaturas, orden, “en vista” |
| Center          | Flex (1)            | Sí              | Preview de página, zoom, overlay       |
| Right sidebar   | 280–360 px          | Sí              | Controles del filtro, presets, aplicar |
| Bottom panel    | Altura ~56–72 px    | Colapsable      | Batch, export, progreso                |
| Top toolbar     | Altura ~48–56 px    | No              | Acciones, pipeline, command palette    |

### 2.2 Pipeline visual

Representación clara del flujo:

**Split → Deskew → Margins → Dewarp → Binarization → Output**

- Cada etapa: icono + etiqueta corta.
- Etapa actual: resaltada (fondo suave, borde sutil).
- Etapas anteriores: opcionalmente con “check” o tono más neutro.
- Separador: línea fina o flecha (→), no cajas pesadas.
- Clic en etapa: cambiar filtro activo (equivalente al actual StageListView).
- Opcional: barra de progreso por etapa (p. ej. “listas” para batch).

### 2.3 Panel de miniaturas (izquierda)

- **QGraphicsView** con **ThumbnailSequence** (sin cambio de modelo lógico).
- Scroll vertical fluido; virtualización si hay >100 páginas.
- Ítem: miniatura + número de página (y opcional estado: listo / pendiente / error).
- Agrupación opcional: por “documento” o por rango (ej. 1–50, 51–100).
- Atajos: Home/End, PgUp/PgDn, Q/W para anterior/siguiente.
- Botón “mantener página en vista” (focus) arriba o integrado.
- Combo de orden (sort) al pie del panel.

### 2.4 Área central (preview)

- **ImageViewBase** (o subclase) dentro de **QScrollArea** o **QGraphicsView** según el filtro.
- Controles de vista:
  - Zoom: 25% / 50% / 100% / Ajustar / Lupa.
  - Cuadrícula (overlay) on/off.
  - Alternar imagen cruda / procesada (before/after).
- Fondo: neutro (ej. #F5F5F6) para no competir con la página.
- Bordes del viewport: redondeados 6–8 px, sin marco pesado.

### 2.5 Panel derecho (parámetros)

- **QScrollArea** con **QWidget** contenedor.
- Secciones colapsables (QToolButton + QFrame) por grupo lógico.
- Controles: sliders con valor numérico visible, combos, checks.
- Presets: botones o combo “Libro”, “Revista”, “Fotocopia”, “Personalizado”.
- Botones de aplicación:
  - “Aplicar a página actual”
  - “Aplicar a selección”
  - “Aplicar a todas”
- Los sliders que afectan geometría/visual deben disparar **preview en vivo** (throttled, p. ej. 150 ms).

### 2.6 Panel inferior (batch/export)

- Botones: “Procesar selección”, “Procesar todo”, “Exportar”.
- Barra de progreso + texto (p. ej. “Página 23/400”).
- “Detener” durante batch.
- Opciones: “Beep al terminar”, “Cerrar al terminar” (opcional).

---

## 3. Sistema visual (2026)

### 3.1 Principios

- **Minimalismo estructurado**: poco ruido, jerarquía clara, bloques de blanco.
- **Superficies suaves**: fondos no planos (#FAFAFA, #F0F0F2), bordes 1px (#E5E5E7).
- **Esquinas redondeadas**: 6–10 px en tarjetas, paneles, botones.
- **Iconografía**: iconos 20–24 px, estilo lineal/outline; color primario para activo.

### 3.2 Paleta sugerida

| Uso           | Variable / Color | Uso en UI                    |
|---------------|------------------|------------------------------|
| Fondo app     | #F5F5F6          | centralWidget, paneles       |
| Fondo panel   | #FAFAFA          | dock contents, cards         |
| Borde sutil   | #E5E5E7          | separadores, bordes          |
| Texto primario| #1A1A1A          | títulos, labels              |
| Texto secundario | #6B6B6B        | hints, secundario            |
| Acento        | #2563EB (blue)   | links, botón primario, paso activo |
| Hover         | #E8E8EA          | filas, botones               |
| Éxito         | #16A34A          | estado “listo”               |
| Error         | #DC2626          | errores, advertencias        |

### 3.3 Tipografía

- **UI**: sans-serif del sistema (SF Pro, Segoe UI, Inter) 13–14 px.
- **Números / DPI**: monospace 12–13 px donde sea relevante.
- **Títulos de sección**: 15 px, peso medio.

### 3.4 Componentes

- **Separadores**: `QFrame::HLine` con altura 1 px, color #E5E5E7.
- **Botones**: altura 32–36 px, padding horizontal 12–16 px, border-radius 6–8 px.
- **Sliders**: altura 6 px, handle 16 px, border-radius 3 px.
- **Inputs**: altura 32 px, border 1 px #E5E5E7, focus ring 2 px acento.
- **Cards (secciones)**: fondo #FAFAFA, border-radius 8 px, padding 12 px.

---

## 4. Mejoras de UX detalladas

### 4.1 Pipeline

- Sustituir **StageListView** (tabla vertical) por una **barra horizontal de etapas** en la parte superior.
- Cada etapa muestra nombre corto + icono; la activa tiene fondo suave y borde inferior o anillo.
- Tooltip con descripción del paso.
- Opcional: indicador de “todas las páginas listas” en esa etapa (para batch).

### 4.2 Parámetros

- Agrupar en **QGroupBox** o secciones colapsables con título.
- **Presets**: “Libro”, “Revista”, “Fotocopia” aplican conjuntos de valores por defecto.
- **Aplicar a**: “Página actual” (por defecto), “Páginas seleccionadas”, “Todas las páginas”.
- Sliders con **actualización en vivo** del preview (debounce 100–200 ms) para no bloquear UI.
- Valores numéricos editables junto al slider (spinbox o QLineEdit con validación).

### 4.3 Navegación de páginas

- Scroll rápido en la lista de miniaturas (virtualización si hace falta).
- Atajos: Home, End, PgUp, PgDn, Q (anterior), W (siguiente).
- Opcional: “Ir a página” (Ctrl+G) con diálogo o campo en toolbar.
- Agrupación visual por bloques (p. ej. cada 50 páginas) para proyectos muy grandes.

### 4.4 Preview

- **Zoom**: niveles fijos + “Ajustar a ventana” + rueda con Ctrl.
- **Grid**: overlay de cuadrícula (opcional) para alineación.
- **Before/After**: conmutar entre imagen de entrada del paso e imagen de salida.
- **Vista dividida**: opcional, mitad raw / mitad procesada (arrastrable).

### 4.5 Productividad

- **Command palette**: Ctrl+K (o Ctrl+Shift+P) abre un **QLineEdit** tipo búsqueda con acciones (Abrir, Guardar, Ir a página, Cambiar etapa, Procesar todo, etc.).
- **Atajos** documentados en menú Help y en tooltips.
- **Batch**: un solo lugar (panel inferior) para “Procesar selección” / “Procesar todo” y progreso.

---

## 5. Rendimiento

- **Thumbnails**: generación en background (ya existente); considerar **Lazy loading** y caché por viewport (solo pintar miniaturas visibles).
- **Preview**: mantener **downscaled pixmap** para interacción en tiempo real; HQ en delay (ya en ImageViewBase).
- **Sliders**: no recalcular pipeline completo en cada tick; **throttle** (timer 100–200 ms) y/o solo actualizar cuando se suelta el slider si el paso es costoso.
- **Grandes TIFF**: no decodificar imagen completa para miniatura; usar **subregion** o **resolución reducida** si la librería lo permite.
- **High DPI**: escalado Qt (Qt::AA_EnableHighDpiScaling) y recursos @2x para iconos críticos.

---

## 6. Resumen de cambios respecto al UI actual

| Aspecto        | Actual                         | Propuesto 2026                          |
|----------------|--------------------------------|-----------------------------------------|
| Pipeline       | Lista vertical (StageListView) | Barra horizontal de etapas en toolbar   |
| Thumbnails     | Panel derecho                  | Panel izquierdo (QDockWidget/Splitter)   |
| Parámetros     | Debajo del pipeline, izquierda | Panel derecho, secciones colapsables    |
| Batch          | Botón en lista de etapas       | Panel inferior dedicado                 |
| Navegación     | Home/End/PgUp/PgDn             | Igual + Command palette (Ctrl+K)        |
| Presets        | No unificado                   | Presets por etapa + “Aplicar a”          |
| Estética       | Estilo clásico Qt              | Fondos suaves, bordes finos, 6–8 px radius |
| Preview        | ImageViewBase                  | + grid, before/after, zoom claro         |

Este documento sirve como referencia para implementar el rediseño de forma incremental sin romper la lógica existente (StageSequence, ThumbnailSequence, FilterOptionsWidget, ImageViewBase).

---

## 7. Jerarquía de widgets (detallada)

### 7.1 MainWindow

```
QMainWindow
├── setMenuBar(QMenuBar)
│   └── File, Tools, Help
├── setCentralWidget(QWidget centralWidget)
│   └── QVBoxLayout (margin 0, spacing 0)
│       ├── TopToolbarWidget (altura fija ~48px)
│       ├── QSplitter* mainSplitter (stretch center=1)
│       │   ├── LeftDockPlaceholder (ancho min 220, max 400)
│       │   ├── CenterStackWidget (stretch 1)
│       │   └── RightDockPlaceholder (ancho min 260, max 420)
│       └── BottomPanelWidget (altura fija ~56px, colapsable)
└── setStatusBar(QStatusBar)
```

### 7.2 TopToolbarWidget

```
QWidget
└── QHBoxLayout (spacing 8, margins 8 4)
    ├── Logo / App name (opcional)
    ├── QFrame (separator vertical)
    ├── PipelineStripWidget (ver 7.3)
    ├── QWidget (stretch)
    ├── QToolButton (Command palette, Ctrl+K)
    ├── QComboBox o QToolButton (Zoom)
    ├── QToolButton (Vista: grid, before/after)
    ├── QToolButton (Batch)
    └── QToolButton (Export)
```

### 7.3 PipelineStripWidget

```
QWidget
└── QHBoxLayout (spacing 4)
    ├── PipelineStepButton "Split"
    ├── PipelineArrowLabel "→"
    ├── PipelineStepButton "Deskew"
    ├── PipelineArrowLabel "→"
    ├── PipelineStepButton "Margins"
    ├── PipelineArrowLabel "→"
    ├── PipelineStepButton "Dewarp"
    ├── PipelineArrowLabel "→"
    ├── PipelineStepButton "Binarize"
    ├── PipelineArrowLabel "→"
    └── PipelineStepButton "Output"
```

Cada `PipelineStepButton` puede ser un `QPushButton` o `QToolButton` con `setCheckable(true)`; el activo tiene `setChecked(true)`.

### 7.4 Left sidebar (Thumbnails)

```
QWidget
└── QVBoxLayout (spacing 4, margins 6)
    ├── QToolButton (focusButton, icon keep-in-view, checkable)
    ├── QGraphicsView (thumbView) — contenido por ThumbnailSequence
    └── QComboBox (sortOptions)
```

### 7.5 Right sidebar (Parameters)

```
QScrollArea (widgetResizable true, frameShape NoFrame)
└── QWidget scrollContent
    └── QVBoxLayout
        └── FilterOptionsWidget (actual del stage)
            └── [contenido dinámico por filtro]
                ├── QGroupBox o CollapsibleSection "Presets"
                │   └── QHBoxLayout (preset buttons)
                ├── QGroupBox "Opciones"
                │   └── controles (sliders, spins, checks)
                └── QWidget "Aplicar"
                    └── QPushButton "A página actual" "A selección" "A todas"
```

### 7.6 Bottom panel

```
QWidget (estilo: fondo #FAFAFA, borde superior 1px #E5E5E7)
└── QHBoxLayout (margins 8, spacing 8)
    ├── QPushButton "Procesar selección"
    ├── QPushButton "Procesar todo"
    ├── QPushButton "Exportar"
    ├── QWidget (stretch)
    ├── QProgressBar + QLabel "Página X / N"
    └── QPushButton "Detener"
```

---

## 8. Guías de componentes UI

### 8.1 Botones

- **Primario**: fondo acento (#2563EB), texto blanco, hover más oscuro.
- **Secundario**: fondo #F0F0F2, texto #1A1A1A, hover #E8E8EA.
- **Terciario**: solo borde o texto, sin relleno fuerte.
- Altura estándar: 32–36 px. Border-radius: 6–8 px. Padding horizontal: 12–16 px.

### 8.2 Sliders

- Altura pista: 6 px. Handle: 16×16 px, circular o redondeado.
- Color pista: #E5E5E7; relleno (range): acento. Handle: blanco + sombra sutil.
- Junto al slider: QSpinBox o QLineEdit con valor numérico (sincronizado).

### 8.3 Secciones colapsables

- Título: QToolButton con icono flecha (▼/▶), texto del grupo.
- Contenido: QWidget en QVBoxLayout; visibility toggled al clic.
- Animación opcional: QPropertyAnimation sobre maximumHeight.

### 8.4 Lista de miniaturas

- Ítem: miniatura (pixmap) + número de página (QLabel).
- Selección: fondo #E8E8EA o borde izquierdo 3 px acento.
- Hover: fondo #F5F5F6. Sin rejilla pesada entre ítems.

### 8.5 Command palette

- QDialog o QMenu no modal, centrado o bajo la barra.
- QLineEdit con placeholder "Buscar acción...".
- QListView o lista de QAction; filtrado por texto.
- Enter ejecuta acción seleccionada; Escape cierra.

### 8.6 Iconos

- Tamaño estándar en toolbar: 20–24 px.
- Estilo: outline/lineal, peso 1.5–2.
- Estados: normal, hover, disabled, checked (acento).
- Formatos: SVG preferido para escalado en HDPI.

---

## 9. Referencia a archivos actuales

| Concepto actual        | Archivo / clase              | Uso en rediseño                          |
|------------------------|------------------------------|------------------------------------------|
| Ventana principal      | MainWindow.cpp/.h, MainWindow.ui | Reorganizar layout; pipeline en toolbar |
| Lista de etapas        | StageListView                | Reemplazar por PipelineStripWidget      |
| Miniaturas             | ThumbnailSequence + thumbView | Mover a panel izquierdo                 |
| Opciones de filtro     | FilterOptionsWidget (por filtro) | Panel derecho, mismo contenido          |
| Preview                | ImageViewBase, ImageView*    | Centro; añadir grid, before/after       |
| Batch                  | createBatchProcessingWidget  | Bajar a BottomPanelWidget               |
| Proyecto vacío         | NewOpenProjectPanel          | Mismo contenido en center stack         |

---

## 10. Archivos de referencia incluidos

- **docs/UI_REDESIGN_2026_QT_SNIPPETS.cpp** — Fragmentos C++ para layout (toolbar, pipeline strip, paneles, command palette, sección colapsable).
- **docs/UI_REDESIGN_2026_STYLES.qss** — Hoja de estilos QSS para el sistema visual 2026 (colores, botones, sliders, inputs, paneles).
