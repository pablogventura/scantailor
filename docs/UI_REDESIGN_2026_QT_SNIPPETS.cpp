/*
 * Snippets Qt para el rediseño UI 2026 — ScanTailor
 * Solo referencia; integrar según convenga en el proyecto.
 * No compila standalone: faltan includes y contexto.
 */

// =============================================================================
// 1. Estructura principal del central widget (pseudocódigo C++)
// =============================================================================

void MainWindow::setupRedesignedLayout()
{
    QWidget* central = new QWidget(this);
    QVBoxLayout* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- Top toolbar ---
    QWidget* topBar = createTopToolbar();
    topBar->setFixedHeight(48);
    rootLayout->addWidget(topBar);

    // --- Main content: splitter ---
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal);

    // Left: thumbnails
    QWidget* leftPanel = createThumbnailPanel();
    leftPanel->setMinimumWidth(220);
    leftPanel->setMaximumWidth(400);
    mainSplitter->addWidget(leftPanel);

    // Center: preview (stacked)
    QFrame* centerFrame = new QFrame();
    centerFrame->setFrameShape(QFrame::NoFrame);
    centerFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pImageFrameLayout = new QStackedLayout(centerFrame);
    mainSplitter->addWidget(centerFrame);

    // Right: parameters
    QScrollArea* rightScroll = new QScrollArea();
    rightScroll->setWidgetResizable(true);
    rightScroll->setFrameShape(QFrame::NoFrame);
    rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget* rightContent = new QWidget();
    m_pOptionsFrameLayout = new QStackedLayout(rightContent);
    rightScroll->setWidget(rightContent);
    rightContent->setMinimumWidth(260);
    rightContent->setMaximumWidth(420);
    mainSplitter->addWidget(rightScroll);

    // Proporciones iniciales del splitter (ej.: 0.2, 0.6, 0.2 del ancho)
    QList<int> sizes;
    sizes << 240 << 800 << 300;  // ajustar según preferencia
    mainSplitter->setSizes(sizes);

    rootLayout->addWidget(mainSplitter, 1);

    // --- Bottom panel ---
    QWidget* bottomPanel = createBottomPanel();
    bottomPanel->setFixedHeight(56);
    rootLayout->addWidget(bottomPanel);

    setCentralWidget(central);
}

// =============================================================================
// 2. Pipeline strip (barra de etapas horizontal)
// =============================================================================

QWidget* MainWindow::createPipelineStrip()
{
    QWidget* strip = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(strip);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(2);

    const QStringList stageNames = {
        tr("Split"), tr("Deskew"), tr("Margins"), tr("Dewarp"),
        tr("Binarize"), tr("Output")
    };

    for (int i = 0; i < stageNames.size(); ++i) {
        if (i > 0) {
            QLabel* arrow = new QLabel(QString::fromUtf8("→"));
            arrow->setStyleSheet("color: #9CA3AF; font-size: 12px;");
            layout->addWidget(arrow);
        }
        QToolButton* btn = new QToolButton();
        btn->setText(stageNames[i]);
        btn->setCheckable(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btn->setProperty("stageIndex", i);
        connect(btn, &QToolButton::clicked, this, [this, i]() { selectStage(i); });
        m_pipelineButtons.append(btn);
        layout->addWidget(btn);
    }
    layout->addStretch();
    return strip;
}

// =============================================================================
// 3. Panel de miniaturas (izquierda)
// =============================================================================

QWidget* MainWindow::createThumbnailPanel()
{
    QWidget* panel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    QToolButton* focusBtn = new QToolButton();
    focusBtn->setIcon(QIcon(":/icons/keep-in-view.png"));
    focusBtn->setCheckable(true);
    focusBtn->setChecked(true);
    focusBtn->setToolTip(tr("Keep current page in view"));
    layout->addWidget(focusBtn);

    QGraphicsView* thumbView = new QGraphicsView();
    thumbView->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    thumbView->setBackgroundBrush(QColor(0xF5, 0xF5, 0xF6));
    thumbView->setStyleSheet("border: none; border-radius: 6px;");
    layout->addWidget(thumbView, 1);

    QComboBox* sortOptions = new QComboBox();
    layout->addWidget(sortOptions);

    // Conectar con ThumbnailSequence existente
    m_ptrThumbSequence->attachView(thumbView);
    // ... resto de conexiones (focusButton, sortOptions)

    return panel;
}

// =============================================================================
// 4. Panel inferior (batch/export)
// =============================================================================

QWidget* MainWindow::createBottomPanel()
{
    QWidget* panel = new QWidget();
    panel->setObjectName("bottomPanel");
    QHBoxLayout* layout = new QHBoxLayout(panel);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(8);

    QPushButton* processSelection = new QPushButton(tr("Process selection"));
    QPushButton* processAll = new QPushButton(tr("Process all"));
    QPushButton* exportBtn = new QPushButton(tr("Export"));
    layout->addWidget(processSelection);
    layout->addWidget(processAll);
    layout->addWidget(exportBtn);

    layout->addStretch();

    QLabel* progressLabel = new QLabel(tr("Page 0 / 0"));
    progressLabel->setObjectName("batchProgressLabel");
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setMaximumWidth(200);
    progressBar->setTextVisible(true);
    layout->addWidget(progressLabel);
    layout->addWidget(progressBar);

    QPushButton* stopBtn = new QPushButton(tr("Stop"));
    stopBtn->setObjectName("batchStopButton");
    layout->addWidget(stopBtn);

    connect(processAll, &QPushButton::clicked, this, &MainWindow::startBatchProcessing);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::stopBatchProcessing);

    return panel;
}

// =============================================================================
// 5. Command palette (simplificado)
// =============================================================================

void MainWindow::showCommandPalette()
{
    QDialog* dlg = new QDialog(this, Qt::Popup);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* layout = new QVBoxLayout(dlg);

    QLineEdit* search = new QLineEdit();
    search->setPlaceholderText(tr("Search action..."));
    search->setClearButtonEnabled(true);
    layout->addWidget(search);

    QListView* list = new QListView();
    QStringListModel* model = new QStringListModel(QStringList()
        << tr("Go to first page") << tr("Go to last page")
        << tr("Process all") << tr("Save project") << tr("Open project"));
    list->setModel(model);
    layout->addWidget(list);

    connect(search, &QLineEdit::textChanged, [list, model](const QString& text) {
        // Filtrar modelo según text
        (void)list; (void)model;
    });
    connect(list, &QListView::activated, [this, dlg](const QModelIndex& idx) {
        // Ejecutar acción según idx.row()
        (void)idx;
        dlg->accept();
    });

    dlg->resize(320, 280);
    dlg->exec();
}

// =============================================================================
// 6. Sección colapsable para parámetros
// =============================================================================

class CollapsibleSection : public QWidget
{
    Q_OBJECT
public:
    CollapsibleSection(const QString& title, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 4, 0, 4);

        m_toggle = new QToolButton();
        m_toggle->setText(title);
        m_toggle->setCheckable(false);
        m_toggle->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        m_toggle->setIcon(QIcon());  // flecha ▼/▶ según estado
        m_toggle->setStyleSheet(
            "QToolButton { text-align: left; border: none; padding: 6px 8px; "
            "border-radius: 4px; } QToolButton:hover { background: #E8E8EA; }"
        );
        layout->addWidget(m_toggle);

        m_content = new QWidget();
        m_contentLayout = new QVBoxLayout(m_content);
        m_contentLayout->setContentsMargins(8, 0, 0, 0);
        layout->addWidget(m_content);

        connect(m_toggle, &QToolButton::clicked, this, [this]() {
            m_content->setVisible(!m_content->isVisible());
            // Actualizar icono flecha
        });
    }

    void addWidget(QWidget* w) { m_contentLayout->addWidget(w); }
    void addLayout(QLayout* l) { m_contentLayout->addLayout(l); }

private:
    QToolButton* m_toggle;
    QWidget* m_content;
    QVBoxLayout* m_contentLayout;
};
